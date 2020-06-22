/***************************************************************************//**
 *
 * @file eta_csp_io.h
 *
 * @brief This file contains eta_csp_io module definitions.
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
 * @addtogroup ecm3532io-m3 Input/Output (IO)
 * @ingroup ecm3532csp-m3
 * @{
 ******************************************************************************/

#ifndef __ETA_CSP_IO_H__
#define __ETA_CSP_IO_H__

#include "eta_csp.h"
#include "eta_csp_uart.h"
#include "eta_csp_gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

//
// Write a character.
//
extern void EtaCspIoPutc(char cChar);

//
// Write a string.
//
extern void EtaCspIoPuts(char *pcStr);

//
// Format and write the string.
//
extern void EtaCspIoPrintf(char *pcFormat, ...);

//
// Wait for transmit to finish.
//
extern void EtaCspIoTxWait(void);

//
// Clear the terminal.
//
extern void EtaCspIoTerminalClear(void);

//
// Redirect the printf output.
//
extern void EtaCspIoPrintfRedirect(tUart *psUart);

#ifdef __cplusplus
}
#endif

#endif // __ETA_CSP_IO_H__

