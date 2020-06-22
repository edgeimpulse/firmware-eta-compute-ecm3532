/***************************************************************************//**
 *
 * @file eta_csp_vhfoo.h
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
 * @addtogroup ecm3532vhfo-m3 Very High Frequency Oscillator (VHFO)
 * @ingroup ecm3532csp-m3
 * @{
 ******************************************************************************/

#ifndef __ETA_CSP_VHFO_H__
#define __ETA_CSP_VHFO_H__

#ifdef __cplusplus
extern "C" {
#endif

//
// Useful defines for testing
//
#define ETA_CSP_VHFO_TRIM_CL_TOP (11)
#define ETA_CSP_VHFO_TRIM_CL_BOT (4)
#define ETA_CSP_VHFO_TRIM_GM_TOP (3)
#define ETA_CSP_VHFO_TRIM_GM_BOT (1)

//
// Initialize the VHFO.
//
extern tEtaStatus EtaCspVhfoInit(void);

//
// Turn off the VHFO.
//
extern void EtaCspVhfoOff(void);

//
// Set the VHFO capacitive trim (TrimCl)
//
extern void EtaCspVhfoTrimClSet(uint32_t);

//
// Get the VHFO capacitive trim (TrimCl)
//
extern uint32_t EtaCspVhfoTrimClGet(void);

//
// Set the VHFO gain trim (TrimGm)
//
extern void EtaCspVhfoTrimGmSet(uint32_t);

//
// Get the VHFO gain trim (TrimGm)
//
extern uint32_t EtaCspVhfoTrimGmGet(void);

//
// Record the VHFO trim settings into the flash page.
//
extern tEtaStatus EtaCspVhfoTrimRecord(void);

//
// Get the current 32-bit VHFO trim register value.
//
extern uint32_t EtaCspVhfoTrimGet(void);

#ifdef __cplusplus
}
#endif

#endif // __ETA_CSP_VHFO_H__

