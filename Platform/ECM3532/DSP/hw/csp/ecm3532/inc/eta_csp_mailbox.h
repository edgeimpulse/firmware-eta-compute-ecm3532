/***************************************************************************//**
 *
 * @file eta_csp_mailbox.h
 *
 * @brief This file contains eta_csp_mailbox module definitions.
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

#ifndef ETA_CSP_MAILBOX_H
#define  ETA_CSP_MAILBOX_H

#include "stdarg.h"
#include "stdlib.h"
#include "api-int_controller.h"
#include "eta_csp_isr.h"
#include "eta_csp_mailbox_cmd.h"
#include "eta_csp_io.h"
#include "eta_chip.h"
#include "eta_csp_common_semaphore.h"
#include "eta_csp_adc.h"
#include "eta_csp_status.h"

//// defined in "eta_csp_io.h"
////////////////////////////////////////////////////////////////////////
//// PRINT_VIA_M3_MBOX
//// glob_printbuff_* is the local buffer inside DSP the DSP "prints" to.
//// it then sends a message via mailbox to the M3 to ask it to start
//// printing from that buffer.
// #ifdef PRINT_VIA_M3_MBOX
//
//// buffer can be any size, but must be bigger than max len print.
//// M3 currently has limitation of 250 char in a single string (1 byte credit
/// return),
//// so that seems a reasonable minimum.
//// Print Options
// #define DSP_PRINT_BUFFER_SIZE 256
// #define DSP_PRINT_BUFFER_USABLE (DSP_PRINT_BUFFER_SIZE-5) // We reserve 5
// bytes for ease of wrapping
//
// #define DSP_PRINT_BUFFER_IN_YMEM 0 // 0 for XMEM, 1 for YMEM ... change when
// glob_printbuff_data moves to new memory
//
// #if DSP_PRINT_BUFFER_IN_YMEM
// extern uchar chess_storage(YMEM) glob_printbuff_data [DSP_PRINT_BUFFER_SIZE]
// ; // change DSP_PRINT_BUFFER_IN_YMEM when you change this
// #else
// extern uchar chess_storage(XMEM) glob_printbuff_data [DSP_PRINT_BUFFER_SIZE]
// ; // change DSP_PRINT_BUFFER_IN_YMEM when you change this
// #endif
//
//// Glob buffer pointers (write / read).
// extern volatile int glob_printbuff_wrptr;
// extern volatile int glob_printbuff_rdptr;
//
//// Temp buffer used to store the beginning of a "printf" in the buffer.
// extern int glob_printbuff_msgstart;
// #endif // PRINT_VIA_M3_MBOX
//// PRINT_VIA_M3_MBOX
////////////////////////////////////////////////////////////////////////


// Print functions
#define DSP_BYTEADDR_TO_SYSADDR(baddr) (((byteaddress & ~0x1) << 1) | \
                                        (byteaddress & 0x1))

typedef void (*tpfMboxHandler) (uint16_t ui16RxMboxCmd,
                                uint32_t ui32RxMboxData);

extern void eta_csp_dsp2m3_16bit_mbox(int data);

extern void EtaCspMboxIsrProcess(tIrqNum iIrqNum, intline_T int_line);

/*******************************************************************************
 *
 * External function definitions.
 *
 ******************************************************************************/

//
// EtaCspDspMboxDefaultHandler - Initializes the Print via M3 Mailbox routines
//
extern void EtaCspDspMboxDefaultHandler(uint16_t ui16RxMboxCmd,
                                 uint32_t ui32RxMboxData);

//
// EtaCspDspMboxDefaultHandlerSet - Set Mailbox Handler to Default Handler
//
extern void EtaCspDspMboxDefaultHandlerSet(tpfMboxHandler pfnMboxHandler);

//
// EtaCspPrintViaMboxInit - Initializes the Print via M3 Mailbox routines
//
extern void EtaCspPrintViaMboxInit(void);

//
// EtaCspWaitForPrintCredits - waits for the credits from PRINT_VIA_M3_MAILBOX
//
extern void EtaCspWaitForPrintCredits(void);

//
// EtaCspDspSemaphoreSet - Set the semaphore to a 1.
//
extern void EtaCspDspSemaphoreSet(tSemaphoreType semaphoreType);

//
// EtaCspDspSemaphoreClear - Clear the semaphore to a 0.
//
extern void EtaCspDspSemaphoreClear(tSemaphoreType semaphoreType);

//
// EtaCspDspSemaphoreWait - Poll forever until a specific semaphore is set
//
extern void EtaCspDspSemaphoreWait(tSemaphoreType semaphoreType);

//
// EtaCspDspSemaphoreRead - Return current value of specified semaphore
//
uint8_t EtaCspDspSemaphoreRead(tSemaphoreType semaphoreType);

//
// EtaCspMboxDsp2M3 - Send DSP to M3 Mailbox
//
extern void EtaCspMboxDsp2M3(uint16 cmd, uint32_t data);

//
// EtaCspAhbWindowSet - Sets AHB window address.
//
extern tEtaStatus EtaCspAhbWindowSet(int window, long unsigned int ahb_byte_addr);

//
// EtaCspMboxInit - Initializes the mailbox (interrupts, clears prev stale messages)
//
extern void EtaCspMboxInit();

#endif // ETA_CSP_MAILBOX_H

