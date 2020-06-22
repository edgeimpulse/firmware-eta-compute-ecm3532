/***************************************************************************//**
 *
 * @file eta_devices_a31r118.h
 *
 * @brief TODO
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
 * @addtogroup etadevices-a31r118 ABOV A31R118 BLE
 * @ingroup etadevices
 * @{
 ******************************************************************************/

#ifndef __ETA_DEVICES_A31R118__
#define __ETA_DEVICES_A31R118__

#include <string.h>
#include "eta_csp_uart.h"

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 *
 * External function definitions.
 *
 ******************************************************************************/
extern void EtaDevicesA31R118Init(tUart *psUart);
extern void EtaDevicesA31R118StringSend(char *pcStr);

#ifdef __cplusplus
}
#endif

#endif // __ETA_DEVICES_A31R118__

