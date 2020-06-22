/***************************************************************************//**
 *
 * @file eta_utils_stdio.h
 *
 * @brief Include file for stdio.
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
 * @addtogroup etautils-stdio Input and Output Library
 * @ingroup etautils
 * @{
 ******************************************************************************/

#ifndef __ETA_UTILS_STDIO_H__
#define __ETA_UTILS_STDIO_H__

#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>
#include "eta_csp_inc.h"

#ifdef USE_SEGGER_RTT_PRINTF
#include "SEGGER_RTT.h"
#else
#include "printf.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 *
 * External function definitions
 *
 ******************************************************************************/

#ifdef USE_SEGGER_RTT_PRINTF

//
// Use the SEGGER RTT implementation.
//
#define EtaUtilsStdioPrintf(f_, ...) SEGGER_RTT_printf(0, (f_), ##__VA_ARGS__)

#else

//
// Use the Tiny printf implementation.
//
#define EtaUtilsStdioPrintf printf_

//
// Wrapper for tiny_printf
//
#define _putchar(cChar) EtaCspIoPutc(cChar)

#endif
//
// Clear the terminal.
//
extern void EtaUtilsStdioTerminalClear(void);

//
// Print a frequency out to the UART.
//
extern void EtaUtilsStdioFreqPutS(tUart *psUart, uint32_t ui32Freq);

#ifdef __cplusplus
}
#endif

#endif // __ETA_UTILS_STDIO_H__

/** @}*/

