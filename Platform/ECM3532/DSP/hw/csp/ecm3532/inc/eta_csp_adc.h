/***************************************************************************//**
 *
 * @file eta_csp_adc.h
 *
 * @brief This file contains eta_csp_adc module definitions.
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

#ifndef ETA_CSP_ADC_H
#define ETA_CSP_ADC_H

#include "eta_csp_common_adc.h"

/*******************************************************************************
 *
 * External function definitions.
 *
 ******************************************************************************/
extern tAdcConfigRetVal EtaCspAdcConfig(tAdcConfig sAdcConfig);
extern tAdcConfigRetVal EtaCspAdcConfigUnpack(uint32_t ui32PackedConfig);

#endif // ETA_CSP_ADC_H

