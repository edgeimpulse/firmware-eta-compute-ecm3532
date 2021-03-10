/***************************************************************************//**
 *
 * @file eta_csp_isr.c
 *
 * @brief This file contains eta_csp_isr module implementations.
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

#include "eta_csp_isr.h"
#include "eta_chip.h"
#include "eta_csp_io.h"

#define ETA_CSP_ISR_DEFAULT_ISR_SPINS_FOREVER 1 // If 1, we loop in the default
                                                // handler forever and
                                                // effectively lockup
#define ETA_CSP_ISR_DEFAULT_ISR_CLEAR_INT     0   // If 1, we reenable the
                                                  // interrupt.

//////////////////////////////////////////////////////////////////////
// Globals
p_hw_intctrl_reg_T p_intctrl = (p_hw_intctrl_reg_T)INT_CONTROLLER_ADDRESS;

// Globals
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// The IRQ handler table.
// 3 levels of interrupts, 10 interrupts each, so array is 30 entries.
static tpfIrqHandler ppfnHandlerTable[IRQ_NUM_TABLE_SZ][INT_LINE_TABLE_SZ] =
{
    EtaCspIsrDefaultHandler, EtaCspIsrDefaultHandler, EtaCspIsrDefaultHandler,
    EtaCspIsrDefaultHandler, EtaCspIsrDefaultHandler, EtaCspIsrDefaultHandler,
    EtaCspIsrDefaultHandler, EtaCspIsrDefaultHandler, EtaCspIsrDefaultHandler,
    EtaCspIsrDefaultHandler, EtaCspIsrDefaultHandler, EtaCspIsrDefaultHandler,
    EtaCspIsrDefaultHandler, EtaCspIsrDefaultHandler, EtaCspIsrDefaultHandler,
    EtaCspIsrDefaultHandler, EtaCspIsrDefaultHandler, EtaCspIsrDefaultHandler,
    EtaCspIsrDefaultHandler, EtaCspIsrDefaultHandler, EtaCspIsrDefaultHandler,
    EtaCspIsrDefaultHandler, EtaCspIsrDefaultHandler, EtaCspIsrDefaultHandler,
    EtaCspIsrDefaultHandler, EtaCspIsrDefaultHandler, EtaCspIsrDefaultHandler,
    EtaCspIsrDefaultHandler, EtaCspIsrDefaultHandler, EtaCspIsrDefaultHandler
};

// The IRQ handler table.
//////////////////////////////////////////////////////////////////////

/***************************************************************************//**
 *
 *  EtaCspIsrHandlerSet - Set the handler for an interrupt.
 *
 *  @param iIrqNum is the interrupt number.
 *  @param int_line is the DSP interrupt line number.
 *  @param handler a pointer to an IRQ function to call.
 *
 ******************************************************************************/
void
EtaCspIsrHandlerSet(tIrqNum iIrqNum, intline_T int_line, tpfIrqHandler handler)
{
    ppfnHandlerTable[iIrqNum][int_line - 1] = handler;
}

/***************************************************************************//**
 *
 *  EtaCspIsrDefaultHandlerSet - Set the default handler for an interrupt.
 *
 *  @param iIrqNum is the interrupt number.
 *  @param int_line is the DSP interrupt line number.
 *
 ******************************************************************************/
void
EtaCspIsrDefaultHandlerSet(tIrqNum iIrqNum, intline_T int_line)
{
    EtaCspIsrHandlerSet(iIrqNum, int_line, EtaCspIsrDefaultHandler);
}

/***************************************************************************//**
 *
 *  EtaCspIsrDefaultHandler - Default ISR Handler
 *
 *  @param iIrqNum is the interrupt number.
 *  @param int_line is the DSP interrupt line number.
 *
 ******************************************************************************/
void
EtaCspIsrDefaultHandler(tIrqNum iIrqNum, intline_T int_line)
{
    // The default (currently) doesn't use iIrqNum or int_line. Tell the
    // compiler to suppress warnings.
    chess_dont_warn_dead(iIrqNum);
    chess_dont_warn_dead(int_line);

    // FIXME, what to do in default handler.     __asm__ __volatile__("BKPT
    // #0");
    // If we set this bit, we'll spin forever. Otherwise, we silently drop bad
    // ISR.
    // Note that if we don't spin forever, we need to clear the interrupt or
    // we'll just keep
    // jumping back into the same ISR.

    while(ETA_CSP_ISR_DEFAULT_ISR_SPINS_FOREVER)
    {
    }

    if(ETA_CSP_ISR_DEFAULT_ISR_CLEAR_INT)
    {
        EtaCspIsrIntClear(iIrqNum, int_line);
    }
}

// CF_INT_LINE_n is numbered 1 to 3, so we need to subtract to get 0-2
#define CHECK_BITMASK_CALL_ISR(ISR_NUM, ISR_LINE)                       \
    if(bitmask & BITMASK(ISR_NUM))ppfnHandlerTable[ISR_NUM][ISR_LINE -  \
                                                            1](ISR_NUM, \
                                                               ISR_LINE);

// Note, CF_INT_LINE_0 is enumerated to 1. So don't use that in the define below

/***************************************************************************//**
 *
 *  eta_csp_isr0 - ISR Line 0 Handler.
 *
 *  Targeted for Mailbox Interrupts
 *
 ******************************************************************************/
extern "C" void
eta_csp_isr0(void) property(isr)
{
    int bitmask;

    bitmask = intctrl_getActiveInterruptsBitmask(p_intctrl, CF_INT_LINE_0);

    CHECK_BITMASK_CALL_ISR(eIrqNumMbox, CF_INT_LINE_0);
}

/***************************************************************************//**
 *
 *  eta_csp_isr1 - ISR Line 1 Handler.
 *
 *  Targeted for DMA Interrupts
 *
 ******************************************************************************/
extern "C" void
eta_csp_isr1(void) property(isr)
{
    int bitmask;

    bitmask = intctrl_getActiveInterruptsBitmask(p_intctrl, CF_INT_LINE_1);

    CHECK_BITMASK_CALL_ISR(eIrqNumDma0, CF_INT_LINE_1);
    CHECK_BITMASK_CALL_ISR(eIrqNumDma1, CF_INT_LINE_1);
    CHECK_BITMASK_CALL_ISR(eIrqNumDma2, CF_INT_LINE_1);
    CHECK_BITMASK_CALL_ISR(eIrqNumDma3, CF_INT_LINE_1);
}

/***************************************************************************//**
 *
 *  eta_csp_isr2 - ISR Line 2 Handler.
 *
 *  Targeted for Other Interrupts
 *
 ******************************************************************************/
extern "C" void
eta_csp_isr2(void) property(isr)
{
    int bitmask;

    bitmask = intctrl_getActiveInterruptsBitmask(p_intctrl, CF_INT_LINE_2);

    CHECK_BITMASK_CALL_ISR(eIrqNumI2s, CF_INT_LINE_2);
    CHECK_BITMASK_CALL_ISR(eIrqNumI2c, CF_INT_LINE_2);
    CHECK_BITMASK_CALL_ISR(eIrqNumSpi, CF_INT_LINE_2);
    CHECK_BITMASK_CALL_ISR(eIrqNumAdc, CF_INT_LINE_2);
}

/***************************************************************************//**
 *
 *  EtaCspIsrExtIntEnable - Map and Enable an interrupt to a specific line
 *
 *  @param iIrqNum is the interrupt number.
 *  @param int_line is the interrupt number.
 *
 ******************************************************************************/
void
EtaCspIsrExtIntEnable(tIrqNum iIrqNum, intline_T int_line)
{
    // Map a hardware interrupt to a CoolFlux interrupt line:
    intctrl_associateInterruptLine(p_intctrl, iIrqNum, int_line);

    // Arguably not needed, default is already rising... but there is value to
    // be sure... OPTME OPTIMIZEME
    // Configure which edge triggers a hardware interrupt (all known interrupts
    // are active high)
    intctrl_setPolarity(p_intctrl, iIrqNum, INTCTRL_RISING_EDGE);

    // Enable Interrupt
    intctrl_enableInterrupt(p_intctrl, iIrqNum);
}

/***************************************************************************//**
 *
 *  EtaCspIsrExtIntDisable - Disable an interrupt (from all lines)
 *
 *  @param iIrqNum is the interrupt number.
 *  @param int_line is the interrupt number (not used, but creates common
 *interface)
 *
 ******************************************************************************/
void
EtaCspIsrExtIntDisable(tIrqNum iIrqNum, intline_T int_line)
{
    chess_dont_warn_dead(int_line);
    intctrl_disableInterrupt(p_intctrl, iIrqNum);
}

/***************************************************************************//**
 *
 *  EtaCspIsrIntClear - Clear an interrupt
 *
 *  @param iIrqNum is the interrupt number.
 *  @param int_line is the interrupt number
 *
 ******************************************************************************/
void
EtaCspIsrIntClear(tIrqNum iIrqNum, intline_T int_line)
{
    intctrl_clearInterrupts_bitmask(p_intctrl, int_line, BITMASK(iIrqNum));
}

/***************************************************************************//**
 *
 *  EtaCspIsrIntTrigger - Manually trigger interrupt (debug)
 *
 *  @param iIrqNum is the interrupt number.
 *  @param int_line is the interrupt number
 *
 ******************************************************************************/
void
EtaCspIsrIntTrigger(tIrqNum iIrqNum, intline_T int_line)
{
    chess_dont_warn_dead(int_line);
    intctrl_triggerInterrupt(p_intctrl, iIrqNum);
}

