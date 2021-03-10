/***************************************************************************//**
 *
 * @file eta_csp_m3.h
 *
 * @brief This file contains eta_csp_m3 module definitions.
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
 * @addtogroup ecm3532m3-m3 M3
 * @ingroup ecm3532csp-m3
 * @{
 ******************************************************************************/

#ifndef __ETA_CSP_M3_H__
#define __ETA_CSP_M3_H__

#include <stdint.h>

extern void EtaCspM3IntDisable(void);
extern void EtaCspM3IntEnable(void);
extern void EtaCspM3IntBaseLevel(uint8_t);
extern void EtaCspM3WFI(void);

#endif // __ETA_CSP_M3_H__

