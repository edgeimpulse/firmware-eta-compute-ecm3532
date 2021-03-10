/***************************************************************************//**
 *
 * @file eta_csp_isr.h
 *
 * @brief This file contains eta_csp_isr module implementations for the DSP.
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
 * This is part of revision 1.0.0b1 of the Tensai Chip Support Package.
 *
 ******************************************************************************/

/***************************************************************************//**
 * @addtogroup ecm3532isr-dsp Interrupt Service Routine (ISR)
 * @ingroup ecm3532csp-dsp
 * @{
 ******************************************************************************/

#ifndef __ETA_CSP_ISR_H__
#define __ETA_CSP_ISR_H__

#ifdef __cplusplus
extern "C" {
#endif


#if 1
#include "eta_chip.h"
#else
#include "reg.h"
#include "reg_eta_ecm3532_dsp.h"
#include "api-int_controller.h"
#endif


//
// Default Interrupt Line
//
#define MBOX_ISR_LINE  CF_INT_LINE_0
#define DMA_ISR_LINE   CF_INT_LINE_1
#define OTHER_ISR_LINE CF_INT_LINE_2

//
//! Initialize pointers to key blocks.
//
extern p_hw_intctrl_reg_T p_intctrl;

//
// Defines for ISR table size.
//
#define IRQ_NUM_TABLE_SZ  10U
#define INT_LINE_TABLE_SZ 3U

//
//! IRQ Number enum.
//
// -----------------------------------------------------------------------------
// This enum numbers should match what is in cf_dsp.v (search for "assign
// interrupts")
typedef enum
{
    eIrqNumMbox     = 0,
    eIrqNumI2s      = 1,
    eIrqNumDma0     = 2,
    eIrqNumDma1     = 3,
    eIrqNumDma2     = 4,
    eIrqNumDma3     = 5,
    eIrqNumI2c      = 6,
    eIrqNumSpi      = 7,
    eIrqNumAdc      = 8,
    eIrqNumReserved = 9
} tIrqNum;

//
//! Interrupt handler function pointer typedef.
//
typedef void (*tpfIrqHandler)(tIrqNum iIrqNum, intline_T iIntLine);

/*******************************************************************************
 *
 * External function definitions
 *
 ******************************************************************************/
extern void EtaCspIsrHandlerSet(tIrqNum iIrqNum, intline_T iIntLine,
                                tpfIrqHandler pfnHandler);
extern void EtaCspIsrDefaultHandlerSet(tIrqNum iIrqNum, intline_T iIntLine);
extern void EtaCspIsrDefaultHandler(tIrqNum iIrqNum, intline_T iIntLine);
extern void EtaCspIsrExtIntEnable(tIrqNum iIrqNum, intline_T iIntLine);
extern void EtaCspIsrIntClear(tIrqNum iIrqNum, intline_T iIntLine);
extern void EtaCspIsrExtIntDisable(tIrqNum iIrqNum, intline_T iIntLine);
extern void EtaCspIsrIntTrigger(tIrqNum iIrqNum, intline_T iIntLine);


#ifdef __cplusplus
}
#endif

#endif // __ETA_CSP_ISR_H__

