/***************************************************************************//**
 *
 * @file eta_bsp_button.c
 *
 * @brief This file contains button module implementation.
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
 * @addtogroup ecm3532button-bsp Button
 * @ingroup ecm3532aisensor-bsp
 * @{
 ******************************************************************************/

#include "eta_bsp_button.h"

/***************************************************************************//**
 *
 * EtaBspButtonInit - Initialize the buttons.
 *
 ******************************************************************************/
void
EtaBspButtonInit(void)
{
    //
    // Initialize the buttons.
    //
    EtaCspGpioPullBitEnable(ETA_BSP_BUTTON0);
    EtaCspGpioPullUpBitSet(ETA_BSP_BUTTON0);
    EtaCspGpioInputEnableBitSet(ETA_BSP_BUTTON0);
}

/***************************************************************************//**
 *
 * EtaBspButtonIsPressed - Return the button state.
 *
 * @param iGpio button GPIO.
 *
 * @return Return **true** if the button is pressed and **false** otherwise.
 *
 ******************************************************************************/
bool
EtaBspButtonIsPressed(tGpioBit iGpio)
{
    //
    // Return the button state.
    //
    return(!EtaCspGpioInputBitGet(iGpio));
}

/***************************************************************************//**
 *
 * EtaBspButtonWaitPressRelease - Wait for a press & release then return the
 *                                pressed time.
 *
 * @param iGpio button GPIO.
 *
 * @return Return the button active time.
 *
 ******************************************************************************/
uint32_t
EtaBspButtonWaitPressRelease(tGpioBit iGpio)
{
    uint32_t ui32ButtonActiveTimeMs;
    uint64_t ui64StartTimeMs;

    //
    // Wait for button press - bit goes low.
    //
    while(!EtaBspButtonIsPressed(iGpio))
    {
    }

    //
    // Get the counter value.
    //
    ui64StartTimeMs = EtaCspTimerCountGetMs();

    //
    // Debounce.
    //
    EtaCspTimerDelayMs(250U);

    //
    // Wait for button release - bit goes high.
    //
    while(EtaBspButtonIsPressed(iGpio))
    {
    }

    //
    // Debounce.
    //
    EtaCspTimerDelayMs(100U);

    //
    // Make sure the button is released.
    //
    while(!EtaBspButtonIsPressed(iGpio))
    {
    }

    //
    // Calculate the button active time.
    //
    ui32ButtonActiveTimeMs = EtaCspTimerCountGetMs() - ui64StartTimeMs;

    //
    // Return the button active time.
    //
    return(ui32ButtonActiveTimeMs);
}

