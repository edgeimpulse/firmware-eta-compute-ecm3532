/***************************************************************************//**
 *
 * @file eta_csp_io.c
 *
 * @brief This file contains eta_csp_io module implementation.
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

#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>
#include "eta_csp.h"
#include "eta_csp_uart.h"
#include "eta_csp_io.h"

#ifndef NULL
#define NULL (0)
#endif

static tUart g_sUart;

static bool g_bRetarget2Backdoor = false;

static char *IoConvert(uint32_t ui32Num, uint32_t ui32Base);

static const char g_pcRepresentation[] = "0123456789ABCDEF";

/***************************************************************************//**
 *
 *  IoConvert - Convert the number based on the base.
 *
 *  @param ui32Num number to convert.
 *  @param ui32Base base to convert to.
 *  @return Return a pointer to the character.
 *
 ******************************************************************************/
static char *
IoConvert(uint32_t ui32Num, uint32_t ui32Base)
{
    static char pcBuffer[55];
    char *pcReturn;

    pcReturn = &pcBuffer[49];
    *pcReturn = '\0';

    do
    {
        *--pcReturn = g_pcRepresentation[ui32Num % ui32Base];
        ui32Num /= ui32Base;
    }
    while(ui32Num != 0);

    return(pcReturn);
}
/***************************************************************************//**
 *
 *  EtaCspIoPutc - Transmit a character.
 *
 *  @param cChar is the character to transmit.
 *
 ******************************************************************************/
void
EtaCspIoPutc(char cChar)
{

    if(g_bRetarget2Backdoor)
    {
    }

    else
    {
        EtaCspUartPutc(&g_sUart, cChar);
    }
}
/***************************************************************************//**
 *
 *  EtaCspIoPuts - Transmit a string.
 *
 *  @param pcStr is a pointer to the sting to transmit.
 *
 ******************************************************************************/
void
EtaCspIoPuts(char *pcStr)
{
    if(g_bRetarget2Backdoor)
    {
    }
    else
    {
        EtaCspUartPuts(&g_sUart, pcStr);
    }
}
/***************************************************************************//**
 *
 *  EtaCspIoPrintf - Writes the C string pointed by format to the terminal.
 *
 *  @param pcFormat is a pointer to string that contains the text to be written.
 *  @param Depending on the format string, the function may expect a sequence of
 *         additional arguments, each containing a value to be used to replace a
 *         format specifier in the format string (or a pointer to a storage
 *         location, for n).  There should be at least as many of these
 *         arguments as the number of values specified in the format specifiers.
 *         Additional arguments are ignored by the function.
 *
 ******************************************************************************/
void
EtaCspIoPrintf(char *pcFormat, ...)
{
    char *pcTraverse;
    uint32_t ui32I;
    char *pcString;
    va_list psArgs;

    //
    // Initialize psArgs to retrieve the additional arguments after parameter
    // pcFormat.
    //
    va_start(psArgs, pcFormat);

    //
    // Walk the characters.
    //
    pcTraverse = pcFormat;
    while(*pcTraverse != '\0')
    {
        while(*pcTraverse != '%')
        {
            if(*pcTraverse == 0)
            {
                break;
            }
            EtaCspIoPutc(*pcTraverse);
            pcTraverse++;
        }

        if(*pcTraverse == 0)
        {
            break;
        }

        //
        // Skip over '%' character.
        //
        pcTraverse++;

        //
        // Fetching and executing arguments.
        //
        switch(*pcTraverse)
        {
            //
            // Fetch char argument.
            //
            case 'c':
            {
                ui32I = va_arg(psArgs, int);
                EtaCspIoPutc(ui32I);
                break;
            }

            //
            // Fetch decimel/integer argument.
            //
            case 'd':
            {
                ui32I = va_arg(psArgs, int);
                if((int)ui32I < 0)
                {
                    ui32I = -ui32I;
                    EtaCspIoPutc('-');
                }
                EtaCspIoPuts(IoConvert(ui32I, 10));
                break;
            }

            //
            // Fetch octal argument.
            //
            case 'o':
            {
                ui32I = va_arg(psArgs, uint32_t);
                EtaCspIoPuts(IoConvert(ui32I, 8));
                break;
            }

            //
            // Fetch string argument.
            //
            case 's':
            {
                pcString = va_arg(psArgs, char *);
                EtaCspIoPuts(pcString);
                break;
            }

            //
            // Fetch the hex argument.
            //
            case 'x':
            {
                ui32I = va_arg(psArgs, uint32_t);
                EtaCspIoPuts(IoConvert(ui32I, 16));
                break;
            }

            //
            // Default case.
            //
            default:
            {
                break;
            }
        }

        //
        // Skip over {c,d,o,s,x} character
        //
        pcTraverse++;
    }

    //
    // Closing argument list to clean-up.
    //
    va_end(psArgs);
}
/***************************************************************************//**
 *
 *  EtaCspIoTxWait - Wait the UART TX to be finished.
 *
 ******************************************************************************/
void
EtaCspIoTxWait(void)
{
    EtaCspUartTxWait(&g_sUart);
}
/***************************************************************************//**
 *
 *  EtaCspIoPrintfRedirect - Target the UART to the backdoor.
 *
 *  @param psUart is a pointer to a UART structure.
 *
 ******************************************************************************/
void
EtaCspIoPrintfRedirect(tUart *psUart)
{
    if(psUart == NULL)
    {
        g_bRetarget2Backdoor = true;
    }
    else
    {
        g_bRetarget2Backdoor = false;
        g_sUart.iNum = psUart->iNum;
        g_sUart.iBaud = psUart->iBaud;
    }
}
/***************************************************************************//**
 *
 *  EtaCspIoTerminalClear - Clear the terminal.
 *
 ******************************************************************************/
void
EtaCspIoTerminalClear(void)
{
    EtaCspIoPuts("\033[2J\033[1;1H");
}

