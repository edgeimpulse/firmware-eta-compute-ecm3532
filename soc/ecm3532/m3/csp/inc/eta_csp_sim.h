/***************************************************************************//**
 *
 * @file eta_csp_sim.h
 *
 * @brief This file contains eta_csp_sim module definitions.
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
 * This is part of revision 1.0.0a3 of the Tensai Software Development Kit.
 *
 ******************************************************************************/

/***************************************************************************//**
 * @addtogroup ecm3532sim-m3 SIM
 * @ingroup ecm3532csp-m3
 * @{
 ******************************************************************************/

#ifndef __ETA_CSP_SIM_H__
#define __ETA_CSP_SIM_H__

#include "eta_chip.h"

#ifdef __cplusplus
extern "C" {
#endif

//#define ETA_CSP_SIM_ERROR(ui32Err)     (REG_TB_ERROR.V = ui32Err)
//#define ETA_CSP_SIM_FINISH(ui32Status) (REG_TB_FINISH.V = ui32Status)
//
//#define ETA_CSP_SIM_PROGRESS() (REG_TB_PROGRESS.V =
//                                    (((g_ui32EtaCspSimProgressCnt++) << 16) |
//                                     __LINE__))

//
// The Sim progress count.
//
//extern uint32_t g_ui32EtaCspSimProgressCnt;

#ifdef __cplusplus
}
#endif

#endif // __ETA_CSP_SIM_H__
