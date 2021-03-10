/***************************************************************************//**
 *
 * @file eta_csp_io.h
 *
 * @brief This file contains eta_csp_io module definitions.
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
 * @addtogroup ecm3532io-dsp Input/Output (IO)
 * @ingroup ecm3532csp-dsp
 * @{
 ******************************************************************************/

#ifndef __ETA_CSP_IO_H__
#define __ETA_CSP_IO_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "stdarg.h"
#include "stdlib.h"
#include "eta_chip.h"

/*******************************************************************************
 *
 * Print Options - UART0, UART1 or via M3 through Mailbox.
 *
 * It is expected to uncomment only one of these options for normal operation
 * but, if you enable UART and PRINT_VIA_M3_MBOX, you can reasonably do both
 * but the UART will receive a handful of non printable characters (0x01 and
 * 0x0 per printf, and once per DSP_PRINT_BUFFER_SIZE you'll get the rollovr
 * code 0x11 0xNN 0xNN where NN is address of buffer). But other than occational
 * non printable characters, one UART and PRINT_VIA_M3_MBOX can be enabled.
 * Enabling two UARTs will result in UART1.
 *
 ******************************************************************************/

// #define PRINT_VIA_M3_MBOX

// #define PUTS_VIA_M3_MBOX

//
// PRINT_VIA_M3_MBOX
// glob_printbuff_* is the local buffer inside DSP the DSP "prints" to.
// it then sends a message via mailbox to the M3 to ask it to start
// printing from that buffer.
//

// buffer can be any size, but must be bigger than max len print.
// M3 currently has limitation of 255 char in a single string (1 byte credit
// return),
// so that seems a reasonable minimum.
// Print Options
#define DSP_PRINT_BUFFER_SIZE   256
#define DSP_PRINT_BUFFER_USABLE (DSP_PRINT_BUFFER_SIZE - 5) // We reserve 5
                                                            // bytes for ease of
                                                            // wrapping

#define DSP_PRINT_BUFFER_IN_YMEM 0 // 0 for XMEM, 1 for YMEM ... change when
                                   // glob_printbuff_data moves to new memory

#if DSP_PRINT_BUFFER_IN_YMEM
extern uchar chess_storage(YMEM) glob_printbuff_data [DSP_PRINT_BUFFER_SIZE]; //
                                                                              //
                                                                              // change
                                                                              //
                                                                              // DSP_PRINT_BUFFER_IN_YMEM
                                                                              //
                                                                              // when
                                                                              //
                                                                              // you
                                                                              //
                                                                              // change
                                                                              //
                                                                              // this
#else
extern uchar chess_storage(XMEM) glob_printbuff_data [DSP_PRINT_BUFFER_SIZE]; //
                                                                              //
                                                                              // change
                                                                              //
                                                                              // DSP_PRINT_BUFFER_IN_YMEM
                                                                              //
                                                                              // when
                                                                              //
                                                                              // you
                                                                              //
                                                                              // change
                                                                              //
                                                                              // this
#endif

// Glob buffer pointers (write / read).
extern volatile int glob_printbuff_wrptr;
extern volatile int glob_printbuff_rdptr;

// Temp buffer used to store the beginning of a "printf" in the buffer.
extern int glob_printbuff_msgstart;

// PRINT_VIA_M3_MBOX
//////////////////////////////////////////////////////////////////////

/*******************************************************************************
 *
 * External function definitions.
 *
 ******************************************************************************/

// Print functions


extern void EtaCspPrintViaM3ProcessPrintCredit(int credit);

extern void etaPutc(const char x);
extern void etaPuts(const char *str);
extern void _etaPuts(const char *str);
extern void etaPrintf(const char *format, ...);
extern void EtaCspIoDebugUartPutc(const char cChar);
extern void EtaCspIoDebugUartPuts(const char *pcString);

// Turn on / off all prints
extern void EtaCspIoVerboseEnable(void);
extern void EtaCspIoVerboseDisable(void);

extern void EtaCspIoPrintfViaM3Mbox(bool bEnable);
extern bool EtaCspIoPrintfViaM3MboxGet(void);
extern void EtaCspIoPrintfViaDspUart(bool bEnable);
extern bool EtaCspIoPrintfViaDspUartGet(void);


extern void EtaCspIoMboxPrintInit(void);
#ifdef __cplusplus
}
#endif

#endif // __ETA_CSP_IO_H__

