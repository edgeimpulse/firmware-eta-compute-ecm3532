/***************************************************************************//**
 *
 * @file eta_csp_nvic.h
 *
 * @brief This file contains eta_csp_nvic module definitions.
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
 * @addtogroup ecm3532nvic-m3 Nested Vectored Interrupt Controller (NVIC)
 * @ingroup ecm3532csp-m3
 * @{
 ******************************************************************************/

#ifndef __ETA_CSP_NVIC_H__
#define __ETA_CSP_NVIC_H__

#include "eta_chip.h"
#include "eta_csp_isr.h"

#ifdef __cplusplus
extern "C" {
#endif

//
// EtaCspNvicIntEnable - Enable an interrupt in the NVIC.
//
extern void EtaCspNvicIntEnable(tIrqNum iIrqNum);

//
// EtaCspNvicIntDisable - Disable an interrupt in the NVIC.
//
extern void EtaCspNvicIntDisable(tIrqNum iIrqNum);

//
// EtaCspNvicIntClear - Clear an interrupt in the NVIC.
//
extern void EtaCspNvicIntClear(tIrqNum iIrqNum);

//
// EtaCspNvicIntDisableAll - Disable all interrupts in the NVIC.
//
extern void EtaCspNvicIntDisableAll(void);

//
// EtaCspNvicIntClearAll - Clear all interrupts in the NVIC.
//
extern void EtaCspNvicIntClearAll(void);

//
// Set the interrupt pending bit.
//
extern void EtaCspNvicIntStatusSet(tIrqNum iIrqNum);

//
// Get the interrupt status.
//
extern reg_nvic_eispr0_t EtaCspNvicIntStatusGet(void);

//
// Reset the M3.
//
extern void EtaCspNvicM3Reset(void);

//
// Reset the SOC.
//
extern void EtaCspNvicSocReset(void);

//
// Set the interrupt priority.
//
extern void EtaCspNvicIntPrioritySet(tIrqNum iIrqNum, uint32_t ui32Priority);

//
// Get the interrupt priority.
//
extern uint32_t EtaCspNvicIntPriorityGet(tIrqNum iIrqNum);

//
// Issue a software triggered interrupt 
//
extern void EtaCspNvicIntSoftTrigger(tIrqNum iIrqNum);

#ifdef __cplusplus
}
#endif

#endif // __ETA_CSP_NVIC_H__

