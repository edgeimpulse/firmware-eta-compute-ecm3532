#include "config.h"
#include "eta_csp_dsp.h"
#include "print_util.h"
#include "rpc.h"
#include "incbin.h"
#include "module_common.h"
#include <stdint.h>

extern tDspMem dsp_mem;

//
// This MACRO returns 1 byte from a dword and increments the address accordingly
// if it is in X/YMEM or SRAM or FLASH
//
#define ReturnByteInDword                                                     \
    ((ui32TempWord >> (ui8DwordOffset * 8)) & 0xff);                          \
    ui8DwordOffset++;                                                         \
    if(((bAddrIsInXymem) && (ui8DwordOffset == 2)) ||                         \
       ((!bAddrIsInXymem) && (ui8DwordOffset == 4)))                          \
    {                                                                         \
        ui32CurrAddr = ui32CurrAddr + 4;                                      \
        ui8DwordOffset = 0;                                                   \
        ui32TempWord = *((uint32_t *)ui32CurrAddr);                           \
    }




/***************************************************************************//**
 *
 *  EtaCspDspPrintViaM3Ex - Routine to print to UART from DSP memory.
 *
 *  @param ui32Addr is the system address.
 *
 ******************************************************************************/
__attribute__((section(".initSection")))
void EtaCspDspPrintViaM3Ex(uint32_t ui32Addr)
{
    ecm35xx_printf("Inside EtaCspDspPrintViaM3Ex, " \
            "ui32Addr=%x\r\n", ui32Addr);

    //
    // DSP XMEM / YMEM is a little odd.  It's a 16 bit word, but zero extended
    // to look like a 32 bit word (makes implementation easier on DSP). But,
    // strings can start at any offset. So we'll take the initial offset and go
    // from there.
    //
    // Note, one could easily speed things up by reading one 16 bit word once,
    // and using both bytes to transmit.  But we are going out to a UART, so
    // speeding up seems silly.
    //
    uint32_t ui32Timeout = 250; // max char to print at one time
    uint32_t ui32CharCount = 0;
    uint8_t ui8Temp;
    uint32_t ui32TempWord;
    uint32_t ui32TempAddr;
    uint32_t ui32CurrAddr;
    uint8_t ui8DwordOffset;
    bool bAddrIsInXymem = true;

    ecm35xx_printf("DSPPRINT:");

    if(bAddrIsInXymem)
    {
        ui32CurrAddr = ui32Addr & ~0x1;
        ui8DwordOffset = ui32Addr &  0x1;
    }
    else
    {
        ui32CurrAddr = ui32Addr & ~0x3;
        ui8DwordOffset = ui32Addr &  0x3;
    }

    //
    // Read next dword (prime pump).
    //
    ui32TempWord = *((uint32_t *)ui32CurrAddr);

    do
    {
        ui8Temp = ReturnByteInDword;
        ui32CharCount++;

        //
        // Code word indicating we have a buffer rollover, next 4 bytes is new
        // address to start from.
        //
        if(ui8Temp == MAILBOX_DSP2M3CMD_PWRAP)
        {
            ui8Temp = ReturnByteInDword;
            ui32TempAddr = (ui8Temp << 24);

            ui8Temp = ReturnByteInDword;
            ui32TempAddr |= (ui8Temp << 16);

            ui8Temp = ReturnByteInDword;
            ui32TempAddr |= (ui8Temp << 8);

            ui8Temp = ReturnByteInDword;
            ui32TempAddr |= (ui8Temp << 0);

            if(bAddrIsInXymem)
            {
                ui32CurrAddr = ui32TempAddr & ~0x1;
                ui8DwordOffset = ui32TempAddr &  0x1;
            }
            else
            {
                ui32CurrAddr = ui32TempAddr & ~0x3;
                ui8DwordOffset = ui32TempAddr &  0x3;
            }

            //
            // Read next dword (prime pump).
            //
            ui32TempWord = *((uint32_t *)ui32CurrAddr);

            ecm35xx_printf("..ROLL ui32CurrAddr:%x..",
                    ui32CurrAddr);
            ui8Temp = ReturnByteInDword;
        }

        if(((0x20 <= ui8Temp) && (ui8Temp <= 0x7e)) | // printable characters
                (ui8Temp == 0x0a)                        | // LF
                (ui8Temp == 0x0d)) // CR
        {
            ecm35xx_printf("%c", ui8Temp);
        }

        //
        // Don't print actual end of line.
        //
        else if(ui8Temp == '\0')
        {
            ecm35xx_printf("..LASTaddr+1..%x\r\n", ui32CurrAddr);
            continue;
        }
        else
        {
            ecm35xx_printf("\r\nUNPRINTABLE DSP CHAR:0x%x\r\n", ui8Temp);
            while(1)
            {
            }
        }
    }
    while((ui8Temp != '\0') && (--ui32Timeout));

    if(ui32Timeout == 0)
    {
        ecm35xx_printf("\r\nWARNING: Had to abort DSP print, over 250" \
                "characters \r\n");
        return;
    }
}

__attribute__((section(".initSection"))) void LoadDsp(void)
{
    extern void* incbin_dsp_start;
    tDspMem* dspPtr = 0;
    dspPtr = (tDspMem*)(&incbin_dsp_start);
    EtaCspDspLoaderSimple (dspPtr);
}

int DspDbgEventHandler(uint32_t low32, uint32_t high32)
{
    EtaCspDspPrintViaM3Ex(high32);
    return 0;
}

void DspdbgInit(void)
{
    rpcRegisterEventCb(RPC_MODULE_ID_DEBUG, DspDbgEventHandler);
}
