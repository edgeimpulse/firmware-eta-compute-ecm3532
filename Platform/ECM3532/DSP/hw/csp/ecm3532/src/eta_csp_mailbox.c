/***************************************************************************//**
 *
 * @file eta_csp_mailbox.c
 *
 * @brief This file contains eta_csp_mailbox module implementations.
 *
 * Copyright (C) 2020 Eta Compute, Inc
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
 * This is part of revision 1.0.0b1 of the Tensai Chip Support Package.
 *
 ******************************************************************************/

/***************************************************************************//**
 * @addtogroup ecm3532mbox-dsp Mailbox
 * @ingroup ecm3532csp-dsp
 * @{
 ******************************************************************************/

#include <stdarg.h>
#include <stdlib.h>

#include "reg.h"
#include "eta_csp_mailbox.h"
#include "eta_csp_isr.h"
#include "eta_chip.h"
#include "eta_csp_io.h"

#define TX_MBOX_TIMEOUT_ENABLE 1 // If 1, we have a timeout to compensate for
                                 // permenantly stalled M3 and will not lockup.
                                 // If 0, we'll wait until Mailbox is ready.

#define SEMAPHORE_TIMEOUT_ENABLE 1 // If 1, we have a timeout to compensate for
                                   // permenantly stalled M3 and will not
                                   // lockup. If 0, we'll wait until Semaphore
                                   // is ready.

#define IFDBG if(0)

/***************************************************************************//**
 *
 *  EtaCspAhbWindowSet - Sets AHB window address.
 *
 *  @param window - index of window to change
 *  @param ahb_byte_addr - long int indicating 32 bit address to map window to.
 *
 ******************************************************************************/
tEtaStatus
EtaCspAhbWindowSet(int window, long unsigned int ahb_byte_addr)
{
    if(REG_DSPCTRL_DSP_CONFIG_STATUS & BM_DSPCTRL_DSP_CONFIG_STATUS_IO_CMD_IN_PROG == 
       BM_DSPCTRL_DSP_CONFIG_STATUS_IO_CMD_IN_PROG)
    {
        return eEtaBusy;
    }

    switch(window)
    {
        case 0:
        {
            REG_DSPCTRL_DSP_WIN_ADDR0_UPPER = (ahb_byte_addr >> 16) & 0xffff;
            REG_DSPCTRL_DSP_WIN_ADDR0_LOWER = (ahb_byte_addr >> 0)  & 0xffff;
            break;
        }

        case 1:
        {
            REG_DSPCTRL_DSP_WIN_ADDR1_UPPER = (ahb_byte_addr >> 16) & 0xffff;
            REG_DSPCTRL_DSP_WIN_ADDR1_LOWER = (ahb_byte_addr >> 0)  & 0xffff;
            break;
        }

        case 2:
        {
            REG_DSPCTRL_DSP_WIN_ADDR2_UPPER = (ahb_byte_addr >> 16) & 0xffff;
            REG_DSPCTRL_DSP_WIN_ADDR2_LOWER = (ahb_byte_addr >> 0)  & 0xffff;
            break;
        }

        case 3:
        {
            REG_DSPCTRL_DSP_WIN_ADDR3_UPPER = (ahb_byte_addr >> 16) & 0xffff;
            REG_DSPCTRL_DSP_WIN_ADDR3_LOWER = (ahb_byte_addr >> 0)  & 0xffff;
            break;
        }
    }
    return(eEtaSuccess);
}

/***************************************************************************//**
 *
 *  EtaCspMboxInit - Initializes the mailbox (interrupts, clears prev stale
 *messages)
 *
 ******************************************************************************/
void
EtaCspMboxInit()
{
    volatile uint16_t rx_mailbox;

    // clear mailbox (or else we may get a stale message from previous runs)
    rx_mailbox = REG_MBOX_DSP_M32DSP_MBOX_LOWER;

    EtaCspIsrIntClear(eIrqNumMbox, MBOX_ISR_LINE);
    EtaCspIsrExtIntEnable(eIrqNumMbox, MBOX_ISR_LINE);

    EtaCspIsrHandlerSet(eIrqNumMbox, MBOX_ISR_LINE,
                        EtaCspMboxIsrProcess);
}

/***************************************************************************//**
 *
 *  EtaCspPrintViaMboxInit - Initializes the Print via M3 Mailbox routines
 *
 ******************************************************************************/
void
EtaCspPrintViaMboxInit(void)
{
    EtaCspIoMboxPrintInit();

    EtaCspMboxInit();
}

/***************************************************************************//**
 *
 *  Mailbox Handler Pointer
 *
 ******************************************************************************/
static tpfMboxHandler default_mbox_handler_entry =
{
    EtaCspDspMboxDefaultHandler
};

/***************************************************************************//**
 *
 *  EtaCspDspMboxDefaultHandler - Initializes the Print via M3 Mailbox routines
 *
 *  @param ui16RxMboxCmd  - Lower 16 bits of Mailbox (in prev chip versions,
 *this was only mailbox)
 *  @param ui32RxMboxData - Upper 32 bits extended Mailbox command
 *
 ******************************************************************************/
void
EtaCspDspMboxDefaultHandler(uint16_t ui16RxMboxCmd, uint32_t ui32RxMboxData)
{
    etaPrintf("\r\nDSP received unknown mbox from M3 lower=%x ext2,1=%lx\r\n",
              ui16RxMboxCmd, ui32RxMboxData);
}

/***************************************************************************//**
 *
 *  EtaCspDspMboxDefaultHandlerSet - Set Mailbox Handler to Default Handler
 *
 ******************************************************************************/
void
EtaCspDspMboxDefaultHandlerSet(tpfMboxHandler pfnMboxHandler)
{
    default_mbox_handler_entry = pfnMboxHandler;
}

/***************************************************************************//**
 *
 *  EtaCspMboxIsrProcess - ISR command for mailboxe interrupt
 *
 *  @param iIrqNum is the interrupt number.
 *  @param int_line is the DSP interrupt line number.
 *
 ******************************************************************************/
volatile int16_t mbIrq = 0;
void
EtaCspMboxIsrProcess(tIrqNum iIrqNum, intline_T int_line)
{
    uint16_t ui16RxMboxCmd;
    uint32_t ui32RxMboxData;

    ui32RxMboxData = REG_MBOX_DSP_M32DSP_MBOX_EXT2; // Upper 16 bits.
    ui32RxMboxData = (ui32RxMboxData << 16);
    ui32RxMboxData |= REG_MBOX_DSP_M32DSP_MBOX_EXT1;
    ui16RxMboxCmd = REG_MBOX_DSP_M32DSP_MBOX_LOWER; // Clears interrupt
    mbIrq++;
    switch(ui16RxMboxCmd & 0xff)
    {
        case MAILBOX_M32DSPCMD_RETURNCRED:
        {
            if(EtaCspIoPrintfViaM3MboxGet())
            {
                EtaCspPrintViaM3ProcessPrintCredit(
                                                        (ui16RxMboxCmd >> 8) &
                                                        0xff);
                break;
            }
            else
            {
                #ifdef WARN
                etaPrintf(
                          "\r\nDSP received MAILBOX_M32DSPCMD_RETURNCRED command (%x) but thats not enabled.\r\n",
                          ui16RxMboxCmd);
                #endif
                break;
            }
        }

#if 0   //To ressolve compile error this case is commented.
        case (MAILBOX_M32DSPCMD_ADC_CFG):
        {
            EtaCspAdcConfigUnpack(ui32RxMboxData);
            break;
        }
#endif

        default:
	    //
	    // Reminder this is called in the interrupt context.
	    //
            default_mbox_handler_entry(ui16RxMboxCmd, ui32RxMboxData);
    }

    EtaCspIsrIntClear(iIrqNum, int_line);
}

/***************************************************************************//**
 *
 *  EtaCspMboxDsp2M3 - Send DSP to M3 Mailbox
 *
 *  @param cmd is the 16 bit command
 *  @param data is the 32 bit data
 *
 ******************************************************************************/
void
EtaCspMboxDsp2M3(uint16 cmd, uint32_t data)
{
    uint32_t timeout;

    timeout = 10000000;

    if(TX_MBOX_TIMEOUT_ENABLE)
    {
        while((--timeout) && (REG_MBOX_DSP_DSP2M32_MBOX_STATUS == 1))
        {
        }

        if(timeout == 0)
        {
            IFDBG EtaCspIoDebugUartPuts(
                                    "Timeout in eta_csp_mailbox.EtaCspMboxDsp2M3\r\n");
        }
    }
    else
    {
        while(REG_MBOX_DSP_DSP2M32_MBOX_STATUS == 1)
        {
        }
    }

    REG_MBOX_DSP_DSP2M3_MBOX_EXT2 = (data >> 16) & 0xffff;
    REG_MBOX_DSP_DSP2M3_MBOX_EXT1 = (data >> 0) & 0xffff;
    REG_MBOX_DSP_DSP2M3_MBOX_LOWER = cmd;
}

/***************************************************************************//**
 *
 *  eta_csp_dsp2m3_16bit_mbox - Send legacy 16 bit only Mailbox
 *
 *  @param cmd is the 16 bit command
 *
 * NOTE: this function is probably only of use during validation.
 ******************************************************************************/
void
eta_csp_dsp2m3_16bit_mbox(int cmd)
{
    uint32_t timeout;

    timeout = 10000000;

    if(TX_MBOX_TIMEOUT_ENABLE)
    {
        while((--timeout) && (REG_MBOX_DSP_DSP2M32_MBOX_STATUS == 1))
        {
        }

        if(timeout == 0)
        {
            IFDBG EtaCspIoDebugUartPuts(
                                    "Timeout in eta_csp_mailbox.eta_csp_dsp2m3_16bit_mbox\r\n");
        }
    }
    else
    {
        while(REG_MBOX_DSP_DSP2M32_MBOX_STATUS == 1)
        {
        }
    }

    REG_MBOX_DSP_DSP2M3_MBOX_LOWER = cmd;
}

/***************************************************************************//**
 *
 *  EtaCspWaitForPrintCredits - waits for the credits from PRINT_VIA_M3_MAILBOX
 *to be returned
 *
 *  Note: This can be disabled (made generally a nop) if PRINT_VIA_M3_MBOX is
 *not set
 *
 ******************************************************************************/
void
EtaCspWaitForPrintCredits()
{
    if(EtaCspIoPrintfViaM3MboxGet())
    {
        // Wait for all the credits to be returned before we stop resonding to
        // doorbells
        while(glob_printbuff_rdptr != glob_printbuff_wrptr)
        {
        }
    }
    return;
}

/***************************************************************************//**
 *
 *  EtaCspDspSemaphoreSet - Set the semaphore to a 1.
 *
 *  @param semaphoreType - Type of Semaphore to set.
 *
 *  Note: For eSemaphoreReadRequest0-3, this command will poll until
 *        the semaphore is not reserved so we can set it to 1.
 *
 ******************************************************************************/
void
EtaCspDspSemaphoreSet(tSemaphoreType semaphoreType)
{
#ifdef SEMAPHORE_TIMEOUT_ENABLE
    uint32_t timeout;
    timeout = 1000000000ULL;
#endif   // SEMAPHORE_TIMEOUT_ENABLE

    switch(semaphoreType)
    {
        case (eSemaphoreSrcReady0):
        {
            REG_DSPCTRL_DSP_DMA_SRCRDY_12 = 0x1;
            break;
        }

        case (eSemaphoreSrcReady1):
        {
            REG_DSPCTRL_DSP_DMA_SRCRDY_13 = 0x1;
            break;
        }

        case (eSemaphoreSrcReady2):
        {
            REG_DSPCTRL_DSP_DMA_SRCRDY_14 = 0x1;
            break;
        }

        case (eSemaphoreSrcReady3):
        {
            REG_DSPCTRL_DSP_DMA_SRCRDY_15 = 0x1;
            break;
        }

        case (eSemaphoreDstReady0):
        {
            REG_DSPCTRL_DSP_DMA_DSTRDY_12 = 0x1;
            break;
        }

        case (eSemaphoreDstReady1):
        {
            REG_DSPCTRL_DSP_DMA_DSTRDY_13 = 0x1;
            break;
        }

        case (eSemaphoreDstReady2):
        {
            REG_DSPCTRL_DSP_DMA_DSTRDY_14 = 0x1;
            break;
        }

        case (eSemaphoreDstReady3):
        {
            REG_DSPCTRL_DSP_DMA_DSTRDY_15 = 0x1;
            break;
        }

        case (eSemaphoreReadRequest0):
        {
#ifdef SEMAPHORE_TIMEOUT_ENABLE
            while((--timeout) && (REG_DSPCTRL_DSP_SEMAPHORE0 == 0x0))
            {
            }
            if(timeout == 0)
            {
                IFDBG EtaCspIoDebugUartPuts("Timeout in EtaCspDspSemaphoreSet 0\r\n");
            }
#else       // SEMAPHORE_TIMEOUT_ENABLE
            while(REG_DSPCTRL_DSP_SEMAPHORE0 == 0x0)
            {
            }
#endif       // SEMAPHORE_TIMEOUT_ENABLE
            break;
        }

        case (eSemaphoreReadRequest1):
        {
#ifdef SEMAPHORE_TIMEOUT_ENABLE
            while((--timeout) && (REG_DSPCTRL_DSP_SEMAPHORE1 == 0x0))
            {
            }
            if(timeout == 0)
            {
                IFDBG EtaCspIoDebugUartPuts("Timeout in EtaCspDspSemaphoreSet 1\r\n");
            }
#else       // SEMAPHORE_TIMEOUT_ENABLE
            while(REG_DSPCTRL_DSP_SEMAPHORE1 == 0x0)
            {
            }
#endif       // SEMAPHORE_TIMEOUT_ENABLE
            break;
        }

        case (eSemaphoreReadRequest2):
        {
#ifdef SEMAPHORE_TIMEOUT_ENABLE
            while((--timeout) && (REG_DSPCTRL_DSP_SEMAPHORE2 == 0x0))
            {
            }
            if(timeout == 0)
            {
                IFDBG EtaCspIoDebugUartPuts("Timeout in EtaCspDspSemaphoreSet 2\r\n");
            }
#else       // SEMAPHORE_TIMEOUT_ENABLE
            while(REG_DSPCTRL_DSP_SEMAPHORE2 == 0x0)
            {
            }
#endif       // SEMAPHORE_TIMEOUT_ENABLE
            break;
        }

        case (eSemaphoreReadRequest3):
        default:
        {
#ifdef SEMAPHORE_TIMEOUT_ENABLE
            while((--timeout) && (REG_DSPCTRL_DSP_SEMAPHORE3 == 0x0))
            {
            }
            if(timeout == 0)
            {
                IFDBG EtaCspIoDebugUartPuts("Timeout in EtaCspDspSemaphoreSet 3\r\n");
            }
#else       // SEMAPHORE_TIMEOUT_ENABLE
            while(REG_DSPCTRL_DSP_SEMAPHORE3 == 0x0)
            {
            }
#endif       // SEMAPHORE_TIMEOUT_ENABLE
            break;
        }
    }
    return;
}

/***************************************************************************//**
 *
 *  EtaCspDspSemaphoreClear - Clear the semaphore to a 0.
 *
 *  @param semaphoreType - Type of Semaphore.
 *
 ******************************************************************************/
void
EtaCspDspSemaphoreClear(tSemaphoreType semaphoreType)
{
    switch(semaphoreType)
    {
        case (eSemaphoreSrcReady0):
        {
            REG_DSPCTRL_DSP_DMA_SRCRDY_12 = 0x0;
            break;
        }

        case (eSemaphoreSrcReady1):
        {
            REG_DSPCTRL_DSP_DMA_SRCRDY_13 = 0x0;
            break;
        }

        case (eSemaphoreSrcReady2):
        {
            REG_DSPCTRL_DSP_DMA_SRCRDY_14 = 0x0;
            break;
        }

        case (eSemaphoreSrcReady3):
        {
            REG_DSPCTRL_DSP_DMA_SRCRDY_15 = 0x0;
            break;
        }

        case (eSemaphoreDstReady0):
        {
            REG_DSPCTRL_DSP_DMA_DSTRDY_12 = 0x0;
            break;
        }

        case (eSemaphoreDstReady1):
        {
            REG_DSPCTRL_DSP_DMA_DSTRDY_13 = 0x0;
            break;
        }

        case (eSemaphoreDstReady2):
        {
            REG_DSPCTRL_DSP_DMA_DSTRDY_14 = 0x0;
            break;
        }

        case (eSemaphoreDstReady3):
        {
            REG_DSPCTRL_DSP_DMA_DSTRDY_15 = 0x0;
            break;
        }

        case (eSemaphoreReadRequest0):
        {
            REG_DSPCTRL_DSP_SEMAPHORE0 = 0x1;
            break;
        }

        case (eSemaphoreReadRequest1):
        {
            REG_DSPCTRL_DSP_SEMAPHORE1 = 0x1;
            break;
        }

        case (eSemaphoreReadRequest2):
        {
            REG_DSPCTRL_DSP_SEMAPHORE2 = 0x1;
            break;
        }

        case (eSemaphoreReadRequest3):
        default:
        {
            REG_DSPCTRL_DSP_SEMAPHORE3 = 0x1;
            break;
        }
    }
    return;
}

/***************************************************************************//**
 *
 *  EtaCspDspSemaphoreWait - Poll forever until a specific semaphore is set
 *(presumably by other processor)
 *
 *  @param semaphoreType - Type of Semaphore.
 *
 ******************************************************************************/
void
EtaCspDspSemaphoreWait(tSemaphoreType semaphoreType)
{
#ifdef SEMAPHORE_TIMEOUT_ENABLE
    uint32_t timeout;
    timeout = 1000000000ULL;
#endif   // SEMAPHORE_TIMEOUT_ENABLE

    uint8_t current_semaphore;

    do
    {
        current_semaphore = EtaCspDspSemaphoreRead(semaphoreType);

#ifdef SEMAPHORE_TIMEOUT_ENABLE
    }
    while((--timeout) && (current_semaphore == 0x0));
    if(timeout == 0)
    {
        IFDBG EtaCspIoDebugUartPuts("Timeout in EtaCspDspSemaphoreWait\r\n");
    }
#else     // SEMAPHORE_TIMEOUT_ENABLE
    }
    while(current_semaphore == 0x0);
#endif     // SEMAPHORE_TIMEOUT_ENABLE
    return;
}

/***************************************************************************//**
 *
 * EtaCspDspSemaphoreRead - Return current value of specified semaphore
 *
 * @param SemaphoreType is one of the 12 available semaphore types.
 *
 * > For semaphore types ReadRequest0,1,2, and 3, treat this function as
 *   a REQUEST and not as a READ. When you read these if you get 0 that
 *   means the semaphore was free, and you just claimed it! The read access
 *   cause the bit to be set to 1. All subsequent reads of this semaphore
 *   will return 1, until it's cleared. Also, note that ANY client can clear
 *   this bit. Call EtaCspDspSemaphoreClear() to clear this bit.
 *
 * @return Returns the state of the semaphore.
 *
 ******************************************************************************/
uint8_t
EtaCspDspSemaphoreRead(tSemaphoreType semaphoreType)
{
    switch(semaphoreType)
    {
        case (eSemaphoreSrcReady0):
        {
            return(REG_DSPCTRL_DSP_DMA_SRCRDY_12);
            break;
        }

        case (eSemaphoreSrcReady1):
        {
            return(REG_DSPCTRL_DSP_DMA_SRCRDY_13);
            break;
        }

        case (eSemaphoreSrcReady2):
        {
            return(REG_DSPCTRL_DSP_DMA_SRCRDY_14);
            break;
        }

        case (eSemaphoreSrcReady3):
        {
            return(REG_DSPCTRL_DSP_DMA_SRCRDY_15);
            break;
        }

        case (eSemaphoreDstReady0):
        {
            return(REG_DSPCTRL_DSP_DMA_DSTRDY_12);
            break;
        }

        case (eSemaphoreDstReady1):
        {
            return(REG_DSPCTRL_DSP_DMA_DSTRDY_13);
            break;
        }

        case (eSemaphoreDstReady2):
        {
            return(REG_DSPCTRL_DSP_DMA_DSTRDY_14);
            break;
        }

        case (eSemaphoreDstReady3):
        {
            return(REG_DSPCTRL_DSP_DMA_DSTRDY_15);
            break;
        }

        case (eSemaphoreReadRequest0):
        {
            return(REG_DSPCTRL_DSP_SEMAPHORE0);
            break;
        }

        case (eSemaphoreReadRequest1):
        {
            return(REG_DSPCTRL_DSP_SEMAPHORE1);
            break;
        }

        case (eSemaphoreReadRequest2):
        {
            return(REG_DSPCTRL_DSP_SEMAPHORE2);
            break;
        }

        case (eSemaphoreReadRequest3):
        default:
        {
            return(REG_DSPCTRL_DSP_SEMAPHORE3);
            break;
        }
    }
}

