/***************************************************************************//**
 *
 * @file a31r118.h
 *
 * @brief This file contains a31r118 ble uart module definitions.
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
#include "config.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>

#define INIT_RSP_STR "+INIT\r"
#define CONN_RSP_STR "+CONE:"
#define DISC_RSP_STR "+DISCE:"
#define MTU_RSP_STR  "+MTUE:"

#define BLE_AT_RESET "AT+RESET\r"

#define RSP_UKWN    0
#define RSP_LAST_SEND_CMD   1
#define RSP_LIMITED_MATCH   2
#define RSP_INIT    3
#define RSP_CONN    4
#define RSP_DISC    5
#define RSP_MTU     6

void bleEnable(void);
void bleReset(void);
void bleReadBdAddr(void);
void bleSendString(char* data);
void bleInit(void);
uint8_t checkForResponse(uint8_t* rxData);