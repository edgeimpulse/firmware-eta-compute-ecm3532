/***************************************************************************//**
 *
 * @file eta_bsp_power.c
 *
 * @brief This is the primary power management code for the ECM3532.
 *
 * These functions provide simple methods for handling:
 *    power_up
 *    power_down
 *    stall
 *    Setting M3 target voltages
 *
 * These power management strategies are too simplistic for a real application
 * but are useful for simple examples.
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
 * @addtogroup ecm3532power-bsp Power
 * @ingroup ecm3532evb-bsp
 * @{
 ******************************************************************************/

#include "eta_bsp_power.h"
#include "eta_utils_state.h"

/***************************************************************************//**
 *
 *  EtaBspPowerM3Sleep - Handle sleep and wake on any configured interrupt.
 *
 ******************************************************************************/
void
EtaBspPowerM3Sleep(bool bSaveContext)
{
    //
       // OK everything turns off here when powering down:
       //
       EtaCspPwrRelease(ePwrDomainSram0 |
                        ePwrDomainSram1 |
                        ePwrDomainSram23 |
                        ePwrDomainSram4567 |
                        ePwrDomainBuckMem |
                        ePwrDomainBuckM3 |
                        ePwrDomainFlash |
                        ePwrDomainPeriph);
       //
       // OK now let's just slap the power off button and see what happens
       //
       EtaUtilsStdioPrintf("Sleeping...");
       EtaCspUartTxWait(&g_sUart0);

    //
    // Sleep.
    //
    if(bSaveContext)
    {
        //
        // Save context of where we are and power down.
        // when we wake up and come through reset then it looks like this
        // function returned. Context should be fully restored at this point.
        //
        EtaUtilsStateSavePwrDown((uint32_t *)REG_RTC_NVRAMA_ADDR);

        // NOTE: upon power up, we should end up here.
    }
    else
    {
        //
        // Power down without saving context.
        //
        EtaCspPwrSocShutdown();
    }
}

/***************************************************************************//**
 *
 * EtaBspPowerM3SleepMs - Sleep for a number of milliseconds.
 *
 *  @param ui32NumberMilliSeconds - number of milliseconds to power down for.
 *
 ******************************************************************************/
void
EtaBspPowerM3SleepMs(uint32_t ui32NumberMilliSeconds)
{
    //
    // Start the RTC Timer so that we can wake up from sleep.
    //
    EtaCspRtcTmrTimedRun(ui32NumberMilliSeconds);

    //
    // Power down.
    //
    EtaBspPowerM3Sleep(false);
}

/***************************************************************************//**
 *
 *  EtaBspPowerM3Stall - Stall until awoken by an interrupt.
 *
 ******************************************************************************/
void
EtaBspPowerM3Stall(void)
{
    //
    // Deinitialize the BSP.
    //
    EtaBspDeInit();

    //
    // Prepare for stall
    //
    EtaUtilsStdioPrintf("Stalling...");
    EtaCspUartTxWait(&g_sUart0);

    //
    // Set up for the stall. Note you may want to add additional
    // interrupt conditions here to wake up the M3 from a stall.
    // Here we are only looking for the TMR to wake us up.
    //
    EtaCspBuckM3PreStall();
    EtaCspSocCtrlStallAnyIrq();
    EtaCspBuckM3PostStall();

    //
    // Post-stall
    //
    EtaUtilsStdioPrintf("Awake\r\n");

    //
    // Resume here.
    //
}

/***************************************************************************//**
 *
 * EtaBspPowerM3StallMs - Stall until for the requested number of milliseconds.
 *
 *  @param ui32NumberMilliSeconds - number of milliseconds to power down for.
 *
 ******************************************************************************/
void
EtaBspPowerM3StallMs(uint32_t ui32NumberMilliSeconds)
{
    //
    // Start the RTC Timer so that we can wake up from stall.
    //
    EtaCspRtcTmrTimedRun(ui32NumberMilliSeconds);

    //
    // Stall.
    //
    EtaBspPowerM3Stall();
}

