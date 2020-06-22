/***************************************************************************//**
 *
 * @file eta_bsp_led.c
 *
 * @brief This file contains LED module implementation.
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
 * @addtogroup ecm3532led-bsp LED
 * @ingroup ecm3532aisensor-bsp
 * @{
 ******************************************************************************/

#include "eta_bsp_led.h"

static const tGpioBit g_iLedArray[ETA_BSP_NUM_LEDS] =
{
    ETA_BSP_LED0,
    ETA_BSP_LED1,
    ETA_BSP_LED2,
    ETA_BSP_LED3,
    ETA_BSP_LED4
};

/***************************************************************************//**
 *
 * EtaBspLedsInit - Initialize the LEDs.
 *
 ******************************************************************************/
void
EtaBspLedsInit(void)
{
    uint32_t ui32I;

    //
    // Initialize the LEDs.
    //
    for(ui32I = 0; ui32I < ETA_BSP_NUM_LEDS; ui32I++)
    {
        EtaCspGpioPullBitDisable(g_iLedArray[ui32I]);
        EtaCspGpioOutputEnableBitSet(g_iLedArray[ui32I]);
        if(ETA_BSP_LED_POLARITY_LOW_IS_ON)
        {
            EtaCspGpioOutputBitSet(g_iLedArray[ui32I]);
        }
    }
}

/***************************************************************************//**
 *
 * EtaBspLedsValueSet - Set multiple LEDs based on the passed value.
 *
 * @param ui8Value value to set LEDs to.
 *
 ******************************************************************************/
void
EtaBspLedsValueSet(uint8_t ui8Value)
{
    uint32_t ui32Set;

    ui32Set = (REG_GPIO_DATA_OUT.V | ETA_BS_LED_MASK);
#if ETA_BSP_LED_POLARITY_LOW_IS_ON
    ui8Value = ~ui8Value;
#endif
    ui32Set &= (((ui8Value & 0x1) << ETA_BSP_LED0) |
                (((ui8Value & 0x2) >> 1) << ETA_BSP_LED1) |
                (((ui8Value & 0x4) >> 2) << ETA_BSP_LED2) |
                (((ui8Value & 0x8) >> 3) << ETA_BSP_LED3) |
                (((ui8Value & 0x10) >> 4) << ETA_BSP_LED4));

    REG_GPIO_DATA_OUT.V = ui32Set;
}

/***************************************************************************//**
 *
 * EtaBspLedsSetAll - Set all LEDs
 *
 ******************************************************************************/
void
EtaBspLedsSetAll(void)
{
    uint32_t ui32I;

    for(ui32I = 0; ui32I < ETA_BSP_NUM_LEDS; ui32I++)
    {
        EtaBspLedSet(g_iLedArray[ui32I]);
    }

}

/***************************************************************************//**
 *
 * EtaBspLedSet - Set a single LED.
 *
 * @param iLed LED to set.
 *
 ******************************************************************************/
void
EtaBspLedSet(tGpioBit iLed)
{
#if ETA_BSP_LED_POLARITY_LOW_IS_ON
    //
    // Clear the LED.
    //
    REG_GPIO_DATA_OUT_CLR.V = (1 << iLed);
#else
    //
    // Set the LED.
    //
    REG_GPIO_DATA_OUT_SET.V = (1 << iLed);
#endif
}

/***************************************************************************//**
 *
 * EtaBspLedsClearAll - Clear all LEDs.
 *
 ******************************************************************************/
void
EtaBspLedsClearAll(void)
{
    uint32_t ui32I;

    for(ui32I = 0; ui32I < ETA_BSP_NUM_LEDS; ui32I++)
    {
        EtaBspLedClear(g_iLedArray[ui32I]);
    }
}

/***************************************************************************//**
 *
 * EtaBspLedClear - Clear a single LED.
 *
 * @param iLed LED to clear.
 *
 ******************************************************************************/
void
EtaBspLedClear(tGpioBit iLed)
{
#if ETA_BSP_LED_POLARITY_LOW_IS_ON
    //
    // Set the LED.
    //
    REG_GPIO_DATA_OUT_SET.V = (1 << iLed);
#else
    //
    // Clear the LED.
    //
    REG_GPIO_DATA_OUT_CLR.V = (1 << iLed);
#endif
}

/***************************************************************************//**
 *
 * EtaBspLedToggle - Toggle an LED.
 *
 * @param iLed LED to toggle.
 *
 ******************************************************************************/
void
EtaBspLedToggle(tGpioBit iLed)
{
    //
    // Toggle the LED.
    //
    REG_GPIO_DATA_OUT_TOG.V = (1 << iLed);
}

