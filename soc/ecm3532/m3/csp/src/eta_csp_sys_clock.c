/*******************************************************************************
 *
 * @file eta_csp_sys_clock.c
 *
 * @brief This file contains eta_csp_sys_clock module implementation.
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
 ******************************************************************************/

/***************************************************************************//**
 * @addtogroup ecm3532sysclk-m3 System Clock (SYSCLK)
 * @ingroup ecm3532csp-m3
 * @{
 ******************************************************************************/

#include "eta_csp_sys_clock.h"
#include "eta_csp_socctrl.h"

//
// The system clock structure.
//
static tEtaCspSysClock sEtaCspSysClock =
{
    .ui32Freq = 0UL,
    .bInitialized = false,
};

/***************************************************************************//**
 *
 *  EtaCspSysClockInit - Initialize the sys clock.
 *
 ******************************************************************************/
void
EtaCspSysClockInit(void)
{
    //
    // Set the sys clock frequency.
    //
    EtaCspSysClockFreqSet();
}

/***************************************************************************//**
 *
 *  EtaCspSysClockFreqGet - Get the system clock in Hz.
 *
 *  @param pui32SysClockFreq a pointer to the memory to fill in the frequency.
 *  @return Return the status.
 *
 ******************************************************************************/
tEtaStatus
EtaCspSysClockFreqGet(uint32_t *pui32SysClockFreq)
{
    tEtaStatus iStatus = eEtaSuccess;

    //
    // If uninitialized, return failure.
    //
    if(sEtaCspSysClock.bInitialized != true)
    {
        iStatus = eEtaFailure;
    }

    //
    // Else fill in the frequency.
    //
    else
    {
        *pui32SysClockFreq = sEtaCspSysClock.ui32Freq;
    }

    //
    // Return the status.
    //
    return(iStatus);
}

/***************************************************************************//**
 *
 *  EtaCspSysClockFreqSet - Set the system clock.
 *
 *  @param ui32SysClockFreq frequency for the desired system clock.
 *
 ******************************************************************************/
void
EtaCspSysClockFreqSet(void)
{
    //
    // Store the frequncy and indicate initialized.
    //
    sEtaCspSysClock.ui32Freq = EtaCspSocCtrlM3FrequencyGet();
    sEtaCspSysClock.bInitialized = true;
}


