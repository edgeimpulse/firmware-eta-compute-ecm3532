/***************************************************************************//**
 *
 * @file eta_csp_adc.c
 *
 * @brief This file contains eta_csp_adc module implementations.
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
 * @addtogroup ecm3532adc-dsp Analog-to-Digital Converter (ADC)
 * @ingroup ecm3532csp-dsp
 * @{
 ******************************************************************************/

#include "reg.h"
#include "eta_csp_adc.h"
#include "reg_eta_ecm3532_dsp.h"
#include "eta_csp_mailbox.h"
#include "eta_csp_io.h"

/***************************************************************************//**
 *
 *  EtaCspAdcConfig - Initialize the ADC module.
 *
 *  @param sAdcConfig Configuration for ADC
 *
 ******************************************************************************/
tAdcConfigRetVal
EtaCspAdcConfig(tAdcConfig sAdcConfig)
{
    REG_ADC_DSP_CSR = 0;

    REG_ADC_DSP_CSR |=  (sAdcConfig.fields.edge_trig) <<
    BP_ADC_DSP_CSR_EDGE_TRIG;

    if(sAdcConfig.fields.enable)
    {
        REG_ADC_DSP_CSR |=  BM_ADC_DSP_CSR_ADC_EN;
    }
    else
    {
        REG_ADC_DSP_CSR &= ~BM_ADC_DSP_CSR_ADC_EN;
    }

    return(eAdcConfigGood);
}

/***************************************************************************//**
 *
 *  EtaCspAdcConfigUnpack - Initialize the ADC module. Meant to be initialized
 *                          from mailbox command from M3
 *
 *  @param cfg packed version of tAdcConfig
 *
 ******************************************************************************/
tAdcConfigRetVal
EtaCspAdcConfigUnpack(uint32_t ui32PackedConfig)
{
    tAdcConfig sAdcConfig;

    sAdcConfig.packed_config = ui32PackedConfig;
    return(EtaCspAdcConfig(sAdcConfig));
}

