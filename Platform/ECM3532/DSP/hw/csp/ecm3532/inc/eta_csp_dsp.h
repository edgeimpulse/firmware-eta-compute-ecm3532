/***************************************************************************//**
 *
 * @file eta_csp_dsp.h
 *
 * @brief This file contains eta_csp_dsp module definitions.
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
 * @addtogroup ecm3532dsp-dsp Digital Signal Processor (DSP)
 * @ingroup ecm3532csp-dsp
 * @{
 ******************************************************************************/

//
// DSP CSP
//
#ifndef ETA_CSP_DSP_H
#define ETA_CSP_DSP_H

#include "reg.h"
#include "reg_eta_ecm3532_dsp.h"
#include "eta_csp_mailbox.h"
#include "eta_csp_io.h"

/*******************************************************************************
 *
 * External function definitions.
 *
 ******************************************************************************/

/***************************************************************************//**
 *
 *  EtaCspDspSetupForSleep - Sets up local logic to allow for sleep operations
 *                           where the DSP clock stops and will be restarted
 *                           when a peripheral indicates it has work to do
 *                           for either DSP core or DMA.
 *
 *  Note: This enables all peripherals to wake the system. Generally, all
 *        peripherals must be enabled to trigger the wake signal in the
 *        peripheral logic (e.g. enable interrupts or source ready, etc), so
 *        enabling all peripherals is reasonably safe barring misconfuration.
 *
 ******************************************************************************/
extern void EtaCspDspSetupForSleep(void);

/***************************************************************************//**
 *
 *  EtaCspDspGoToSleep - Places the DSP Core in sleep, and keeps it there until
 * we are ready.
 *
 ******************************************************************************/
extern void EtaCspDspGoToSleep(void);

/***************************************************************************//**
 *
 *  EtaCspDspWakeFromSleep - Places the DSP Core in fully running, non sleep
 *                           mode.
 *
 ******************************************************************************/
extern void EtaCspDspWakeFromSleep(void);

/***************************************************************************//**
 *
 *  EtaCspDspDebugHalt - Places the DSP Core in its processor halt state
 *
 *  This is useful stopping execution until the CHESS debugger can connect
 *  to the DSP and take control.
 *
 ******************************************************************************/
inline assembly void
EtaCspDspDebugHalt(void)
{
// DB fixme remove REG_DSPCTRL_DSP_DSP_HOLD = 0xAA ;

    //
    // Issue the DSP's breakpoint instruction from inline assembly
    //
    asm_begin
    nop // pipe cleaner
    nop // pipe cleaner
    nop // pipe cleaner
    nop // pipe cleaner
    debug
    nop // pipe cleaner
    nop // pipe cleaner
    nop // pipe cleaner
    nop // pipe cleaner
        asm_end
}

#endif // ETA_CSP_DSP_H

