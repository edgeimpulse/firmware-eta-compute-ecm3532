/*******************************************************************************
 *
 * @file eta_csp_sys_clock.h
 *
 * @brief This file contains eta_csp_sys_clock module definitions
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

#ifndef __ETA_CSP_SYS_CLOCK_H__
#define __ETA_CSP_SYS_CLOCK_H__

#include <stdint.h>
#include <stdbool.h>
#include "eta_status.h"

#ifdef __cplusplus
extern "C" {
#endif

//
//! Clock structure.
//
typedef struct
{
    //
    //! Frequency.
    //
    uint32_t ui32Freq;

    //
    //! Boolean to track if the clock has been initialized.
    //
    bool bInitialized;
}
tEtaCspSysClock;

//
// Initialize the system clock.
//
extern void EtaCspSysClockInit(void);

//
// Return the system clock.
//
extern tEtaStatus EtaCspSysClockFreqGet(uint32_t *pui32SysClockFreq);

//
// Set the system clock.
//
extern void EtaCspSysClockFreqSet(void);

#ifdef __cplusplus
}
#endif

#endif // __ETA_CSP_SYS_CLOCK_H__

