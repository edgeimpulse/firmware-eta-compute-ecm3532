/***************************************************************************//**
 *
 * @file eta_csp_gpio.c
 *
 * @brief This file contains eta_csp_gpio module implementations.
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

#include "eta_chip.h"
#include "eta_csp_gpio.h"
#include "eta_csp.h"

/***************************************************************************//**
 *
 *  EtaCspGpioRelease - GPIO release.
 *
 *  When the M3 powers down, the always on block freezes the control inputs to
 *  the 32 GPIO pads. These are latched in freeze latches.  When an application
 *  powers back up in a warm start it has to call this function to release the
 *  GPIO freeze latches.
 *
 ******************************************************************************/
void
EtaCspGpioRelease(void)
{
    REG_W1(RTC_PWR_CLEAR, GPIO_FROZEN, 1);
}

/***************************************************************************//**
 *
 *  EtaCspGpioOutputBitSet - Set a GPIO bit.
 *
 *  @param iBit desired GPIO bit to set.
 *
 ******************************************************************************/
void
EtaCspGpioOutputBitSet(tGpioBit iBit)
{
    REG_GPIO_DATA_OUT_SET.V |= (1 << iBit);
}

/***************************************************************************//**
 *
 *  EtaCspGpioOutputBitGet - Get a bit from DATA_OUT register.
 *
 *  @param GPIO bit to get.
 *
 *  @return GPIO bit value.
 *
 ******************************************************************************/
bool
EtaCspGpioOutputBitGet(tGpioBit iBit)
{
    return(!!((REG_GPIO_DATA_OUT.V >> iBit) & 1));
}

/***************************************************************************//**
 *
 *  EtaCspGpioOutputBitClear - Clear a single GPIO bit.
 *
 *  @param iBit desired GPIO bit to clear.
 *
 ******************************************************************************/
void
EtaCspGpioOutputBitClear(tGpioBit iBit)
{
    REG_GPIO_DATA_OUT_CLR.V |= (1 << iBit);
}

/***************************************************************************//**
 *
 *  EtaCspGpioOutputBitToggle - Toggle a single GPIO bit.
 *
 *  @param iBit desired GPIO bit to toggle.
 *
 ******************************************************************************/
void
EtaCspGpioOutputBitToggle(tGpioBit iBit)
{
    REG_GPIO_DATA_OUT_TOG.V |= (1 << iBit);
}

/***************************************************************************//**
 *
 *  EtaCspGpioOutputBitsSet - Set multiple GPIO bits.
 *
 *  @param ui32Bits desired GPIO bits to set.
 *
 ******************************************************************************/
void
EtaCspGpioOutputBitsSet(uint32_t ui32Bits)
{
    REG_GPIO_DATA_OUT_SET.V |= ui32Bits;
}

/***************************************************************************//**
 *
 *  EtaCspGpioOutputBitsClear - Clear multiple GPIO bits.
 *
 *  @param ui32Bits desired GPIO bits to clear.
 *
 ******************************************************************************/
void
EtaCspGpioOutputBitsClear(uint32_t ui32Bits)
{
    REG_GPIO_DATA_OUT_CLR.V = ui32Bits;
}

/***************************************************************************//**
 *
 *  EtaCspGpioOutputBitsToggle - Toggle multiple GPIO bits.
 *
 *  @param ui32Bits desired GPIO bits to toggle.
 *
 ******************************************************************************/
void
EtaCspGpioOutputBitsToggle(uint32_t ui32Bits)
{
    REG_GPIO_DATA_OUT_TOG.V |= ui32Bits;
}

/***************************************************************************//**
 *
 *  EtaCspGpioOutputBits - Write multiple GPIO bits.
 *
 *  @param ui32Bits new GPIO bit state.
 *
 ******************************************************************************/
void
EtaCspGpioOutputBits(uint32_t ui32Bits)
{
    REG_GPIO_DATA_OUT.V = ui32Bits;
}

/***************************************************************************//**
 *
 * EtaCspGpioOutputBitsGet - Get the DATA OUT register bits.
 *
 * @return uint32_t
 *
 ******************************************************************************/
uint32_t
EtaCspGpioOutputBitsGet(void)
{
    return(REG_GPIO_DATA_OUT.V);
}

/***************************************************************************//**
 *
 * EtaCspGpioPullBitEnable - Enable the internal pull resistor.
 *
 * @param iBit GPIO bit
 *
 ******************************************************************************/
void
EtaCspGpioPullBitEnable(tGpioBit iBit)
{
    REG_RTC_PULL_ENABLE.V |= (1 << iBit);
}

/***************************************************************************//**
 *
 * EtaCspGpioPullBitsSet - Enable the internal pull resistor bits.
 *
 * @param ui32Bits - Collection of GPIO bits
 *
 ******************************************************************************/
void
EtaCspGpioPullBitsSet(uint32_t ui32Bits)
{
    REG_RTC_PULL_ENABLE.V = ui32Bits;
}

/***************************************************************************//**
 *
 * EtaCspGpioPullBitDisable - Disable the internal pull resistor.
 *
 * @param iBit GPIO bit
 *
 ******************************************************************************/
void
EtaCspGpioPullBitDisable(tGpioBit iBit)
{
    REG_RTC_PULL_ENABLE.V &= ~(1 << iBit);
}

/***************************************************************************//**
 *
 * EtaCspGpioPullBitsGet - Get the internal pull resistor bits.
 *
 * @return uint32_t
 *
 ******************************************************************************/
uint32_t
EtaCspGpioPullBitsGet(void)
{
    return(REG_RTC_PULL_ENABLE.V);
}

/***************************************************************************//**
 *
 * EtaCspGpioPullUpBitSet - Set the bit pull direction to up.
 *
 * @param iBit GPIO bit
 *
 ******************************************************************************/
void
EtaCspGpioPullUpBitSet(tGpioBit iBit)
{
    REG_RTC_PULL_DIRECTION.V |= (1 << iBit);
}

/***************************************************************************//**
 *
 * EtaCspGpioPullDownBitSet - Set the bit pull direction to down.
 *
 * @param iBit GPIO bit
 *
 ******************************************************************************/
void
EtaCspGpioPullDownBitSet(tGpioBit iBit)
{
    REG_RTC_PULL_DIRECTION.V &= ~(1 << iBit);
}

/***************************************************************************//**
 *
 * EtaCspGpioPullDirectionSet - Set the bit pull directions.
 *
 * @param ui32Bits - Collection of GPIO bits
 *
 ******************************************************************************/
void
EtaCspGpioPullDirectionSet(uint32_t ui32Bits)
{
    REG_RTC_PULL_DIRECTION.V = ui32Bits;
}

/***************************************************************************//**
 *
 * EtaCspGpioPullUpBitsGet - Get the internal pull resistor bits.
 *
 * @return uint32_t
 *
 ******************************************************************************/
uint32_t
EtaCspGpioPullUpBitsGet(void)
{
    return(REG_RTC_PULL_DIRECTION.V);
}

/***************************************************************************//**
 *
 *  EtaCspGpioInputBitGet - Get a DATA INPUT bit from GPIO pad.
 *
 *  @param GPIO DATA INPUT bit to get.
 *
 *  @return GPIO bit value.
 *
 ******************************************************************************/
bool
EtaCspGpioInputBitGet(tGpioBit iBit)
{
    return(!!((REG_GPIO_DATA_IN.V >> iBit) & 1));
}

/***************************************************************************//**
 *
 * EtaCspGpioInputGet - Get the 32-bit value of the Data Input register.
 *
 * @return uint32_t
 *
 ******************************************************************************/
uint32_t
EtaCspGpioInputGet(void)
{
    return(REG_GPIO_DATA_IN.V);
}

/***************************************************************************//**
 *
 * EtaCspGpioInputEnableBitSet - Set an input enable bit.
 *
 * @param iBit GPIO bit
 *
 ******************************************************************************/
void
EtaCspGpioInputEnableBitSet(tGpioBit iBit)
{
    REG_GPIO8_INPUT_ENABLE.V |= (1 << iBit);
}

/***************************************************************************//**
 *
 * EtaCspGpioInputEnableBitClear - Clear an input enable bit.
 *
 * @param iBit GPIO bit
 *
 ******************************************************************************/
void
EtaCspGpioInputEnableBitClear(tGpioBit iBit)
{
    REG_GPIO8_INPUT_ENABLE.V &= ~(1 << iBit);
}

/***************************************************************************//**
 *
 * EtaCspGpioInputEnableSet - Set all output enable bits at once.
 *
 * @param ui32Bits GPIO bits
 *
 ******************************************************************************/
void
EtaCspGpioInputEnableSet(uint32_t ui32Bits)
{
    REG_GPIO8_INPUT_ENABLE.V = ui32Bits;
}

/***************************************************************************//**
 *
 * EtaCspGpioInputEnableGet - Get the input enable bits.
 *
 * @return uint32_t
 *
 ******************************************************************************/
uint32_t
EtaCspGpioInputEnableGet(void)
{
    return(REG_GPIO8_INPUT_ENABLE.V);
}

/***************************************************************************//**
 *
 * EtaCspGpioOutputEnableBitSet - Set an Output enable bit.
 *
 * @param ui32Gpio GPIO number
 *
 ******************************************************************************/
void
EtaCspGpioOutputEnableBitSet(tGpioBit iBit)
{
    REG_GPIO_DATA_ENABLE.V |= (1 << iBit);
}

/***************************************************************************//**
 *
 * EtaCspGpioOutputEnableBitClear - Clear an Output enable bit.
 *
 * @param ui32Gpio GPIO number
 *
 ******************************************************************************/
void
EtaCspGpioOutputEnableBitClear(tGpioBit iBit)
{
    REG_GPIO_DATA_ENABLE.V &= ~(1 << iBit);
}

/***************************************************************************//**
 *
 * EtaCspGpioOutputEnableSet - Set all output enable bits.
 *
 * @param ui32Bits GPIO bits
 *
 ******************************************************************************/
void
EtaCspGpioOutputEnableSet(uint32_t ui32Bits)
{
    REG_GPIO_DATA_ENABLE.V = ui32Bits;
}

/***************************************************************************//**
 *
 * EtaCspGpioOutputEnableGet - Get the output enable bits.
 *
 * @return uint32_t
 *
 ******************************************************************************/
uint32_t
EtaCspGpioOutputEnableGet(void)
{
    return(REG_GPIO_DATA_ENABLE.V);
}

/***************************************************************************//**
 *
 * EtaCspGpioDriveHighSet - Set drive high bits.
 *
 * @param ui32Bits GPIO bits
 *
 ******************************************************************************/
void
EtaCspGpioDriveHighSet(uint32_t ui32Bits)
{
    REG_GPIO8_DRIVE_HIGH.V = ui32Bits;
}

/***************************************************************************//**
 *
 * EtaCspGpioDriveHighGet - Return the current drive strength settings.
 *
 * @return 32-bit drive strength settings.
 *
 ******************************************************************************/
uint32_t
EtaCspGpioDriveHighGet(void)
{
    return(REG_GPIO8_DRIVE_HIGH.V);
}

/***************************************************************************//**
 *
 * EtaCspGpioAIntEnablesSet - Set the GPIO A interrupt enables.
 *
 * @param ui32Bits GPIO bits
 *
 ******************************************************************************/
void
EtaCspGpioAIntEnablesSet(uint32_t ui32Bits)
{
    REG_GPIO8_INT_ENABLEA.V = ui32Bits;
}

/***************************************************************************//**
 *
 * EtaCspGpioAIntEnable - Enable the GPIO A interrupt.
 *
 * @param iBit GPIO bit
 *
 ******************************************************************************/
void
EtaCspGpioAIntEnable(tGpioBit iBit)
{
    REG_GPIO8_INT_ENABLEA.V |= (1 << iBit);
}

/***************************************************************************//**
 *
 * EtaCspGpioAIntDisable - Disable the GPIO A interrupt.
 *
 * @param iBit GPIO bit
 *
 ******************************************************************************/
void
EtaCspGpioAIntDisable(tGpioBit iBit)
{
    REG_GPIO8_INT_ENABLEA.V &= ~(1 << iBit);
}

/***************************************************************************//**
 *
 * EtaCspGpioAIntEnableGet - Get interrupt enable bits.
 *
 * @return Interrupt A enable register value
 *
 ******************************************************************************/
uint32_t
EtaCspGpioAIntEnableGet(void)
{
    return(REG_GPIO8_INT_ENABLEA.V);
}

/***************************************************************************//**
 *
 * EtaCspGpioAIntStatusGet - Interrupt status of GPIO A interrupt.
 *
 * @return Interrupt A status register value
 *
 ******************************************************************************/
uint32_t
EtaCspGpioAIntStatusGet(void)
{
    return(REG_GPIO8_INT_STATUSA.V);
}

/***************************************************************************//**
 *
 * EtaCspGpioAIntClear - Clear the GPIO A interrupt.
 *
 * @param iBit GPIO bit
 *
 ******************************************************************************/
void
EtaCspGpioAIntClear(tGpioBit iBit)
{
    REG_GPIO8_INT_STATUSA_CLR.V = (1 << iBit);
}

/***************************************************************************//**
 *
 * EtaCspGpioAIntClearBits - Clear any number of GPIO A interrupt bits.
 *
 * @param ui32Bits
 *
 ******************************************************************************/
void
EtaCspGpioAIntClearBits(uint32_t ui32Bits)
{
    REG_GPIO8_INT_STATUSA_CLR.V = ui32Bits;
}

/***************************************************************************//**
 *
 * EtaCspGpioAPolarityGet - Return 32-bit value in polarity A register.
 *
 * @return Polarity A register value.
 *
 ******************************************************************************/
uint32_t
EtaCspGpioAPolarityGet(void)
{
    return(REG_GPIO8_POLARITYA.V);
}

/***************************************************************************//**
 *
 * EtaCspGpioAIntPolaritySet - Set Interrupt A polarity bits.
 *
 * @param ui32Bits - Collection of polarity bits.
 *
 ******************************************************************************/
void
EtaCspGpioAIntPolaritySet(uint32_t ui32Bits)
{
    REG_GPIO8_POLARITYA.V = ui32Bits;
}

/***************************************************************************//**
 *
 * EtaCspGpioAIntPolarityActiveLow - Set Interrupt A polarity Active Low
 *
 * @param iBit GPIO bit
 *
 ******************************************************************************/
void
EtaCspGpioAIntPolarityActiveLow(tGpioBit iBit)
{
    REG_GPIO8_POLARITYA_SET.V |= (1 << iBit);
}

/***************************************************************************//**
 *
 * EtaCspGpioAIntPolarityActiveHigh - Set Interrupt A polarity Active High
 *
 * @param iBit GPIO bit
 *
 ******************************************************************************/
void
EtaCspGpioAIntPolarityActiveHigh(tGpioBit iBit)
{
    REG_GPIO8_POLARITYA_CLR.V |= (1 << iBit);
}

/***************************************************************************//**
 *
 * EtaCspGpioBPolarityGet - Return 32-bit value in polarity A register.
 *
 * @return Polarity B register value.
 *
 ******************************************************************************/
uint32_t
EtaCspGpioBPolarityGet(void)
{
    return(REG_GPIO8_POLARITYB.V);
}

/***************************************************************************//**
 *
 * EtaCspGpioBIntPolaritySet - Set Interrupt B polarity bits.
 *
 * @param ui32Bits - Collection of polarity bits.
 *
 ******************************************************************************/
void
EtaCspGpioBIntPolaritySet(uint32_t ui32Bits)
{
    REG_GPIO8_POLARITYB.V = ui32Bits;
}

/***************************************************************************//**
 *
 * EtaCspGpioBIntPolarityActiveLow - Set Interrupt B polarity Active Low
 *
 * @param iBit GPIO bit
 *
 ******************************************************************************/
void
EtaCspGpioBIntPolarityActiveLow(tGpioBit iBit)
{
    REG_GPIO8_POLARITYB_SET.V |= (1 << iBit);
}

/***************************************************************************//**
 *
 * EtaCspGpioBIntPolarityActiveHigh - Set Interrupt B polarity Active High
 *
 * @param iBit GPIO bit
 *
 ******************************************************************************/
void
EtaCspGpioBIntPolarityActiveHigh(tGpioBit iBit)
{
    REG_GPIO8_POLARITYB_CLR.V |= (1 << iBit);
}

/***************************************************************************//**
 *
 * EtaCspGpioBIntEnablesSet - Set the GPIO B interrupt enables.
 *
 * @param ui32Bits GPIO bits
 *
 ******************************************************************************/
void
EtaCspGpioBIntEnablesSet(uint32_t ui32Bits)
{
    REG_GPIO8_INT_ENABLEB.V = ui32Bits;
}

/***************************************************************************//**
 *
 * EtaCspGpioBIntEnable - Enable the GPIO B interrupt.
 *
 * @param iBit GPIO bit
 *
 ******************************************************************************/
void
EtaCspGpioBIntEnable(tGpioBit iBit)
{
    REG_GPIO8_INT_ENABLEB.V |= (1 << iBit);
}

/***************************************************************************//**
 *
 * EtaCspGpioBIntDisable - Disable the GPIO B interrupt.
 *
 * @param iBit GPIO bit
 *
 ******************************************************************************/
void
EtaCspGpioBIntDisable(tGpioBit iBit)
{
    REG_GPIO8_INT_ENABLEB.V &= ~(1 << iBit);
}

/***************************************************************************//**
 *
 * EtaCspGpioBIntEnableGet - Get interrupt enable bits.
 *
 * @return Interrupt A enable register value
 *
 ******************************************************************************/
uint32_t
EtaCspGpioBIntEnableGet(void)
{
    return(REG_GPIO8_INT_ENABLEB.V);
}

/***************************************************************************//**
 *
 * EtaCspGpioBIntStatusGet - Interrupt status of GPIO B interrupt.
 *
 * @return Interrupt B status register value
 *
 ******************************************************************************/
uint32_t
EtaCspGpioBIntStatusGet(void)
{
    return(REG_GPIO8_INT_STATUSB.V);
}

/***************************************************************************//**
 *
 * EtaCspGpioBIntClear - Clear the GPIO B interrupt.
 *
 * @param iBit GPIO bit
 *
 ******************************************************************************/
void
EtaCspGpioBIntClear(tGpioBit iBit)
{
    REG_GPIO8_INT_STATUSB_CLR.V = (1 << iBit);
}

/***************************************************************************//**
 *
 * EtaCspGpioBIntClearBits - Clear any number of GPIO B interrupt bits.
 *
 * @param ui32Bits
 *
 ******************************************************************************/
void
EtaCspGpioBIntClearBits(uint32_t ui32Bits)
{
    REG_GPIO8_INT_STATUSB_CLR.V = ui32Bits;
}

/***************************************************************************//**
 *
 * EtaCspGpioM3TickStart - Start the M3 tick counter
 *
 *
 ******************************************************************************/
void
EtaCspGpioM3TickStart(void)
{
    //
    // Clear and restart the counter
    //
    REG_GPIO_M3_TICK.V = 0x474F;
}

/***************************************************************************//**
 *
 * EtaCspGpioM3TickFreeze - Freeze the M3 tick counter
 *
 *
 ******************************************************************************/
void
EtaCspGpioM3TickFreeze(void)
{
    //
    // Clear and restart the counter
    //
    REG_GPIO_M3_TICK.V = 0x4E4F;
}

/***************************************************************************//**
 *
 * EtaCspGpioM3TickGet - get the m3 oscillator tick count
 *
 *
 ******************************************************************************/
uint64_t
EtaCspGpioM3TickGet(void)
{
    uint64_t ui64WorkA;
    uint64_t ui64WorkB;

    //
    // Freeze the M3 tick count.
    //
    REG_GPIO_M3_TICK.V = 0x4E4F;

    //
    // Grab the lower 32 bits of a tick.
    //
    ui64WorkA = REG_GPIO_M3_TICK.V;

    //
    // Now grab the upper 4 bits and align them
    //
    ui64WorkB = REG_GPIO_CSR.BF.M3_TICK_HI;
    ui64WorkB <<= 32;

    return(ui64WorkA + ui64WorkB);
}

/***************************************************************************//**
 *
 *  EtaCspGpioInit - Initialize the GPIO module.
 *
 ******************************************************************************/
void
EtaCspGpioInit(void)
{
    REG_GPIO8_CFG_STATUS.V = 0UL;
    REG_GPIO_DATA_OUT.V = 0x00000000;
    REG_GPIO_DATA_ENABLE.V = 0x00000000;
    REG_GPIO8_INPUT_ENABLE.V = 0x00000000;
    REG_RTC_PULL_ENABLE.V = 0x00000000;
    REG_RTC_PULL_DIRECTION.V = 0xFFFFFFFF;

    //
    // Ensure that the UART0 pads pull resistors are turned off.
    //
    REG_GPIO8_UART0_CTRL.V = 0x000000f0;

    //
    // Ensure that the SPI0 MISO has the pad pullup enabled.
    //
    REG_GPIO8_SPI0_CTRL.V = 0x00000010;
}

/***************************************************************************//**
 *
 *  EtaCspGpioFence - Establish a fence for things that touch the GPIO module.
 *
 ******************************************************************************/
void
EtaCspGpioFence(void)
{
    REG_GPIO_DEBUG0_READ_ONLY.V = REG_RTC_CSRA.V; // fence
}

