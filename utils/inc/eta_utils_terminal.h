/***************************************************************************//**
 *
 * @file eta_utils_terminal.h
 *
 * @brief Terminal include.
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
 * This is part of revision 1.0.0a6 of the Tensai Software Development Kit.
 *
 ******************************************************************************/

/***************************************************************************//**
 * @addtogroup etautils-terminal Terminal
 * @ingroup etautils
 * @{
 ******************************************************************************/

#ifndef _ETA_UTILS_TERMINAL_H_
#define _ETA_UTILS_TERMINAL_H_

#include "eta_csp_uart.h"

#ifdef ETA_UTILS_USE_TERMINAL
#include "microrl.h"
#endif

/*******************************************************************************
 *
 * External function definitions
 *
 ******************************************************************************/

//
// Initialize the terminal.
//
extern void EtaUtilsTerminalInit(tUart *psUart);

//
// Print to stream callback
//
extern void EtaUtilsTerminalPrint(const char *pcStr);

//
// Get char from stream.
//
extern char EtaUtilsTerminalCharGet(bool bWait);

//
// Execute callback.
//
extern int EtaUtilsTerminalExecute(int argc, const char * const *argv);

//
// Completion callback.
//
extern char **EtaUtilsTerminalComplet(int argc, const char * const *argv);

//
// Ctrl+c callback.
//
extern void EtaUtilsTerminalSigint(void);

#endif // _ETA_UTILS_TERMINAL_H_

/** @}*/

