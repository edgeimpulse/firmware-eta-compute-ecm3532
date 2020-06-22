/***************************************************************************//**
 *
 * @file eta_utils_stdio.c
 *
 * @brief Functions to help with IO.
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

#include <stdio.h>
#include "eta_utils_stdio.h"

/***************************************************************************//**
 *
 *  DigitPutC - Put a integer out to the UART.
 *
 *  @param psUart pointer to the UART structure.
 *  @param i32Int the integer to print.
 *
 ******************************************************************************/
static void
DigitPutC(tUart *psUart, int32_t i32Int)
{
    uint8_t ui8Char;

    if(i32Int < 0)
    {
        EtaCspUartPutc(psUart, (uint8_t)'Z');
        return;
    }
    if(i32Int > 9)
    {
        EtaCspUartPutc(psUart, (uint8_t)'Y');
        return;
    }
    ui8Char = 0x30 + i32Int;
    EtaCspUartPutc(psUart, (uint8_t)ui8Char);
}

/***************************************************************************//**
 *
 *  EtaStdioTerminalClear - Clear the terminal.
 *
 ******************************************************************************/
void
EtaUtilsStdioTerminalClear(void)
{
#ifndef USE_SEGGER_RTT_PRINTF
    EtaCspIoPuts("\033[2J\033[1;1H");
#else
    SEGGER_RTT_printf(0, "", RTT_CTRL_RESET);
#endif
}

/***************************************************************************//**
 *
 *  EtaUtilsStdioFreqPutS - Put the frequency out to the UART.
 *
 *  @param psUart pointer to the UART structure.
 *  @param ui32Freq is the frequency to print.
 *
 ******************************************************************************/
void
EtaUtilsStdioFreqPutS(tUart *psUart, uint32_t ui32Freq)
{
    uint32_t ui32WorkA, ui32WorkB, ui32WorkC;

    //
    // Initialize ui32WorkA to the frequency.
    //
    ui32WorkA = ui32Freq;

    //
    // Integer part to the left of the decimal point.
    //
    ui32WorkB = ui32WorkA / 1000000;

    //
    // Truncate to integer part.
    //
    ui32WorkC = ui32WorkB * 1000000;

    //
    // Fractional part is in workA.
    //
    ui32WorkA = ui32WorkA - ui32WorkC;

    //
    // 4 digits to the right of the decimal point.
    //
    ui32WorkC = ui32WorkA / 100;

    //
    // Print 100s,10s,1s digits
    // The IFs prevent leading 0s for aesthetics (we always put 1s digits even
    // if 0)
    //
    // We cannot handle > 999MHz.
    //
    if((ui32WorkB / 100))
    {
        DigitPutC(psUart, (ui32WorkB / 100) % 10);
    }
    if((ui32WorkB / 100) || (ui32WorkB / 10))
    {
        DigitPutC(psUart, (ui32WorkB / 10) % 10);
    }
    DigitPutC(psUart, (ui32WorkB / 1) % 10);

    //
    // Decimal point.
    //
    EtaCspUartPutc(psUart, (uint8_t)'.');

    //
    // Print fractional results.
    //
    DigitPutC(psUart, (ui32WorkC / 1000) % 10);
    DigitPutC(psUart, (ui32WorkC / 100) % 10);
    DigitPutC(psUart, (ui32WorkC / 10) % 10);
    DigitPutC(psUart, (ui32WorkC / 1) % 10);
}

/** @}*/

