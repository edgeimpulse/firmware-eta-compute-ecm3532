/***************************************************************************//**
 *
 * @file eta_csp_dsp.h
 *
 * @brief This file contains eta_csp_dsp module definitions.
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
 * This is part of revision 1.0.0b1 of the Tensai Chip Support Package.
 *
 ******************************************************************************/

/***************************************************************************//**
 * @addtogroup ecm3532dsp-m3 Digital Signal Processor (DSP)
 * @ingroup ecm3532csp-m3
 * @{
 ******************************************************************************/

#ifndef ETA_CSP_DSP_H
#define ETA_CSP_DSP_H

#include <stdint.h>
#include <stdlib.h>
#include "eta_csp_isr.h"
#include "eta_csp_nvic.h"
#include "eta_csp_mailbox_cmd.h"
#include "eta_csp_common_semaphore.h"
#include "memio.h"

#ifdef __cplusplus
extern "C" {
#endif

//
// These are fixed addresses for the DSP memories.
//
#define DSP_PMEM_START (0x40840000)
#define DSP_XMEM_START (0x40880000)
#define DSP_YMEM_START (0x408c0000)

//
// This macro does the simple math needed to access the IOMEM space from the
// AHB.  It doesn't provide a nice, register named access for the IOMEM from
// AHB, but it is a little better.
//
#define IOMEM2AHB(ui32IoMem) MEMIO32(0x40800000 + (ui32IoMem * 4))

//
// This fence will compile inline and do only one byte reads from the RTC block.
//
#define EtaCspDspFenceFast() {REG_GPIO_DEBUG0_READ_ONLY.V = \
	                                              MEMIO8(REG_DSPCTRL_M3_DSP_STATUS_ADDR);}

//
// This fence will compile inline and do only one byte reads from the RTC block.
//
#define ETA_CSP_DSP_MBOX_FENCE() {REG_GPIO_DEBUG0_READ_ONLY.V = \
	                                              MEMIO8(REG_MBOX_M3_M32DSP_LOWER_ADDR);}

//
//! DSP Power Status typedefs
//
typedef enum
{
    eDspPowerOff = 0,
    eDspPowerOn  = 1,
}
tDspPowerState;

//
//! Flash loader pieces typedef.
//
typedef struct
{
    //
    //! Pointer to the image name.
    //
    const char *pcImageName;

    //
    //! P/X/Ymem sizes.
    //
    uint32_t ui32PmemSize;
    uint32_t ui32XmemSize;
    uint32_t ui32YmemSize;

    //
    //! Pointer to the P/X/Ymem data.
    //
    const uint32_t *pui32PmemData;
    const uint16_t *pui16XmemData;
    const uint16_t *pui16YmemData;
}
tDspMem;

//
//! Mailbox default handler.
//
typedef void (*tpfMboxHandler) (uint16_t ui16RxMboxCmd,
                                uint32_t ui32RxMboxData);

//
// Return the power state of the DSP Complex
//
extern tDspPowerState EtaCspDspPowerStateGet(void);

//
// Assert the reset to the DSP Core
//
extern void EtaCspDspResetAssert(void);

//
// Deassert the reset to the DSP Core
//
extern void EtaCspDspResetDeassert(void);

//
// Simple, no frills, just do it loader calls.
// EtaCspDspLoaderMain(true,true,false,*).
//
extern void EtaCspDspLoaderSimple(const tDspMem *psDspMem);

//
// The main dsp loader.
//
extern uint32_t EtaCspDspLoaderMain(bool bQuiet, bool bCheckData,
                                    bool bLeaveInReset, const tDspMem *psDspMem);

//
// Check the DSP load.
//
extern uint32_t EtaCspDspLoaderCheck(bool bQuiet, bool bDisplayError,
                                     const tDspMem *psDspMem);

//
// Flash Loader Pieces.
//

//
// DSP Mailbox Commands.
//

//
// DSP has these mailbox commands:
// For DSP mailbox commands, the DSP sends 16 bit word that is:
// [15] XMEM=0, YMEM=1
// [14:0] System Address pointint to first byte of command.
// System address is similar to DSP linear byte address, but
// DSP 16 bit half words are mapped to every 32 bit word. So this effectively
// means
// SystemHalfWordAddr[13:0] = DSPHalfWordAddr [12:1],1'b0,DSPHalfWordAddr[0];
// and
// SystemByteAddr[14:0] = SystemHalfWordAddr[13:0] <<1 + byte offset.
//


//
// Power up the DSP Complex
//
extern void EtaCspDspPowerUp(void);

//
// Power down the DSP Complex
//
extern void EtaCspDspPowerDown(void);



// EtaCspDspMboxInit - Initialization routine for M3 Mailbox.
extern void EtaCspDspMboxInit(void);

//
// The routine to print to UART from DSP memory.  It takes system address as an
// input.
//
extern void EtaCspDspPrintViaM3(uint32_t ui32Address);

//
// Send data to the mailbox.
//
extern void EtaCspDspMboxDataSend(uint32_t ui32Data);

//
// Send a command to the DSP.
//
extern void EtaCspDspMboxCmdSend(uint16_t ui16Cmd, uint32_t ui32Data);

//
// Routine that turns the DSP memory addresses into a linear address.  It takes
// the current SYSTEM address (ui32CurrAddress) and returns the ui32CurrAddress
// incremented by 1.
//
extern uint32_t EtaCspDspPrintAddrInc(uint32_t ui32CurrAddress);

//
// The routine to processed received mailboxes.
//
extern void EtaCspDspMboxM3Receive(tIrqNum iIrqNum);

//
// The default mailbox handler.
//
extern void EtaCspDspMboxDefaultHandler(uint16_t ui16RxMboxCmd,
                                        uint32_t ui32RxMboxData);

//
// Set the default handler.
//
extern void EtaCspDspMboxDefaultHandlerSet(tpfMboxHandler pfnMboxHandler);

//////////////////////////////////////////////////////////////////////
// Semaphore, Source Ready, Destination Ready Prototypes

/***************************************************************************//**
 *
 *  EtaCspDspSetSemaphore - Set the semaphore to a 1.
 *
 *  Note: For eSemaphoreReadRequest0-3, this command will poll until
 *        the semaphore is not reserved so we can set it to 1.
 *
 ******************************************************************************/
extern void EtaCspDspSetSemaphore(tSemaphoreType semaphoreType);

/***************************************************************************//**
 *
 *  EtaCspDspSemaphoreClear - Clear the semaphore to a 0.
 *
 ******************************************************************************/
void EtaCspDspSemaphoreClear(tSemaphoreType semaphoreType);

/***************************************************************************//**
 *
 *  EtaCspDspSetSemaphore - Poll forever until a specific semaphore is set (presumably by other processor)
 *
 ******************************************************************************/
extern void EtaCspDspWaitForSemaphore(tSemaphoreType semaphoreType);

/***************************************************************************//**
 *
 *  EtaCspDspReadSemaphore - Return current value of specified semaphore
 *
 ******************************************************************************/
extern uint8_t EtaCspDspReadSemaphore(tSemaphoreType semaphoreType);

// Semaphore, Source Ready, Destination Ready Prototypes
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Enable Pads for debugger JTAG connections.

/***************************************************************************//**
 *
 *  EtaCspDspDebugEnable - Set pad mux to enable DSP JTAG debug interface.
 *
 ******************************************************************************/
extern void EtaCspDspDebugEnable(void);

// Enable Pads for debugger JTAG connections.
//////////////////////////////////////////////////////////////////////

//
// EtaCspDspFence - Establish a fence for things that touchte the M3 side
// dsp_ctrl module.
//
extern void EtaCspDspFence(void);

#ifdef __cplusplus
}
#endif

#endif // ETA_CSP_DSP_H

