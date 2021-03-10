/***************************************************************************//**
 *
 * @file eta_csp_hfo.h
 *
 * @brief This file contains eta_csp_hfo module definitions.
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
 * This is part of revision 1.0.0b1 of the Tensai Chip Support Package.
 *
 ******************************************************************************/

/***************************************************************************//**
 * @addtogroup ecm3532hfo-m3 High Frequency Oscillator (HFO)
 * @ingroup ecm3532csp-m3
 * @{
 ******************************************************************************/

#ifndef __ETA_CSP_HFO_H__
#define __ETA_CSP_HFO_H__

#include <stdint.h>
#include "eta_status.h"
#include "eta_chip.h"

#ifdef __cplusplus
extern "C" {
#endif

//
// Start the HFO.
//
extern void EtaCspHfoStart(void);

//
// Stop the HFO.
//
extern void EtaCspHfoStop(void);

//
// Tuning algorithm to be used periodically for checking the tuning.  Uses fine
// tuning only.
//
extern uint32_t EtaCspHfoTuneCheck(uint32_t ui32Window, uint32_t ui32NumSamples,
                                   uint32_t ui32Timeout);

//
// Tuning algorithm used for cold start (POR or RESET_N).
//
extern uint32_t EtaCspHfoTuneInit(uint32_t ui32CoarseWindow,
                                  uint32_t ui32FineWindow,
                                  uint32_t ui32CoarseSamples,
                                  uint32_t ui32FineSamples,
                                  uint32_t ui32CoarseTimeout,
                                  uint32_t ui32FineTimeout);

//
// Tuning algorithm used for warm starts. Only uses fine tuning.
//
extern uint32_t EtaCspHfoTuneUpdate(uint32_t ui32Window,
                                    uint32_t ui32NumSamples,
                                    uint32_t ui32Timeout);

//
// Tune based on the defaults.
//
extern uint32_t EtaCspHfoTuneDefaultInit(void);

//
// Update the tune.
//
extern uint32_t EtaCspHfoTuneDefaultUpdate(void);

#ifdef __cplusplus
}
#endif

#endif // __ETA_CSP_HFO_H__

