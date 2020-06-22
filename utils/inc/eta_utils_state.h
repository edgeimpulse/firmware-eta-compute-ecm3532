/***************************************************************************//**
 *
 * @file eta_utils_state.h
 *
 * @brief Functions to help with saving/restoring state.
 *
 * Copyright (C) 2019 Eta Compute, Inc
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
 * @addtogroup etautils-state State Save/Restore
 * @ingroup etautils
 * @{
 ******************************************************************************/

#ifndef __ETA_UTILS_STATE_H__
#define __ETA_UTILS_STATE_H__

#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>
#include "eta_csp_inc.h"

#ifdef __cplusplus
extern "C" {
#endif

//
//! Main application after a state restore.
//
typedef void (*tpfEtaUtilsStateAppMain)(void);

/*******************************************************************************
 *
 * External function definitions
 *
 ******************************************************************************/

//
// Switch to a new stack. This is for bare metal users not RTOS users.
//
extern void EtaUtilsStateNewStack(tpfEtaUtilsStateAppMain pfnAppMain,
                                  uint32_t *pui32NewStackTop);

//
// Conditionally restore the processor state based on cold/warm start.
//     COLD: Execution returns from this function for cold start.
//     WARM: Execution Resumes where EtaUtilsStateSave would have returned.
//
extern void EtaUtilsStateRestore(uint32_t *pui32SpSaveAddress);

//
// Save the processor state on the current stack, save the SP and power down.
//
extern void EtaUtilsStateSavePwrDown(uint32_t *pui32SpSaveAddress);

#ifdef __cplusplus
}
#endif

#endif // __ETA_UTILS_STATE_H__

/** @}*/

