/***************************************************************************//**
 *
 * @file eta_csp_dsp.c
 *
 * @brief This file contains eta_csp_dsp module implementations.
 *
 * Copyright (C) 2018 Eta Compute, Inc
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * This is part of revision 1.0.0a6 of the Tensai Software Development Kit.
 *
 ******************************************************************************/

/***************************************************************************//**
 * @addtogroup ecm3532dsp-m3 Digital Signal Processor (DSP)
 * @ingroup ecm3532csp-m3
 * @{
 ******************************************************************************/

#include <string.h>
#include <stdlib.h>
#include "eta_chip.h"
#include "eta_csp_isr.h"
#include "eta_csp_nvic.h"
#include "eta_csp_uart.h"
#include "eta_csp_dsp.h"
#include "eta_csp_io.h"
#include "memio.h"
#include "eta_csp_i2s.h"
#include "eta_csp_pdm.h"
#include "eta_csp_buck.h"

#define DSPPRINT_DEBUG if(0)

//
// The default mailbox handler.
//
static tpfMboxHandler pfnDefaultMboxHandler =
{
    EtaCspDspMboxDefaultHandler
};

/***************************************************************************//**
 *
 *  EtaCspDspPowerUp - Power Up DSP Complex
 *
 ******************************************************************************/
void
EtaCspDspPowerUp(void)
{
    //
    // Power Up the Buck Regulator
    //
    REG_W1(RTC_PWR_SET,DSP_BUCK,1);

    //
    // Wait for Buck to be Ready 4 inductor mode only
    //
    if( ! REG_RTC_AO_CSR2.BF.TWO_INDUCTOR_MODE)
    {
        while(REG_RTC_BUCK_DSP_CSR.BF.READY == 0);
    }

    //
    // Power On DSP Core
    //
    REG_M1(RTC_PWR_SET,DSP,1);

    //
    // Issue fence to make sure prev write finished
    // NOTE this was a write to the RTC
    //
    // fixme remove after testing REG_GPIO_DEBUG0_READ_ONLY.V = REG_RTC_PWR_SET.V;
    EtaCspRtcFenceFast();

    //
    // wait for DSP power domain to be stable
    //
    while(REG_RTC_BUCK_DSP_CSR.BF.STABLE == 0);

    //
    // Take the DSP Complex out of reset
    //
    REG_W1 (SOCCTRL_DSP_CSR, DSP_COMPLEX_RESET_N, 1);

    //
    // Set the DSP Buck to known good initial voltages
    //
    EtaCspBuckDspVoltageSet(eBuckDspVoltage700Mv, true);

}

/***************************************************************************//**
 *
 *  EtaCspDspPowerDown - Power Down DSP Complex
 *
 ******************************************************************************/
void
EtaCspDspPowerDown(void)
{
    //
    // Prepare the DSP Buck before turning off the switch and buck
    //
    EtaCspBuckDspPrePowerDown();

    //
    // Power Down the Buck Regulator first
    //
    REG_W1(RTC_PWR_CLEAR,DSP_BUCK,1);

    //
    // Issue fence to make sure prev write finished
    //
    // fixme remove after testing REG_GPIO_DEBUG0_READ_ONLY.V = REG_RTC_PWR_CLEAR.V;
    EtaCspRtcFenceFast();

    //
    // Assert the DSP Complex reset for the next powerup cycle
    //
    REG_W1 (SOCCTRL_DSP_CSR, DSP_COMPLEX_RESET_N, 0);

    //
    // Turn off the switch to the DSP core last
    //
    REG_M1(RTC_PWR_CLEAR,DSP,1);

    //
    // Issue fence to make sure prev write finished
    //
    // fixme remove after testing REG_GPIO_DEBUG0_READ_ONLY.V = REG_RTC_PWR_CLEAR.V;
    EtaCspRtcFenceFast();

}

/***************************************************************************//**
 *
 *  EtaCspDspLoaderSimple - Simple, no frills, just do it loader.
 *
 *  @param psDspMem is a pointer to a DSP structure.
 *
 ******************************************************************************/
void
EtaCspDspLoaderSimple(const tDspMem *psDspMem)
{
    bool bQuiet, bCheckData, bLeaveInReset;

    //
    // Set the flags.
    //
    bQuiet = true;
    bCheckData = true;
    bLeaveInReset = false;

    //
    // Call the main loader.
    //
    EtaCspDspLoaderMain(bQuiet, bCheckData, bLeaveInReset, psDspMem);
}

/***************************************************************************//**
 *
 *  EtaCspDspLoaderMain - The main dsp loader.
 *
 *  @param bQuiet set to disable prints.
 *  @param bCheckData set to enable readback and check of P,X,Ymem.
 *  @param bLeaveInReset set to leave the DSP core in reset (accessing memories
 *                       in DSP complex necessitates being out of reset)
 *  @param psDspMem is a pointer to a DSP structure.
 *  @return Returns the result of EtaCspDspLoaderCheck if bCheckData is set or 0
 *          otherwise.
 *
 *  @note If check is bad, we leave DSP in reset.
 *
 ******************************************************************************/
uint32_t
EtaCspDspLoaderMain(bool bQuiet, bool bCheckData, bool bLeaveInReset,
                    const tDspMem *psDspMem)
{
    uint32_t ui32Ret = 0;
    uint32_t ui32Index0;
    uint32_t ui32WriteData;
    const uint32_t *pui32Source;
    const uint16_t *pui16Source;
    uint32_t *pui32Dest;
    uint32_t ui32Counter;


    //
    // Take DSP out of power down.
    //     Require this to be done before coming here
    //
    // EtaCspDspPowerUp();

    //
    // Get DSP ready for loading FW.
    //
    REG_SOCCTRL_DSP_CSR.BF.DSP_COMPLEX_RESET_N = 1; // Enable DSP enough to
                                                    // access memories.

    //
    // Put up a fence for DSP control writes
    //
    EtaCspDspFenceFast();
    // fixme remove after testing REG_SOCCTRL_DEBUG_READ_ONLY.V = REG_SOCCTRL_DSP_CSR.V; // fence

    REG_DSPCTRL_M3_DSP_CTRL.BF.RESET_DSP = 1; // Put DSP in reset (probably was
                                              // already there, but be safe)

    #ifndef FOR_SIMULATION
    //
    // Load PMEM.
    //

    pui32Source = psDspMem->pui32PmemData;
    pui32Dest = (uint32_t *)DSP_PMEM_START;
    ui32Counter = 0;
    for(ui32Index0 = 0; ui32Index0 < psDspMem->ui32PmemSize; ui32Index0++)
    {
        ui32WriteData = *pui32Source;
        *pui32Dest = ui32WriteData;
        pui32Dest++;
        pui32Source++;
        ui32Counter++;

    }

    //
    // Load XMEM.
    //
    pui16Source = psDspMem->pui16XmemData;
    pui32Dest = (uint32_t *)DSP_XMEM_START;
    for(ui32Index0 = 0; ui32Index0 < psDspMem->ui32XmemSize; ui32Index0++)
    {
        ui32WriteData = (uint32_t)*pui16Source;
        *pui32Dest = ui32WriteData;
        pui32Dest++;
        pui16Source++;
    }

    //
    // Load YMEM.
    //
    pui16Source = psDspMem->pui16YmemData;
    pui32Dest = (uint32_t *)DSP_YMEM_START;
    for(ui32Index0 = 0; ui32Index0 < psDspMem->ui32YmemSize; ui32Index0++)
    {
        ui32WriteData = (uint32_t)*pui16Source;
        *pui32Dest = ui32WriteData;
        pui32Dest++;
        pui16Source++;
    }
    #endif // #ifndef FOR_SIMULATION

    //
    // Debug code (introduce errors)
    // MEMIO32(DSP_PMEM_START+42*4)  = 0x1234567; // word 0x2a
    // MEMIO32(DSP_XMEM_START+24*4)  = 0x4567;    // word 0x18

    //
    // Check Data.
    //
    if(bCheckData)
    {
        ui32Ret = EtaCspDspLoaderCheck(bQuiet, 1, psDspMem);
    }
    else
    {
        ui32Ret = 0;
    }

    if(bLeaveInReset)
    {
    }
    else if(ui32Ret != 0)
    {
        EtaCspIoPrintf("  Errors detected. Leaving DSP in reset.\r\n");
    }
    else
    {
        //
        // Take DSP out of reset.
        //
        // EtaCspIoPrintf("Taking DSP out of reset\r\n");
        REG_DSPCTRL_M3_DSP_CTRL.BF.RESET_DSP = 0;
    }

    return(ui32Ret);
}

/***************************************************************************//**
 *
 *  EtaCspDspLoaderCheck - The DSP loader checking program.
 *
 *  @param bQuiet set to disable prints.
 *  @param bDisplayErr set to get detailed list of errors (can be very verbose).
 *  @param psDspMem is a pointer to a DSP structure.
 *  @return Return the count of successful loads.
 *
 ******************************************************************************/
uint32_t
EtaCspDspLoaderCheck(bool bQuiet, bool bDisplayErr, const tDspMem *psDspMem)
{
    uint32_t ui32Ret = 0;
    uint32_t ui32Index0;
    uint32_t ui32ReadData;
    uint32_t ui32ExpData;
    const uint32_t *pui32Exp;
    const uint16_t *pui16Exp;
    uint32_t *pui32Act;
    uint32_t ui32PmemErr = 0;
    uint32_t ui32XmemErr = 0;
    uint32_t ui32YmemErr = 0;


    //
    // Load PMEM.
    //
    pui32Exp = psDspMem->pui32PmemData;
    pui32Act = (uint32_t *)DSP_PMEM_START;
    for(ui32Index0 = 0; ui32Index0 < psDspMem->ui32PmemSize; ui32Index0++)
    {
        ui32ExpData = *pui32Exp;
        ui32ReadData = (*pui32Act) & 0xffffffff;
        if(ui32ExpData != ui32ReadData)
        {
            ui32Ret++;
            ui32PmemErr++;
            if(bDisplayErr)
            {
                EtaCspIoPrintf("  ERROR: PMEM [0x%x] act:0x%x exp:0x%x\r\n",
                               ui32Index0, ui32ReadData, ui32ExpData);
            }
        }

        pui32Act++;
        pui32Exp++;
    }

    //
    // Load XMEM.
    //
    pui16Exp = psDspMem->pui16XmemData;
    pui32Act = (uint32_t *)DSP_XMEM_START;
    for(ui32Index0 = 0; ui32Index0 < psDspMem->ui32XmemSize; ui32Index0++)
    {
        ui32ExpData = *pui16Exp;
        ui32ReadData = (*pui32Act) & 0xffff;
        if(ui32ExpData != ui32ReadData)
        {
            ui32Ret++;
            ui32XmemErr++;
            if(bDisplayErr)
            {
                EtaCspIoPrintf("  ERROR: XMEM [0x%x] act:0x%x exp:0x%x\r\n",
                               ui32Index0, ui32ReadData, ui32ExpData);
            }
        }
        pui32Act++;
        pui16Exp++;
    }

    //
    // Load YMEM.
    //
    pui16Exp = psDspMem->pui16YmemData;
    pui32Act = (uint32_t *)DSP_YMEM_START;
    for(ui32Index0 = 0; ui32Index0 < psDspMem->ui32YmemSize; ui32Index0++)
    {
        ui32ExpData = *pui16Exp;
        ui32ReadData = (*pui32Act) & 0xffff;
        if(ui32ExpData != ui32ReadData)
        {
            ui32Ret++;
            ui32YmemErr++;
            if(bDisplayErr)
            {
                EtaCspIoPrintf("  ERROR: YMEM [0x%x] act:0x%x exp:0x%x\r\n",
                               ui32Index0,
                               ui32ReadData, ui32ExpData);
            }
        }
        pui32Act++;
        pui16Exp++;
    }


    //
    // Return the result.
    //
    return(ui32Ret);
}


/***************************************************************************//**
 *
 * EtaCspDspPrintViaM3Init - Initialization routine for M3 print.  At the
 *                            moment, it simply clears existing DSP mailbox
 *                            interrupts.
 *
 ******************************************************************************/
void
EtaCspDspPrintViaM3Init(void)
{
    EtaCspNvicIntClear(eIrqNumDsp);
    EtaCspNvicIntEnable(eIrqNumDsp);
    EtaCspIsrHandlerSet(eIrqNumDsp, EtaCspDspMboxM3Receive);
}

/***************************************************************************//**
 *
 *  EtaCspDspMboxDataSend - Send data to the mailbox.
 *
 *  @param ui32Data is a 32 bit word to send to the DSP.
 *
 *  @note This task will stall if the DSP is taking its time to clear previous
 *        mailboxes.
 *
 ******************************************************************************/
void
EtaCspDspMboxDataSend(uint32_t ui32Data)
{
    uint32_t ui32MsgTimeout = 100000;

    //
    // Print.
    //
    // EtaCspIoPrintf("M3 EtaCspDspMboxDataSend(0x%x)\r\n",data);

    //
    // We wait until DSP has read previous mailbox (if any)
    // addr 0x4 will be a 1 if the mailbox is busy.
    // Note we DO NOT timeout, only print a message saying why we are waiting.
    //
    do
    {
        if(--ui32MsgTimeout == 0)
        {
            ui32MsgTimeout = 100000;
            EtaCspIoPrintf("M3 waiting for DSP_MAILBOX to be not busy. " \
                           "curr value = %x\r\n", REG_MBOX_M3_M32DSP_STATUS.V);
        }
    }
    while(REG_MBOX_M3_M32DSP_STATUS.V == 1);

    REG_MBOX_M3_M32DSP_LOWER.V = ui32Data;
}

/***************************************************************************//**
 *
 *  EtaCspDspMboxCmdSend - Send a command to the DSP.
 *
 *  @param ui16Cmd is 16 bit word we send to the DSP
 *  @param ui32Data is a 32 bit word we send to the DSP.
 *
 *  @note This task will stall if the DSP is taking its time to clear previous
 *        mailboxes.
 ******************************************************************************/
void
EtaCspDspMboxCmdSend(uint16_t ui16Cmd, uint32_t ui32Data)
{
    uint32_t ui32MsgTimeout = 100000;

    //
    // Print.
    //
    // EtaCspIoPrintf("M3 EtaCspDspMboxDataSend(0x%x)\r\n",data);

    //
    // We wait until DSP has read previous mailbox (if any)
    // addr 0x4 will be a 1 if the mailbox is busy.
    // Note we DO NOT timeout, only print a message saying why we are waiting.
    //
    do
    {
        if(--ui32MsgTimeout == 0)
        {
            ui32MsgTimeout = 100000;
            EtaCspIoPrintf("M3 waiting for DSP_MAILBOX to be not busy. " \
                           "curr value = %x\r\n", REG_MBOX_M3_M32DSP_STATUS.V);
        }
    }
    while(REG_MBOX_M3_M32DSP_STATUS.V == 1);

    REG_MBOX_M3_M32DSP_EXTEND.V = ui32Data;
    REG_MBOX_M3_M32DSP_LOWER.V = ui16Cmd;

    //
    // Issue fence to make sure prev write finished
    //
    ETA_CSP_DSP_MBOX_FENCE();
    // fixme remove after testing REG_GPIO_DEBUG0_READ_ONLY.V = REG_MBOX_M3_M32DSP_LOWER.V;
}

/***************************************************************************//**
 *
 *  EtaCspDspPrintAddrInc - Routine that turns the DSP memory addresses into a
 *                          linear address.  It takes the current SYSTEM address
 *                          (ui32CurrAddress) and returns the ui32CurrAddress
 *                          incremented by 1.
 *
 *  @param ui32CurrAddress current SYSTEM address.
 *  @return Return ui32CurrAddress incremented by one.
 *
 ******************************************************************************/
uint32_t
EtaCspDspPrintAddrInc(uint32_t ui32CurrAddress)
{
    uint32_t ui32NewAddr;

    ui32NewAddr = ui32CurrAddress + 1;

    if((ui32NewAddr & 0x3) >= 0x2)
    {
        ui32NewAddr = (ui32NewAddr & (~0x3)) + 4;
    }

    return(ui32NewAddr);
}

/***************************************************************************//**
 *
 *  EtaCspDspMboxDefaultHandler - The default mailbox handler.
 *
 *  @param ui16RxMboxCmd is the received mailbox command.
 *  @param ui32RxMboxData is the received mailbox data.
 *
 ******************************************************************************/
void
EtaCspDspMboxDefaultHandler(uint16_t ui16RxMboxCmd, uint32_t ui32RxMboxData)
{
    EtaCspIoPrintf("ERROR: Unknown DSP mailbox command:%x with data:%x\r\n",
                   ui16RxMboxCmd, ui32RxMboxData);
//
// Keil.
//
#ifdef __CC_ARM
    __breakpoint(0);

//
// IAR.
//
#elif __ICCARM__
    asm volatile("BKPT #0");

//
// GCC.
//
#else
    __asm__ __volatile__ ("BKPT #0");
#endif
}

/***************************************************************************//**
 *
 *  EtaCspDspMboxDefaultHandlerSet - Set the default mailbox handler.
 *
 *  @param pfnMboxHandler is a pointer to the desired new mailbox handler.
 *
 ******************************************************************************/
void
EtaCspDspMboxDefaultHandlerSet(tpfMboxHandler pfnMboxHandler)
{
    pfnDefaultMboxHandler = pfnMboxHandler;
}

/***************************************************************************//**
 *
 *  EtaCspDspMboxM3Receive - The routine to processed received mailboxes.
 *
 *  @param iIrqNum is the interrupt number.
 *
 ******************************************************************************/
void
EtaCspDspMboxM3Receive(tIrqNum iIrqNum)
{
    DSPPRINT_DEBUG EtaCspIoPrintf("M3 inside DSP_MAILBOX interrupt\r\n");

    uint16_t ui16RxMboxCmd;
    uint32_t ui32RxMboxData;

    //
    // Read DSP mailbox, clear interrupt.
    //
    ui32RxMboxData = REG_MBOX_M3_DSP2M3_MBOX_EXTEND.V;
    ui16RxMboxCmd = REG_MBOX_M3_DSP2M3_MBOX_LOWER.V;
    DSPPRINT_DEBUG EtaCspIoPrintf("..M3 RX MBOX=%x..", ui16RxMboxCmd);

    //
    // Clear NVIC interrupt
    //
    // REG_NVIC_EICPR0.BF.DSP = 1;
    EtaCspNvicIntClear(iIrqNum);

    //
    // ui16RxMboxCmd must be 1 byte command OR we must deal with non-16 bit
    // allignment.
    //
    switch(ui16RxMboxCmd & 0xff)
    {
        case (MAILBOX_DSP2M3CMD_PRINTVIAM3):
        {
            EtaCspDspPrintViaM3(ui32RxMboxData);
            break;
        }

        case (MAILBOX_DSP2M3CMD_I2S_CFG):
        {
            EtaCspI2sConfigUnpack(ui32RxMboxData);
            break;
        }


        case (MAILBOX_DSP2M3CMD_PDM_CFG):
        {
            EtaCspPdmConfigUnpack(ui32RxMboxData);
            break;
        }

        default:
        {
            pfnDefaultMboxHandler(ui16RxMboxCmd, ui32RxMboxData);
            break;
        }
    }
}

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
    }                                                                         \
    DSPPRINT_DEBUG EtaCspIoPrintf("\r\ntempword:%x..ui32CurrAddr:%x "         \
                                  " ui8DwordOffset:%x cTemp:%x    :%c ",      \
                                  ui32TempWord, ui32CurrAddr, ui8DwordOffset, \
                                  ui8Temp, ui8Temp);

/***************************************************************************//**
 *
 *  EtaCspDspPrintViaM3 - Routine to print to UART from DSP memory.
 *
 *  @param ui32Addr is the system address.
 *
 ******************************************************************************/
void
EtaCspDspPrintViaM3(uint32_t ui32Addr)
{
    DSPPRINT_DEBUG EtaCspIoPrintf("Inside EtaCspDspPrintViaM3, " \
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

    EtaCspIoPrintf("DSPPRINT:");

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

            DSPPRINT_DEBUG EtaCspIoPrintf("..ROLL ui32CurrAddr:%x..",
                                          ui32CurrAddr);
            ui8Temp = ReturnByteInDword;
        }

        if(((0x20 <= ui8Temp) && (ui8Temp <= 0x7e)) | // printable characters
           (ui8Temp == 0x0a)                        | // LF
           (ui8Temp == 0x0d)) // CR
        {
            EtaCspIoPrintf("%c", ui8Temp);
        }

        //
        // Don't print actual end of line.
        //
        else if(ui8Temp == '\0')
        {
            DSPPRINT_DEBUG EtaCspIoPrintf("..LASTaddr+1..%x\r\n", ui32CurrAddr);
            continue;
        }
        else
        {
            EtaCspIoPrintf("\r\nUNPRINTABLE DSP CHAR:0x%x\r\n", ui8Temp);
            while(1)
            {
            }
        }
    }
    while((ui8Temp != '\0') && (--ui32Timeout));

    if(ui32Timeout == 0)
    {
        EtaCspIoPrintf("\r\nWARNING: Had to abort DSP print, over 250" \
                       "characters \r\n");
        return;
    }

    DSPPRINT_DEBUG EtaCspIoPrintf("M3 returning 0x %x credits \r\n",
                                  ui32CharCount);
    EtaCspDspMboxDataSend(MAILBOX_M32DSPCMD_RETURNCRED | (ui32CharCount << 8));
}

/***************************************************************************//**
 *
 *  EtaCspDspSetSemaphore - Set the semaphore to a 1.
 *
 *  Note: For eSemaphoreReadRequest0-3, this command will poll until
 *        the semaphore is not reserved so we can set it to 1.
 *
 ******************************************************************************/
void EtaCspDspSetSemaphore(tSemaphoreType semaphoreType)
{
  #ifdef SEMAPHORE_TIMEOUT_ENABLE
  uint32_t timeout;
  timeout = 1000000000ULL;
  #endif //SEMAPHORE_TIMEOUT_ENABLE

  switch (semaphoreType)
  {
    case (eSemaphoreSrcReady0) :
      REG_DSPCTRL_M3_DMA_SRCRDY_12.BF.READY = 0x1;
      break;

    case (eSemaphoreSrcReady1) :
      REG_DSPCTRL_M3_DMA_SRCRDY_13.BF.READY = 0x1;
      break;

    case (eSemaphoreSrcReady2) :
      REG_DSPCTRL_M3_DMA_SRCRDY_14.BF.READY = 0x1;
      break;

    case (eSemaphoreSrcReady3) :
      REG_DSPCTRL_M3_DMA_SRCRDY_15.BF.READY = 0x1;
      break;

    case (eSemaphoreDstReady0) :
      REG_DSPCTRL_M3_DMA_DSTRDY_12.BF.READY = 0x1;
      break;

    case (eSemaphoreDstReady1) :
      REG_DSPCTRL_M3_DMA_DSTRDY_13.BF.READY = 0x1;
      break;

    case (eSemaphoreDstReady2) :
      REG_DSPCTRL_M3_DMA_DSTRDY_14.BF.READY = 0x1;
      break;

    case (eSemaphoreDstReady3) :
      REG_DSPCTRL_M3_DMA_DSTRDY_15.BF.READY = 0x1;
      break;

    case (eSemaphoreReadRequest0) :
      #ifdef SEMAPHORE_TIMEOUT_ENABLE
        while ((--timeout) && (REG_DSPCTRL_M3_SEMAPHORE0.BF.SEMAPHORE == 0x0));
        if (timeout == 0) { eta_csp_debug_uart_puts("Timeout in EtaCspDspSetSemaphore 0\r\n"); }
      #else //SEMAPHORE_TIMEOUT_ENABLE
        while (REG_DSPCTRL_M3_SEMAPHORE0.BF.SEMAPHORE == 0x0);
      #endif //SEMAPHORE_TIMEOUT_ENABLE
      break;

    case (eSemaphoreReadRequest1) :
      #ifdef SEMAPHORE_TIMEOUT_ENABLE
        while ((--timeout) && (REG_DSPCTRL_M3_SEMAPHORE1.BF.SEMAPHORE == 0x0));
        if (timeout == 0) { eta_csp_debug_uart_puts("Timeout in EtaCspDspSetSemaphore 1\r\n"); }
      #else //SEMAPHORE_TIMEOUT_ENABLE
        while (REG_DSPCTRL_M3_SEMAPHORE1.BF.SEMAPHORE == 0x0);
      #endif //SEMAPHORE_TIMEOUT_ENABLE
      break;

    case (eSemaphoreReadRequest2) :
      #ifdef SEMAPHORE_TIMEOUT_ENABLE
        while ((--timeout) && (REG_DSPCTRL_M3_SEMAPHORE2.BF.SEMAPHORE == 0x0));
        if (timeout == 0) { eta_csp_debug_uart_puts("Timeout in EtaCspDspSetSemaphore 2\r\n"); }
      #else //SEMAPHORE_TIMEOUT_ENABLE
        while (REG_DSPCTRL_M3_SEMAPHORE2.BF.SEMAPHORE == 0x0);
      #endif //SEMAPHORE_TIMEOUT_ENABLE
      break;

    case (eSemaphoreReadRequest3) :
    default :
      #ifdef SEMAPHORE_TIMEOUT_ENABLE
        while ((--timeout) && (REG_DSPCTRL_M3_SEMAPHORE3.BF.SEMAPHORE == 0x0));
        if (timeout == 0) { eta_csp_debug_uart_puts("Timeout in EtaCspDspSetSemaphore 3\r\n"); }
      #else //SEMAPHORE_TIMEOUT_ENABLE
        while (REG_DSPCTRL_M3_SEMAPHORE3.BF.SEMAPHORE == 0x0);
      #endif //SEMAPHORE_TIMEOUT_ENABLE
      break;
  }
  return;
}

/***************************************************************************//**
 *
 *  EtaCspDspClrSemaphore - Clear the semaphore to a 0.
 *
 ******************************************************************************/
void EtaCspDspClrSemaphore(tSemaphoreType semaphoreType)
{

  switch (semaphoreType)
  {
    case (eSemaphoreSrcReady0) :
      REG_DSPCTRL_M3_DMA_SRCRDY_12.BF.READY = 0x0;
      break;

    case (eSemaphoreSrcReady1) :
      REG_DSPCTRL_M3_DMA_SRCRDY_13.BF.READY = 0x0;
      break;

    case (eSemaphoreSrcReady2) :
      REG_DSPCTRL_M3_DMA_SRCRDY_14.BF.READY = 0x0;
      break;

    case (eSemaphoreSrcReady3) :
      REG_DSPCTRL_M3_DMA_SRCRDY_15.BF.READY = 0x0;
      break;

    case (eSemaphoreDstReady0) :
      REG_DSPCTRL_M3_DMA_DSTRDY_12.BF.READY = 0x0;
      break;

    case (eSemaphoreDstReady1) :
      REG_DSPCTRL_M3_DMA_DSTRDY_13.BF.READY = 0x0;
      break;

    case (eSemaphoreDstReady2) :
      REG_DSPCTRL_M3_DMA_DSTRDY_14.BF.READY = 0x0;
      break;

    case (eSemaphoreDstReady3) :
      REG_DSPCTRL_M3_DMA_DSTRDY_15.BF.READY = 0x0;
      break;

    case (eSemaphoreReadRequest0) :
      REG_DSPCTRL_M3_SEMAPHORE0.BF.SEMAPHORE = 0x0;
      break;

    case (eSemaphoreReadRequest1) :
      REG_DSPCTRL_M3_SEMAPHORE1.BF.SEMAPHORE = 0x0;
      break;

    case (eSemaphoreReadRequest2) :
      REG_DSPCTRL_M3_SEMAPHORE2.BF.SEMAPHORE = 0x0;
      break;

    case (eSemaphoreReadRequest3) :
    default :
      REG_DSPCTRL_M3_SEMAPHORE3.BF.SEMAPHORE = 0x0;
      break;
  }
  return;
}

/***************************************************************************//**
 *
 *  EtaCspDspSetSemaphore - Poll forever until a specific semaphore is set (presumably by other processor)
 *
 ******************************************************************************/
void EtaCspDspWaitForSemaphore(tSemaphoreType semaphoreType)
{
  #ifdef SEMAPHORE_TIMEOUT_ENABLE
  uint32_t timeout;
  timeout = 1000000000ULL;
  #endif //SEMAPHORE_TIMEOUT_ENABLE

  uint8_t current_semaphore;

  do
  {
    current_semaphore = EtaCspDspReadSemaphore (semaphoreType);

    #ifdef SEMAPHORE_TIMEOUT_ENABLE
  }   while ((--timeout) && (current_semaphore == 0x0));
      if (timeout == 0) { eta_csp_debug_uart_puts("Timeout in EtaCspDspSetSemaphore 3\r\n"); }
    #else //SEMAPHORE_TIMEOUT_ENABLE
  }   while (current_semaphore == 0x0);
    #endif //SEMAPHORE_TIMEOUT_ENABLE
  return;
}

/***************************************************************************//**
 *
 *  EtaCspDspReadSemaphore - Return current value of specified semaphore
 *
 ******************************************************************************/
uint8_t EtaCspDspReadSemaphore(tSemaphoreType semaphoreType)
{
  switch (semaphoreType)
  {
    case (eSemaphoreSrcReady0) :
      return (REG_DSPCTRL_M3_DMA_SRCRDY_12.BF.READY);
      break;

    case (eSemaphoreSrcReady1) :
      return (REG_DSPCTRL_M3_DMA_SRCRDY_13.BF.READY);
      break;

    case (eSemaphoreSrcReady2) :
      return (REG_DSPCTRL_M3_DMA_SRCRDY_14.BF.READY);
      break;

    case (eSemaphoreSrcReady3) :
      return (REG_DSPCTRL_M3_DMA_SRCRDY_15.BF.READY);
      break;

    case (eSemaphoreDstReady0) :
      return (REG_DSPCTRL_M3_DMA_DSTRDY_12.BF.READY);
      break;

    case (eSemaphoreDstReady1) :
      return (REG_DSPCTRL_M3_DMA_DSTRDY_13.BF.READY);
      break;

    case (eSemaphoreDstReady2) :
      return (REG_DSPCTRL_M3_DMA_DSTRDY_14.BF.READY);
      break;

    case (eSemaphoreDstReady3) :
      return (REG_DSPCTRL_M3_DMA_DSTRDY_15.BF.READY);
      break;

    case (eSemaphoreReadRequest0) :
      return (REG_DSPCTRL_M3_SEMAPHORE0.BF.SEMAPHORE);
      break;

    case (eSemaphoreReadRequest1) :
      return (REG_DSPCTRL_M3_SEMAPHORE1.BF.SEMAPHORE);
      break;

    case (eSemaphoreReadRequest2) :
      return (REG_DSPCTRL_M3_SEMAPHORE2.BF.SEMAPHORE);
      break;

    case (eSemaphoreReadRequest3) :
    default :
      return (REG_DSPCTRL_M3_SEMAPHORE3.BF.SEMAPHORE);
      break;
  }
}


/***************************************************************************//**
 *
 *  EtaCspDspDebugEnable - Set pad mux to enable DSP JTAG debug interface.
 *
 ******************************************************************************/
void
EtaCspDspDebugEnable(void)
{
    //
    // Set padmux so JTAG_TMS is visible on GPIO[17] as an input
    //
    REG_M1(SOCCTRL_PMUXC, GPIO17, BFV_SOCCTRL_PMUXC_GPIO17_DSP_JTAG_TMS);

    //
    // Set padmux so JTAG_TDI is visible on GPIO[18] as an input
    //
    REG_M1(SOCCTRL_PMUXC, GPIO18, BFV_SOCCTRL_PMUXC_GPIO18_DSP_JTAG_TDI);

    //
    // Set padmux so JTAG_TRST_DSP_N is visible on GPIO[19] as an input
    //
    REG_M1(SOCCTRL_PMUXC, GPIO19, BFV_SOCCTRL_PMUXC_GPIO19_DSP_JTAG_TRST_N);

    //
    // Set padmux so JTAG_TCK is visible on GPIO[21] as an input
    //
    REG_M1(SOCCTRL_PMUXC, GPIO21, BFV_SOCCTRL_PMUXC_GPIO21_DSP_JTAG_TCLK);

    //
    // Set padmux so JTAG_TDO is visible on GPIO[22] as an output
    //
    REG_M1(SOCCTRL_PMUXC, GPIO22, BFV_SOCCTRL_PMUXC_GPIO22_DSP_JTAG_TDO);

    //
    // Set the PULL UP resistors GPIO[22,21,19,18,17]
    //
    REG_RTC_PULL_ENABLE.V    |= 0x006E0000;
    REG_RTC_PULL_DIRECTION.V |= 0x006E0000;
}

/***************************************************************************//**
 *
 *  EtaCspDspFence - Establish a fence for things that touchte the M3 side 
 *  dsp_ctrl module.
 *
 ******************************************************************************/
void
EtaCspDspFence(void)
{
    // fixme remove after testing REG_GPIO_DEBUG0_READ_ONLY.V = REG_DSPCTRL_M3_DSP_STATUS.V; // fence
    EtaCspDspFenceFast();

}

