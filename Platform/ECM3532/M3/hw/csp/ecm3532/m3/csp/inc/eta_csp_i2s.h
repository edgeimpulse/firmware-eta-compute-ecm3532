/***************************************************************************//**
 *
 * @file eta_csp_i2s.h
 *
 * @brief This file contains eta_csp_i2s module definitions.
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
 ******************************************************************************/

/***************************************************************************//**
 * @addtogroup ecm3532i2s-m3 Inter-IC Sound (I2S)
 * @ingroup ecm3532csp-m3
 * @{
 ******************************************************************************/

#ifndef __ETA_CSP_I2S_H__
#define __ETA_CSP_I2S_H__

#include <stdint.h>
#include "eta_status.h"
#include "eta_chip.h"
#include "eta_csp_common_i2s.h"

// Uncomment this to get detailed print of the structure (for debug)
//#define ETA_CSP_I2S_DEBUG_PRINT

#ifdef __cplusplus
extern "C" {
#endif

//
// This fence will compile inline and do only one byte reads from the I2S block.
//
#define EtaCspI2sFenceFast() \
                             {REG_GPIO_DEBUG0_READ_ONLY.V = MEMIO8(REG_I2S_CSR_ADDR);}

//
// Add a fence on the last bus operation to SOC Ctrl registers.
//
extern void EtaCspI2sFence(void);

//
// Initialize the I2S.
//
/***************************************************************************//**
 *
 *  EtaCspI2sConfig - Initialize the I2S module.
 *
 *  @param cfg encoded configuration value.
 *
 ******************************************************************************/
tI2sConfigRetVal EtaCspI2sConfig(tI2sConfig i2sConfig);


//
// Initialize the I2S.
//
/***************************************************************************//**
 *
 *  EtaCspI2sConfigUnpack - Initialize the I2S module. Meant to be initialized
 *                          from mailbox command from DSP
 *
 *  @param cfg packed version of tI2sConfig
 *
 ******************************************************************************/
tI2sConfigRetVal EtaCspI2sConfigUnpack(uint32_t packed_config);

#ifdef __cplusplus
}
#endif

#endif // __ETA_CSP_I2S_H__

