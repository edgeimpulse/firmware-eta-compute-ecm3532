/***************************************************************************//**
 *
 * @file eta_csp_pdm.h
 *
 * @brief This file contains eta_csp_pdm module definitions.
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

#ifndef __ETA_CSP_PDM_H__
#define __ETA_CSP_PDM_H__

#include <stdint.h>
#include "eta_status.h"
#include "eta_chip.h"
#include "eta_csp_common_pdm.h"

// Uncomment this to get detailed print of the structure (for debug)
//#define ETA_CSP_PDM_DEBUG_PRINT

#ifdef __cplusplus
extern "C" {
#endif

//
// This fence will compile inline and do only one byte reads from the RTC block.
//
#define EtaCspPdmFenceFast() \
                             {REG_GPIO_DEBUG0_READ_ONLY.V = MEMIO8(REG_PDM_PCMA_CTRL_ADDR(1));}

//
// Add a fence on the last bus operation to SOC Ctrl registers.
//
extern void EtaCspPdmFence(void);


//
// Initialize the PDM.
//
/***************************************************************************//**
 *
 *  EtaCspPdmConfig - Initialize the PDM module.
 *
 *  @param cfg encoded configuration value.
 *
 ******************************************************************************/
tPdmConfigRetVal EtaCspPdmConfig(tPdmConfig pdmConfig);


//
// Initialize the PDM.
//
/***************************************************************************//**
 *
 *  EtaCspPdmConfigUnpack - Initialize the PDM module. Meant to be initialized
 *                          from mailbox command from DSP
 *
 *  @param cfg packed version of tPdmConfig
 *
 ******************************************************************************/
tPdmConfigRetVal EtaCspPdmConfigUnpack(uint32_t packed_config);

#ifdef __cplusplus
}
#endif

#endif // __ETA_CSP_PDM_H__

