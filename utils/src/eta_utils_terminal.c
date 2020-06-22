/***************************************************************************//**
 *
 * @file eta_utils_terminal.c
 *
 * @brief Terminal source.
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

#include <string.h>
#include <stdlib.h>
#include "eta_utils_terminal.h"
#include "eta_utils_stdio.h"

//
// The UART for the terminal.
//
tUart *g_psTerminalUart;

/***************************************************************************//**
 *
 * EtaUtilsTerminalInit - Initialize the terminal.
 *
 ******************************************************************************/
void
EtaUtilsTerminalInit(tUart *psUart)
{
    g_psTerminalUart = psUart;
}

/***************************************************************************//**
 *
 * TerminalCharGet - Get a character from the terminal.
 *
 * @param bWait **true** if the function should wait on a character **false**
 *              otherwise.
 *
 ******************************************************************************/
char
EtaUtilsTerminalCharGet(bool bWait)
{
    if(bWait)
    {
        //
        // Wait for a char, read, and pop it.
        //
        return(EtaCspUartGetcWait(g_psTerminalUart, true));
    }
    else
    {
        //
        // Do not wait for a char, read, and pop it.
        //
        return(EtaCspUartGetc(g_psTerminalUart, true));
    }
}

/***************************************************************************//**
 *
 * TerminalPrint - Print to the terminal.
 *
 ******************************************************************************/
void
EtaUtilsTerminalPrint(const char *pcStr)
{
    EtaUtilsStdioPrintf("%s", pcStr);
}

/** @}*/

