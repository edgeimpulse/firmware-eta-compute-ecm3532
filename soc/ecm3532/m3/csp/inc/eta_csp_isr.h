/***************************************************************************//**
 *
 * @file eta_csp_isr.h
 *
 * @brief This file contains eta_csp_isr module definitions.
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
 * @addtogroup ecm3532isr-m3 Interrupt Service Routine (ISR)
 * @ingroup ecm3532csp-m3
 * @{
 ******************************************************************************/

#ifndef __ETA_CSP_ISR_H__
#define __ETA_CSP_ISR_H__

#include "eta_status.h"

#ifdef __cplusplus
extern "C" {
#endif

//
//! The size of the interrupt table.
//
#define IRQ_NUM_TABLE_SZ 24U

//
//! The IRQ number typedef.
//
typedef enum
{
    //
    //! GPIO[0-1] IRQs.
    //
    eIrqNumGpio0 = 0,
    eIrqNumGpio1 = 1,

    //
    //! Timer[0-1] IRQs.
    //
    eIrqNumTimer0 = 2,
    eIrqNumTimer1 = 3,

    //
    //! UART[0-1] IRQs.
    //
    eIrqNumUart0 = 4,
    eIrqNumUart1 = 5,

    //
    //! SPI[0-1] IRQs.
    //
    eIrqNumSpi0 = 6,
    eIrqNumSpi1 = 7,

    //
    //! I2C[0-1] IRQs.
    //
    eIrqNumI2c0 = 8,
    eIrqNumI2c1 = 9,

    //
    //! RTC[0-1] IRQs.
    //
    eIrqNumRtc0 = 10,
    eIrqNumRtc1 = 11,

    //
    //! DSP IRQ.
    //
    eIrqNumDsp = 12,

    //
    //! ADC IRQ.
    //
    eIrqNumAdc = 13,

    //
    //! PWM IRQ.
    //
    eIrqNumPwm = 14,

    //
    //! Watchdog timer IRQ.
    //
    eIrqNumWdt = 15,

    //
    //! RTC timer IRQ.
    //
    eIrqNumRtcTimer = 16,

    //
    //! BOD IRQ.
    //
    eIrqNumBod = 17,

    //
    //! SW[0-6] IRQs.
    //
    eIrqNumSw0 = 18,
    eIrqNumSw1 = 19,
    eIrqNumSw2 = 20,
    eIrqNumSw3 = 21,
    eIrqNumSw4 = 22,
    eIrqNumSw5 = 23,
}
tIrqNum;

//
//! Interrupt handler function pointer typedef.
//
typedef void (*tpfIrqHandler)(tIrqNum iIrqNum);

//
// The default ISR handler.
//
extern void EtaCspIsrDefaultHandler(tIrqNum iIrqNum);

//
// Set the ISR handler for a particular interrupt.
//
extern void EtaCspIsrHandlerSet(tIrqNum iIrqNum, tpfIrqHandler pfnHandler);

//
// Set the default interrupt handler.
//
extern void EtaCspIsrDefaultHandlerSet(tIrqNum iIrqNum);

#ifdef __cplusplus
}
#endif

#endif // __ETA_CSP_ISR_H__

