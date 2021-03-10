/***************************************************************************//**
 *
 * @file eta_bsp_button.h
 *
 * @brief This file contains button module definitions.
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
 * This is part of revision 1.0.0a6 of the Tensai Software Development Kit.
 *
 ******************************************************************************/

/***************************************************************************//**
 * @addtogroup ecm3532button-bsp Button
 * @ingroup ecm3532aisensor-bsp
 * @{
 ******************************************************************************/

#ifndef __ETA_BSP_BUTTON_H__
#define __ETA_BSP_BUTTON_H__

#include <stdint.h>
#include <stdbool.h>
#include "eta_bsp.h"

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 *
 * External function definitions
 *
 ******************************************************************************/
extern void EtaBspButtonInit(void);
extern bool EtaBspButtonIsPressed(tGpioBit bit);
extern uint32_t EtaBspButtonWaitPressRelease(tGpioBit bit);

#ifdef __cplusplus
}
#endif

#endif // __ETA_BSP_BUTTON_H__

