/***************************************************************************//**
 *
 * @file eta_csp_timer.h
 *
 * @brief This file contains eta_csp_timer module definitions.
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
 * This is part of revision 1.0.0b1 of the Tensai Chip Support Package.
 *
 ******************************************************************************/

/***************************************************************************//**
 * @addtogroup ecm3532timer-m3 Timer
 * @ingroup ecm3532csp-m3
 * @{
 ******************************************************************************/

#ifndef __ETA_CSP_TIMER_H__
#define __ETA_CSP_TIMER_H__

#include <stdint.h>
#include "eta_csp.h"
#include "memio.h"

#include "eta_csp_gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

////////////////////////////////////////////////////////////////////////////////
//
//! Comparator Selection Enum
//
////////////////////////////////////////////////////////////////////////////////
typedef enum
{
    eTimerComparatorA = 0,
    eTimerComparatorB = 1,
    eTimerComparatorC = 2,
    eTimerComparatorD = 3,
}
tTimerComparatorNumber;

////////////////////////////////////////////////////////////////////////////////
//
//! Interrupt Request Selection Enum
//
////////////////////////////////////////////////////////////////////////////////
typedef enum
{
    eTimer0 = 0,
    eTimer1 = 1,
}
tTimerIrqNumber;

////////////////////////////////////////////////////////////////////////////////
//
//! Comparator Selection Enum
//
////////////////////////////////////////////////////////////////////////////////
typedef enum
{
    eTimerIntCompareA = 0,
    eTimerIntCompareB = 1,
    eTimerIntCompareC = 2,
    eTimerIntCompareD = 3,
    eTimerIntCaptured = 4,
    eTimerIntLowOflow = 5,
    eTimerIntHiOflow  = 6,
}
tTimerInterrupts;

//
// This fence will compile inline and do only one byte reads from the TIMER block.
//
#define EtaCspTimerFenceFast() \
                             {REG_GPIO_DEBUG0_READ_ONLY.V = MEMIO8(REG_TIMER_CFG_STATUS_ADDR);}


/*******************************************************************************
 *
 * External function definitions
 *
 ******************************************************************************/

//
// Add a fence on the last bus operation to SOC Ctrl registers.
//
extern void EtaCspTimerFence(void);

//
// Initialize the timer to count in milliseconds.
//
extern void EtaCspTimerInitMs(void);

//
// Reset the timer.
//
extern void EtaCspTimerSoftReset(void);

//
// Return the timer count in milliseconds.
//
extern uint64_t EtaCspTimerCountGetMs(void);

//
// Delay for a number of milliseconds.
//
extern void EtaCspTimerDelayMs(uint64_t ui64DelayMs);

//
// Get the timer ticks
//
extern void EtaCspTimerTicksGet(uint32_t *ui32CntHi, uint32_t *ui32CntLo);

//
// Set comparator ticks.
//
extern void EtaCspTimerCmpSet(uint32_t ui32Cycle, tTimerComparatorNumber iCmp);

//
// Get timer interrupt status bit.
//
extern int32_t EtaCspTimerIntGet(tTimerInterrupts iIBit);

//
// Clear timer interrupt status bit.
//
extern void EtaCspTimerIntClear(tTimerInterrupts iIBit);

//
// EtaCspTimerIntAllClear Clear all timer interrupt bit value.
//
void EtaCspTimerIntAllClear(void);

//
// EtaCspTimerIntAllGet Get all of the timer interrupt bits.
//
extern uint32_t EtaCspTimerIntAllGet(void);

//
// Set timer interrupt status bit. (useful for progam debug)
//
extern void EtaCspTimerIntSet(tTimerInterrupts iIBit);

//
// EtaCspTimerIntAllClear Clear all timer interrupt bit value.
//
extern void EtaCspTimerIntAllClear(void);


//
// EtaCspTimerIntEnaleAllGet Get all of the timer interrupt enable bits.
//
extern uint32_t EtaCspTimerIntEnableAllGet(tTimerIrqNumber iNumber);

//
// Get timer interrupt enable bit.
//
extern bool EtaCspTimerIntEnableGet(tTimerIrqNumber iNumber, 
                                    tTimerInterrupts iIBit);

//
// Timer set interrupt enable.
//
extern void EtaCspTimerIntEnableSet(tTimerIrqNumber iNumber,
                                    tTimerInterrupts iIBit);

//
// Timer clear interrupt enable.
//
extern void EtaCspTimerIntEnableClear(tTimerIrqNumber iNumber,
                                      tTimerInterrupts iIBit);

//
// EtaCspTimerIntEnableAllClear Clear all timer interrupt bit value.
//
extern void EtaCspTimerIntEnableAllClear(tTimerIrqNumber iNumber);

//
// Enable capture from counter based on gpio pin. NOTE: the GPIO selector mux
// will be programmed here as well.
//
extern void EtaCspTimerCaptureEnable(tGpioCapSelPolarity iPolarity,
                                     tGpioBit iGpioBit);

//
// Disable capturing from counter based on gpio pin.
//
extern void EtaCspTimerCaptureDisable(void);

// EtaCspTimerCaptureStateGet - Disable Capturing time on the edge of a GPIO
// pin.
extern bool EtaCspTimerCaptureStateGet(void);

//
// Read 64-bit capture register value.
//
extern uint64_t EtaCspTimerCaptureGet(void);

//
// Read lower 32-bits of capture register value.
//
extern uint32_t EtaCspTimerCaptureLowGet(void);

//
// EtaCspTimerFlagGet - Get the state of the timer toggle flag.
//
extern bool EtaCspTimerFlagGet(void);

#ifdef __cplusplus
}
#endif

#endif // __ETA_CSP_TIMER_H__

