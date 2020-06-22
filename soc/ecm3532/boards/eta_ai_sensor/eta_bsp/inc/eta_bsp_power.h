/***************************************************************************//**
 *
 * @file eta_m3_power.h
 *
 * @brief This is the standard include file for all examples.
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
 * @ingroup ecm3532aisensor-bsp
 * @{
 ******************************************************************************/

#ifndef __ETA_BSP_POWER_H__
#define __ETA_BSP_POWER_H__

#include <stdint.h>
#include "eta_bsp.h"

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 *
 * External function definitions
 *
 ******************************************************************************/

//
// Power down the M3.
//
extern void EtaBspPowerM3Sleep(bool bSaveContext);

//
// Power down the M3 for a number of milliseconds.
//
extern void EtaBspPowerM3SleepMs(uint32_t ui32NumberMilliSeconds);

//
// Stall the M3.
//
extern void EtaBspPowerM3Stall(void);

//
// Stall the M3 for a number of milliseconds.
//
extern void EtaBspPowerM3StallMs(uint32_t ui32NumberMilliSeconds);

#ifdef __cplusplus
}
#endif

#endif // __ETA_BSP_POWER_H__

