/***************************************************************************//**
 *
 * @file eta_csp_dsp.c
 *
 * @brief This file contains eta_csp_dsp module implementations.
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

#include "reg.h"
#include "reg_eta_ecm3532_dsp.h"
#include "eta_csp_mailbox.h"
#include "eta_csp_io.h"

/***************************************************************************//**
 *
 *  EtaCspDspSetupForSleep - Sets up local logic to allow for sleep operations
 *                           where the DSP clock stops and will be restarted
 *                           when a peripheral indicates it has work to do
 *                           for either DSP core or DMA.
 *
 *  Note: This enables all peripherals to wake the system. Generally, all
 *        peripherals  must be enabled to trigger the wake signal in the
 *        peripheral logic (e.g. enable interrupts or source ready, etc), so
 *        enabling all peripherals is reasonably safe barring misconfiguration.
 *
 ******************************************************************************/
void
EtaCspDspSetupForSleep(void)
{
    REG_DSPCTRL_DSP_CLK_CTRL = (0 << BP_DSPCTRL_DSP_CLK_CTRL_HYST_COUNT)     |
                               BM_DSPCTRL_DSP_CLK_CTRL_ALLOW_INT_TO_WAKE     |
                               BM_DSPCTRL_DSP_CLK_CTRL_ALLOW_DMA_TO_WAKE     |
                               BM_DSPCTRL_DSP_CLK_CTRL_ALLOW_ADC_TO_WAKE     |
                               BM_DSPCTRL_DSP_CLK_CTRL_ALLOW_I2S_TO_WAKE     |
                               BM_DSPCTRL_DSP_CLK_CTRL_ALLOW_PDM1_TO_WAKE    |
                               BM_DSPCTRL_DSP_CLK_CTRL_ALLOW_PDM0_TO_WAKE    |
                               BM_DSPCTRL_DSP_CLK_CTRL_ALLOW_I2C_TO_WAKE     |
                               BM_DSPCTRL_DSP_CLK_CTRL_ALLOW_SPI_TO_WAKE     |
                               BM_DSPCTRL_DSP_CLK_CTRL_ALLOW_AHB2DSP_TO_WAKE |
                               BM_DSPCTRL_DSP_CLK_CTRL_KEEP_CLK_RUNNING;
}

/***************************************************************************//**
 *
 *  EtaCspDspGoToSleep - Places the DSP Core in sleep, and keeps it there until
 *                       ready.
 *
 ******************************************************************************/
void
EtaCspDspGoToSleep(void)
{
    // This clears the Keep Clock Running bit
    REG_DSPCTRL_DSP_CLK_CTRL_CLR = BM_DSPCTRL_DSP_CLK_CTRL_KEEP_CLK_RUNNING;

    // When the DMA wakes the DSP Complex, the DSP Core can execute a few
    // cycles.
    // This while loop ensures the DSP Core doesn't wonder through the code
    // until someone intended it to by enabling the clock.
    while((REG_DSPCTRL_DSP_CLK_CTRL &
           BM_DSPCTRL_DSP_CLK_CTRL_KEEP_CLK_RUNNING) == 0)
    {
    }
}

/***************************************************************************//**
 *
 *  EtaCspDspWakeFromSleep - Places the DSP Core in fully running, non sleep
 *                           mode.
 *
 ******************************************************************************/
void
EtaCspDspWakeFromSleep(void)
{
    REG_DSPCTRL_DSP_CLK_CTRL_SET = BM_DSPCTRL_DSP_CLK_CTRL_KEEP_CLK_RUNNING;
}

