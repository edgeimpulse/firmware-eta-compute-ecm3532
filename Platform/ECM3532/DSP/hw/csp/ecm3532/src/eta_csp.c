/***************************************************************************//**
 *
 * @file eta_csp.c
 *
 * @brief This file contains eta_csp module implementations.
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
 * @ingroup ecm3532csp-dsp
 * @{
 ******************************************************************************/

#include "reg.h"
#include "reg_eta_ecm3532_dsp.h"
#include "eta_csp_mailbox.h"
#include "eta_csp_io.h"
#include "eta_csp.h"


  #define ETA_CSP_WELL_KNOWN (0x1003FFE0)
  volatile int chess_storage(IOMEM) *g_pEtaCspFinishFlag;


/***************************************************************************//**
 *
 * EtaCspFinish - Notify M3 that we are finished for either DSP core or DMA.
 *
 * Note: This writes the provided return code to the lower 16-bits and 0x1
 * to upper 16-bits to the address M3 looks for a return code from the DSP.
 * A non-zero value at this address notifies the M3 that DSP is done with
 * its task, and lower 16 bits provide the return code; 0 means no errors.
 *
 * @param exit_value  - Integer return code from the DSP.
 *
 ******************************************************************************/
void
EtaCspFinish(int exit_value)
{
    g_pEtaCspFinishFlag =
        (volatile int chess_storage(IOMEM) *)((ETA_CSP_WELL_KNOWN & 0x3FFF) /
                                              2);
    REG_DSPCTRL_DSP_WIN_ADDR0_UPPER = (ETA_CSP_WELL_KNOWN >> 16) & 0xFFFF;
    REG_DSPCTRL_DSP_WIN_ADDR0_LOWER = (ETA_CSP_WELL_KNOWN >> 0)  & 0xC000;
    g_pEtaCspFinishFlag[0] = exit_value;
    g_pEtaCspFinishFlag[1] = 0x1;
}

/***************************************************************************//**
 ******************************************************************************/
uint_fast8_t
EtaCspAhbWindowDontWaitGet()
{
    return((REG_DSPCTRL_DSP_CONFIG_STATUS        |
            BM_DSPCTRL_DSP_CONFIG_STATUS_DONT_WAIT) >>
           (BP_DSPCTRL_DSP_CONFIG_STATUS_DONT_WAIT));
}

/***************************************************************************//**
 ******************************************************************************/
void
EtaCspAhbWindowDontWaitSet(uint_fast8_t ui8Enable)
{
    if(ui8Enable)
    {
        REG_DSPCTRL_DSP_CONFIG_STATUS |= BM_DSPCTRL_DSP_CONFIG_STATUS_DONT_WAIT;
    }
    else
    {
        REG_DSPCTRL_DSP_CONFIG_STATUS &= ~BM_DSPCTRL_DSP_CONFIG_STATUS_DONT_WAIT;
    }
}

/***************************************************************************//**
 *
 * EtaCspAhbWindowCacheLineDisable - Disable 32-bit cache line in AHB Window.
 *
 * Note: There is a 32-bit cache line in AHB window that will store 32-bit
 * AHB read data. When enabled (default) 2 16-bit reads from consecutive
 * addresses will only cause one AHB access. If disabled, every 16-bit read
 * will cause a 32-bit AHB access.
 *
 ******************************************************************************/
void
EtaCspAhbWindowCacheLineDisable()
{
    REG_DSPCTRL_DSP_CONFIG_STATUS |= BM_DSPCTRL_DSP_CONFIG_STATUS_CACHE_DISABLE;
}

/***************************************************************************//**
 *
 * EtaCspAhbWindowCacheLineEnable - Enable 32-bit cache line in AHB Window
 * (Default).
 *
 * Note: There is a 32-bit cache line in AHB window that will store 32-bit
 * AHB read data. When enabled (default) 2 16-bit reads from consecutive
 * addresses will only cause one AHB access. If disabled, every 16-bit read
 * will cause a 32-bit AHB access.
 *
 ******************************************************************************/
void
EtaCspAhbWindowCacheLineEnable()
{
    REG_DSPCTRL_DSP_CONFIG_STATUS &= ~BM_DSPCTRL_DSP_CONFIG_STATUS_CACHE_DISABLE;
}

/***************************************************************************//**
 *
 * EtaCspCtrlSpiClkSet - Set the SPI master source clock to always on or
 * auto mode
 *
 * @param clkCtrl - 1: clock is always on, 0 (default): clock gating enabled.
 *
 * Note: Setting clock to always on mode increase the power, but will have
 * a faster response time. Auto mode is lower power, but requires some
 * spin up time for every DMA or DSP Core access (typically ~500-625ns).
 *
 ******************************************************************************/
void
EtaCspCtrlSpiClkSet(tClkCtrl clkCtrl)
{
    if (clkCtrl==eClkCtrlAlwaysOn)
    {
        REG_DSPCTRL_DSP_CONFIG_STATUS |= BM_DSPCTRL_DSP_CONFIG_STATUS_DSP_SPI_CLK_ON;
    }
    else // eClkCtrlAuto
    {
        REG_DSPCTRL_DSP_CONFIG_STATUS &= ~BM_DSPCTRL_DSP_CONFIG_STATUS_DSP_SPI_CLK_ON;
    }
}

/***************************************************************************//**
 *
 * EtaCspCtrlI2cClkSet - Set the I2C master source clock to always on or
 * auto mode
 *
 * @param clkCtrl - 1: clock is always on, 0 (default): clock gating enabled.
 *
 * Note: Setting clock to always on mode increase the power, but will have
 * a faster response time. Auto mode is lower power, but requires some
 * spin up time for every DMA or DSP Core access (typically ~500-625ns).
 *
 ******************************************************************************/
void
EtaCspCtrlI2cClkSet(tClkCtrl clkCtrl)
{
    if (clkCtrl==eClkCtrlAlwaysOn)
    {
        REG_DSPCTRL_DSP_CONFIG_STATUS |= BM_DSPCTRL_DSP_CONFIG_STATUS_DSP_I2C_CLK_ON;
    }
    else // eClkCtrlAuto
    {
        REG_DSPCTRL_DSP_CONFIG_STATUS &= ~BM_DSPCTRL_DSP_CONFIG_STATUS_DSP_I2C_CLK_ON;
    }
}

