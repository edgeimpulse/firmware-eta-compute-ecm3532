/***************************************************************************//**
 *
 * @file eta_devices_a31r118.c
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

#include "eta_devices_a31r118.h"

tUart *g_psBleUart;

/***************************************************************************//**
 *
 * EtaDevicesA31R118Init - Initialize the UART to the BLE.
 *
 * @param psUart Pointer to the UART structure to use.
 *
 ******************************************************************************/
void
EtaDevicesA31R118Init(tUart *psUart)
{
    //
    // Save the UART.
    //
    g_psBleUart = psUart;
}

/***************************************************************************//**
 *
 * EtaDevicesA31R118StringSend - Send a string over BLE.
 *
 * @param pcStr Pointer to the sting to send.
 *
 ******************************************************************************/
void
EtaDevicesA31R118StringSend(char *pcStr)
{
    //
    // Send the string.
    //
    EtaCspUartPuts(g_psBleUart, pcStr);
}

