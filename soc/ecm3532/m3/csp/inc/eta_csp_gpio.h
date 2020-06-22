/***************************************************************************//**
 *
 * @file eta_csp_gpio.h
 *
 * @brief This file contains eta_csp_gpio module definitions.
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
 * @addtogroup ecm3532gpio-m3 General Purpose IO (GPIO)
 * @ingroup ecm3532csp-m3
 * @{
 ******************************************************************************/

#ifndef __ETA_CSP_GPIO_H__
#define __ETA_CSP_GPIO_H__

#include "eta_chip.h"
#include "eta_csp.h"
#include "eta_csp_rtc.h"
#ifdef __cplusplus
extern "C" {
#endif

#ifndef __LANGUAGE_ASM__

//
//! GPIO bits union typedef.
//This structure is no longer used and going away -- fixme
//
#ifdef NOTDEF
	fixme remove
typedef union
{
    uint32_t ui32Value;
    struct tGpioBitField
    {
        uint32_t ui32D0                   : 1;
        uint32_t ui32D1                   : 1;
        uint32_t ui32D2                   : 1;
        uint32_t ui32D3                   : 1;
        uint32_t ui32D4                   : 1;
        uint32_t ui32D5                   : 1;
        uint32_t ui32D6                   : 1;
        uint32_t ui32D7                   : 1;
        uint32_t ui32D8                   : 1;
        uint32_t ui32D9                   : 1;
        uint32_t ui32D10                  : 1;
        uint32_t ui32D11                  : 1;
        uint32_t ui32D12                  : 1;
        uint32_t ui32D13                  : 1;
        uint32_t ui32D14                  : 1;
        uint32_t ui32D15                  : 1;
        uint32_t ui32D16                  : 1;
        uint32_t ui32D17                  : 1;
        uint32_t ui32D18                  : 1;
        uint32_t ui32D19                  : 1;
        uint32_t ui32D20                  : 1;
        uint32_t ui32D21                  : 1;
        uint32_t ui32D23                  : 1;
        uint32_t ui32D24                  : 1;
        uint32_t ui32D25                  : 1;
        uint32_t ui32D26                  : 1;
        uint32_t ui32D27                  : 1;
        uint32_t ui32D28                  : 1;
        uint32_t ui32D29                  : 1;
        uint32_t ui32D30                  : 1;
        uint32_t ui32D31                  : 1;
    }
    sGpioBitField;
}
tGpioBits;
#endif

//
//! GPIO bit enum typedef.
//
typedef enum
{
    eGpioBitNone = -1,
    eGpioBit0  = 0,
    eGpioBit1  = 1,
    eGpioBit2  = 2,
    eGpioBit3  = 3,
    eGpioBit4  = 4,
    eGpioBit5  = 5,
    eGpioBit6  = 6,
    eGpioBit7  = 7,
    eGpioBit8  = 8,
    eGpioBit9  = 9,
    eGpioBit10 = 10,
    eGpioBit11 = 11,
    eGpioBit12 = 12,
    eGpioBit13 = 13,
    eGpioBit14 = 14,
    eGpioBit15 = 15,
    eGpioBit16 = 16,
    eGpioBit17 = 17,
    eGpioBit18 = 18,
    eGpioBit19 = 19,
    eGpioBit20 = 20,
    eGpioBit21 = 21,
    eGpioBit22 = 22,
    eGpioBit23 = 23,
    eGpioBit24 = 24,
    eGpioBit25 = 25,
    eGpioBit26 = 26,
    eGpioBit27 = 27,
    eGpioBit28 = 28,
    eGpioBit29 = 29,
    eGpioBit30 = 30,
    eGpioBit31 = 31,
}
tGpioBit;


typedef enum
{
    eGpioCapSelRising   = 0,
    eGpioCapSelFalling  = 1,
}
tGpioCapSelPolarity;

#endif // __LANGUAGE_ASM__

/*******************************************************************************
 *
 * External function definitions
 *
 ******************************************************************************/


//
// EtaCspGpioRelease - GPIO release.
//
extern void EtaCspGpioRelease(void);

//
// EtaCspGpioOutputBitGet - Get a bit from DATA_OUT register.
//
extern bool EtaCspGpioOutputBitGet(tGpioBit iBit);

//
// EtaCspGpioOutputBitSet - Set a GPIO bit.
//
extern void EtaCspGpioOutputBitSet(tGpioBit iBit);

//
// EtaCspGpioOutputBitsSet - Set multiple GPIO bits.
//
// fixme extern void EtaCspGpioOutputBitsSet(tGpioBits uBits);
extern void EtaCspGpioOutputBitsSet(uint32_t ui32Bits);

//
// EtaCspGpioOutputBitClear - Clear a single GPIO bit.
//
extern void EtaCspGpioOutputBitClear(tGpioBit iBit);

//
// EtaCspGpioOutputBitsClear - Clear multiple GPIO bits.
//
// fixme remove extern void EtaCspGpioOutputBitsClear(tGpioBits uBits);
extern void EtaCspGpioOutputBitsClear(uint32_t ui32Bits);

//
// EtaCspGpioOutputBitToggle - Toggle a single GPIO bit.
//
extern void EtaCspGpioOutputBitToggle(tGpioBit iBit);

//
// EtaCspGpioOutputBitsToggle - Toggle multiple GPIO bits.
//
// fixme remove extern void EtaCspGpioOutputBitsToggle(tGpioBits uBits);
extern void EtaCspGpioOutputBitsToggle(uint32_t ui32Bits);

// EtaCspGpioOutputBits - Write multiple GPIO bits.
extern void EtaCspGpioOutputBits(uint32_t ui32Bits);

// EtaCspGpioOutputBitsGet - Get the DATA OUT register bits.
extern uint32_t EtaCspGpioOutputBitsGet(void);

//
// EtaCspGpioPullBitEnable - Enable the internal pull resistor.
//
extern void EtaCspGpioPullBitEnable(tGpioBit iBit);

//
// EtaCspGpioPullBitsSet - Enable the internal pull resistor bits.
//
// fixme remove extern void EtaCspGpioPullBitsSet(tGpioBits uBits);
extern void EtaCspGpioPullBitsSet(uint32_t ui32Bits);

//
// EtaCspGpioPullBitsGet - Get the internal pull resistor bits.
//
extern uint32_t EtaCspGpioPullBitsGet(void);

//
// EtaCspGpioPullBitDisable - Disable the internal pull resistor.
//
extern void EtaCspGpioPullBitDisable(tGpioBit iBit);

//
// EtaCspGpioPullUpBitSet - Set the bit pull direction to up.
//
extern void EtaCspGpioPullUpBitSet(tGpioBit iBit);

//
// EtaCspGpioPullDownBitSet - Set the bit pull direction to down.
//
extern void EtaCspGpioPullDownBitSet(tGpioBit iBit);

//
// EtaCspGpioPullDirectionSet - Set the bit pull directions.
//
// fixme remove extern void EtaCspGpioPullDirectionSet(tGpioBits uBits);
extern void EtaCspGpioPullDirectionSet(uint32_t ui32Bits);

//
// EtaCspGpioPullUpBitsGet - Get the internal pull resistor bits.
//
extern uint32_t EtaCspGpioPullUpBitsGet(void);

//
// EtaCspGpioInputBitGet - Get a DATA INPUT bit from GPIO pad.
//
extern bool EtaCspGpioInputBitGet(tGpioBit iBit);

//
// EtaCspGpioInputGet - Get the 32-bit value of the Data Input register.
//
extern uint32_t EtaCspGpioInputGet(void);

//
// EtaCspGpioInputEnableBitSet - Set an input enable bit.
//
extern void EtaCspGpioInputEnableBitSet(tGpioBit iBit);

//
// EtaCspGpioInputEnableBitClear - Clear an input enable bit.
//
extern void EtaCspGpioInputEnableBitClear(tGpioBit iBit);

// EtaCspGpioInputEnableSet - Set all output enable bits at once.
extern void EtaCspGpioInputEnableSet(uint32_t ui32Bits);

//
//  EtaCspGpioInputEnableGet - Get the input enable bits.
//
extern uint32_t EtaCspGpioInputEnableGet(void);

//
// EtaCspGpioOutputEnableBitSet - Set an Output enable bit.
//
extern void EtaCspGpioOutputEnableBitSet(tGpioBit iBit);

//
// EtaCspGpioOutputEnableBitClear - Clear an Output enable bit.
//
extern void EtaCspGpioOutputEnableBitClear(tGpioBit iBit);

// EtaCspGpioOutputEnableSet - Set all output enable bits.
extern void EtaCspGpioOutputEnableSet(uint32_t ui32Bits);

//
// EtaCspGpioOutputEnableGet - Get the output enable bits.
//
extern uint32_t EtaCspGpioOutputEnableGet(void);

//
// EtaCspGpioDriveHighSet - Set drive high bits.
//
extern void EtaCspGpioDriveHighSet(uint32_t ui32Bits);

//
// EtaCspGpioDriveHighGet - Return the current drive strength settings.
//
extern uint32_t EtaCspGpioDriveHighGet(void);

// EtaCspGpioAIntEnablesSet - Set the GPIO A interrupt enables.
extern void EtaCspGpioAIntEnablesSet(uint32_t ui32Bits);

//
// EtaCspGpioAIntEnable - Enable the GPIO A interrupt.
//
extern void EtaCspGpioAIntEnable(tGpioBit iBit);

//
//  EtaCspGpioAIntDisable - Disable the GPIO A interrupt.
//
extern void EtaCspGpioAIntDisable(tGpioBit iBit);

// EtaCspGpioAIntEnableGet - Get interrupt enable bits.
extern uint32_t EtaCspGpioAIntEnableGet(void);

//
// EtaCspGpioAIntStatusGet - Interrupt status of GPIO A interrupt.
//
extern uint32_t EtaCspGpioAIntStatusGet(void);

//
// EtaCspGpioBIntStatusGet - Interrupt status of GPIO B interrupt.
//
extern uint32_t EtaCspGpioBIntStatusGet(void);

//
// EtaCspGpioAIntClear - Clear the GPIO A interrupt.
//
extern void EtaCspGpioAIntClear(tGpioBit iBit);

//
// EtaCspGpioAIntClearBits - Clear any number of GPIO A interrupt bits.
//
extern void EtaCspGpioAIntClearBits(uint32_t ui32Bits);

//
// EtaCspGpioAPolarityGet - Return 32-bit value in polarity A register.
//
extern uint32_t EtaCspGpioAPolarityGet(void);

//
// EtaCspGpioAIntPolaritySet - Set Interrupt A polarity bits.
//
extern void EtaCspGpioAIntPolaritySet(uint32_t ui32Bits);

//
// EtaCspGpioAIntPolarityActiveLow - Set Interrupt A polarity Active Low
//
extern void EtaCspGpioAIntPolarityActiveLow(tGpioBit iBit);

//
// EtaCspGpioAIntPolarityActiveHigh - Set Interrupt A polarity Active High
//
extern void EtaCspGpioAIntPolarityActiveHigh(tGpioBit iBit);

//
// EtaCspGpioBPolarityGet - Return 32-bit value in polarity A register.
//
extern uint32_t EtaCspGpioBPolarityGet(void);

//
// EtaCspGpioBIntPolaritySet - Set Interrupt B polarity bits.
//
extern void EtaCspGpioBIntPolaritySet(uint32_t ui32Bits);

//
// EtaCspGpioBIntPolarityActiveLow - Set Interrupt B polarity Active Low
//
extern void EtaCspGpioBIntPolarityActiveLow(tGpioBit iBit);

//
// EtaCspGpioBIntPolarityActiveHigh - Set Interrupt B polarity Active High
//
extern void EtaCspGpioBIntPolarityActiveHigh(tGpioBit iBit);

// EtaCspGpioBIntEnablesSet - Set the GPIO B interrupt enables.
extern void EtaCspGpioBIntEnablesSet(uint32_t ui32Bits);

//
// EtaCspGpioBIntEnable - Enable the GPIO B interrupt.
//
extern void EtaCspGpioBIntEnable(tGpioBit iBit);

//
// EtaCspGpioBIntDisable - Disable the GPIO B interrupt.
//
extern void EtaCspGpioBIntDisable(tGpioBit iBit);

// EtaCspGpioBIntEnableGet - Get interrupt enable bits.
extern uint32_t EtaCspGpioBIntEnableGet(void);

//
// EtaCspGpioBIntClear - Clear the GPIO A interrupt.
//
void EtaCspGpioBIntClear(tGpioBit iBit);

//
// EtaCspGpioBIntClearBits - Clear any number of GPIO B interrupt bits.
//
extern void EtaCspGpioBIntClearBits(uint32_t ui32Bits);

//
// EtaCspGpioM3TickStart - Start the M3 tick counter
//
extern void EtaCspGpioM3TickStart(void);

// EtaCspGpioM3TickFreeze - Freeze the M3 tick counter
extern void EtaCspGpioM3TickFreeze(void);

//
// EtaCspGpioM3TickGet - get the m3 oscillator tick count
//
uint64_t EtaCspGpioM3TickGet(void);

//
// Initialize the GPIO module.
//
extern void EtaCspGpioInit(void);

// EtaCspGpioFence - Establish a fence for things that touchte the GPIO module.
extern void EtaCspGpioFence(void);


#ifdef __cplusplus
}
#endif

#endif // __ETA_CSP_GPIO_H__

