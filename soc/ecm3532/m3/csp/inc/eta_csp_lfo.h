/***************************************************************************//**
 *
 * @file eta_csp_lfo.h
 *
 * @brief This file contains eta_csp_vhfo module definitions.
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
 * @addtogroup ecm3532vhfo-m3 Very High Frequency Oscillator (LFO)
 * @ingroup ecm3532csp-m3
 * @{
 ******************************************************************************/

#ifndef __ETA_CSP_LFO_H__
#define __ETA_CSP_LFO_H__

#ifdef __cplusplus
extern "C" {
#endif

//
// Useful defines for testing
//
#define ETA_CSP_LFO_TRIM_CL_TOP (11)
#define ETA_CSP_LFO_TRIM_CL_BOT (4)
#define ETA_CSP_LFO_TRIM_GM_TOP (3)
#define ETA_CSP_LFO_TRIM_GM_BOT (1)

//
// Initialize the LFO.
//
extern tEtaStatus EtaCspLfoInit(void);



// EtaCspLfo36nATrimSet - Set the LFO 36nA current mode trim.
extern void EtaCspLfo36nATrimSet(uint32_t);

//
// Get the LFO 36na trim (LFO_36na_Trim)
//
extern uint32_t EtaCspLfo36nATrimGet(void);

//
// Set the LFO gain trim (LfoTrim)
//
extern void EtaCspLfoTrimSet(uint32_t);

//
// Get the current 32-bit LFO trim register value.
//
extern uint32_t EtaCspLfoTrimGet(void);

//
// Record the LFO trim settings into the flash page.
//
extern tEtaStatus EtaCspLfoTrimRecord(void);

#ifdef __cplusplus
}
#endif

#endif // __ETA_CSP_LFO_H__

