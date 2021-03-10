/***************************************************************************//**
 *
 * @file eta_csp_pdm.h
 *
 * @brief This file contains eta_csp_pdm module definitions.
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
 * @addtogroup ecm3532pdm-dsp Pulse Density Modulation (PDM)
 * @ingroup ecm3532csp-dsp
 * @{
 ******************************************************************************/

#ifndef __ETA_CSP_PDM_H__
#define __ETA_CSP_PDM_H__

#include "eta_csp_common_pdm.h"

#ifdef __cplusplus
extern "C" {
#endif


/*******************************************************************************
 *
 * External function definitions.
 *
 ******************************************************************************/

extern tPdmConfigRetVal EtaCspPdmConfig(tPdmConfig sPdmConfig);

extern uint_fast8_t EtaCspPdmRxOfFlagRd(uint16_t ui16Instance);

extern uint_fast8_t EtaCspPdmRxUfFlagRd(uint16_t ui16Instance);

#ifdef __cplusplus
}
#endif

#endif // __ETA_CSP_PDM_H__

