/***************************************************************************//**
 *
 * @file eta_csp_pdm.c
 *
 * @brief This file contains eta_csp_pdm module implementations.
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
 * @addtogroup ecm3532pdm-m3 Pulse-density Modulation (PDM)
 * @ingroup ecm3532csp-m3
 * @{
 ******************************************************************************/

#include <stdint.h>
#include "eta_status.h"
#include "eta_chip.h"
#include "eta_csp_pdm.h"
#include "eta_csp_io.h"
#include "eta_csp_mailbox_cmd.h"

/***************************************************************************//**
 *
 *  EtaCspPdmConfig - Initialize the PDM module. Meant to be initialized either
 *                    from mailbox command from DSP or locally.
 *
 *  @param cfg encoded configuration value.
 *
 ******************************************************************************/
tPdmConfigRetVal
EtaCspPdmConfig(tPdmConfig pdmConfig)
{
    uint32_t tempdata0;
    uint8_t instance;

#ifdef ETA_CSP_PDM_DEBUG_PRINT
    etaPrintf("M3:  packed_config:%x\r\n", pdmConfig.packed_config);
    etaPrintf("M3:  enable       :%x\r\n", pdmConfig.fields.enable);
    etaPrintf("M3:  swap_chan    :%x\r\n", pdmConfig.fields.swap_chan);
    etaPrintf("M3:  edge_trig    :%x\r\n", pdmConfig.fields.edge_trig);
    etaPrintf("M3:  clock_src    :%x\r\n", pdmConfig.fields.clock_src);
    etaPrintf("M3:  sample_freq  :%x\r\n", pdmConfig.fields.sample_freq);
    etaPrintf("M3:  chan_config  :%x\r\n", pdmConfig.fields.chan_config);
    etaPrintf("M3:  port_select  :%x\r\n", pdmConfig.fields.port_select);
#endif   // ETA_CSP_PDM_DEBUG_PRINT

    ///////////////////////////////////////////////
    // Setup Pad Mux
    if(pdmConfig.fields.enable && (pdmConfig.fields.port_select == ePdmPort0))
    {
#ifdef ETA_CSP_PDM_DEBUG_PRINT
        etaPrintf("INFO: Configuring GPIO for PDM0 on GPIO 17,18\r\n");
#endif     // ETA_CSP_PDM_DEBUG_PRINT

        instance = 0;
        REG_M2(SOCCTRL_PMUXC, GPIO17, BFV_SOCCTRL_PMUXC_GPIO17_PDM0CLK,
               GPIO18, BFV_SOCCTRL_PMUXC_GPIO18_PDM0DATA);
    }
    else if(pdmConfig.fields.enable &&
            (pdmConfig.fields.port_select == ePdmPort0Alt))
    {
#ifdef ETA_CSP_PDM_DEBUG_PRINT
        etaPrintf("INFO: Configuring GPIO for PDM0 on GPIO 8,9\r\n");
#endif     // ETA_CSP_PDM_DEBUG_PRINT

        instance = 0;
        REG_M2(SOCCTRL_PMUXB, GPIO8, BFV_SOCCTRL_PMUXB_GPIO8_PDM0CLK,
               GPIO9, BFV_SOCCTRL_PMUXB_GPIO9_PDM0DATA);
    }
    else if(pdmConfig.fields.enable &&
            (pdmConfig.fields.port_select == ePdmPort1))
    {
#ifdef ETA_CSP_PDM_DEBUG_PRINT
        etaPrintf("INFO: Configuring GPIO for PDM1 on GPIO 14,15\r\n");
#endif     // ETA_CSP_PDM_DEBUG_PRINT

        instance = 1;
        REG_M2(SOCCTRL_PMUXB, GPIO14, BFV_SOCCTRL_PMUXB_GPIO14_PDM1CLK,
               GPIO15, BFV_SOCCTRL_PMUXB_GPIO15_PDM1DATA);
    }
    else if(pdmConfig.fields.enable)
    {
        instance = 0; // DB FS-41    fixme VW must check
        return(ePdmConfigInvalidConfig);
    }
    else
    {
        instance = 0; // DB FS-41    fixme VW must check
    }

    // Fence
    REG_GPIO_DEBUG0_READ_ONLY.V = REG_SOCCTRL_PMUXB.V;

    // Setup Pad Mux
    ///////////////////////////////////////////////

    ///////////////////////////////////////////////
    // Setup Clock Source
    switch(pdmConfig.fields.clock_src)
    {
        case (ePdmClkSrc2MhzXtal):
        {
            // Power up the 16MHz oscillator
            REG_M1(RTC_OSC_CSR2, VHFO_PD_N, 1);

            // Fence
            REG_GPIO_DEBUG0_READ_ONLY.V = REG_SOCCTRL_PMUXB.V;
            REG_M1(SOCCTRL_CLK_CFG, ADC_CLK_SEL,
                   BFV_SOCCTRL_CLK_CFG_ADC_CLK_SEL_2MHZ_XTAL);
            break;
        }

        case (ePdmClkSrc2MhzHfo):
        {
            REG_M1(SOCCTRL_CLK_CFG, ADC_CLK_SEL,
                   BFV_SOCCTRL_CLK_CFG_ADC_CLK_SEL_2MHZ_HFO);
            break;
        }

        case (ePdmClkSrc1MhzHfo):
        {
            REG_M1(SOCCTRL_CLK_CFG, ADC_CLK_SEL,
                   BFV_SOCCTRL_CLK_CFG_ADC_CLK_SEL_1MHZ_HFO);
            break;
        }

        case (ePdmClkSrcFromPad):
        {
            // Enable ACLK as input
            tempdata0 = REG_SOCCTRL_PMUXB.V;
            tempdata0 &= ~(BM_SOCCTRL_PMUXB_GPIO12);
            tempdata0 |=
                (BFV_SOCCTRL_PMUXB_GPIO12_I2S_ACLK << BP_SOCCTRL_PMUXB_GPIO12);
            REG_SOCCTRL_PMUXB.V = tempdata0;

            // Fence
            REG_GPIO_DEBUG0_READ_ONLY.V = REG_SOCCTRL_PMUXB.V;

            // Select ACLK as clock source
            REG_M1(SOCCTRL_CLK_CFG, ADC_CLK_SEL,
                   BFV_SOCCTRL_CLK_CFG_ADC_CLK_SEL_2MHZ_PAD);
            break;
        }

        default:
        {
            return(ePdmConfigInvalidConfig);
            break;
        }
    }

    // Setup Clock Source
    ///////////////////////////////////////////////

    ///////////////////////////////////////////////
    // Configure Actual PDM

    // Common config
    REGN_M1(instance, PDM_PDM_CORE_CONF, PGA_R, 0xa);
    REGN_M1(instance, PDM_PDM_CORE_CONF, PGA_L, 0xa);
    REGN_M1(instance, PDM_PDM_CORE_CONF, SINC_RATE, 0x40);
    REGN_M1(instance, PDM_PDM_CORE_CONF, MCLKDIV, 0x0);
    REGN_M1(instance, PDM_PDM_CORE2_CONF, PDMCKO_DLY, 0x3);

    if(pdmConfig.fields.sample_freq == ePdmSampleFreq16Ksps)
    {
#ifdef ETA_CSP_PDM_DEBUG_PRINT
        etaPrintf("INFO: Configuring For 16Ksamples/sec (2MHz CKO)\r\n");
#endif     // ETA_CSP_PDM_DEBUG_PRINT
        REGN_M1(instance, PDM_PDM_CORE2_CONF, DIV_MCLKQ,
                BFV_PDM_PDM_CORE2_CONF_DIV_MCLKQ_BYPASS);
    }
    else if(pdmConfig.fields.sample_freq == ePdmSampleFreq8Ksps)
    {
#ifdef ETA_CSP_PDM_DEBUG_PRINT
        etaPrintf("INFO: Configuring For 8Ksamples/sec (1MHz CKO)\r\n");
#endif     // ETA_CSP_PDM_DEBUG_PRINT
        REGN_M1(instance, PDM_PDM_CORE2_CONF, DIV_MCLKQ,
                BFV_PDM_PDM_CORE2_CONF_DIV_MCLKQ_DIV_BY_1);
    }
    else
    {
        return(ePdmConfigInvalidConfig);
    }

    // Fence
    REG_PDM_DEBUG0(instance).V;

    // Actually enable PDM
    REGN_M1(instance, PDM_PCMA_CTRL, EN, pdmConfig.fields.enable);

    // REGN_M1(instance,PDM_PCMA_CTRL,EN,BFV_PDM_PCMA_CTRL_EN_ENABLE);

    // Configure Actual PDM
    ///////////////////////////////////////////////

    return(ePdmConfigGood);
}

/***************************************************************************//**
 *
 *  EtaCspPdmConfigUnpack - Initialize the PDM module. Meant to be initialized
 *                          from mailbox command from DSP
 *
 *  @param cfg packed version of tPdmConfig
 *
 ******************************************************************************/
tPdmConfigRetVal
EtaCspPdmConfigUnpack(uint32_t packed_config)
{
    tPdmConfig pdmConfig;

    pdmConfig.packed_config = packed_config;

    return(EtaCspPdmConfig(pdmConfig));
}

/***************************************************************************//**
 *
 * EtaCspPdmFence - Set up a fence for writes to the PDM registers.
 *
 *
 ******************************************************************************/
void
EtaCspPdmFence(void)
{
    EtaCspPdmFenceFast();
}

