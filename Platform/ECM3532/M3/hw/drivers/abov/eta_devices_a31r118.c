/***************************************************************************//**
 *
 * @file eta_devices_a31r118.c
 *
 * @brief Driver for A31R118 BLE uart module
 *
 * Copyright (C) 2019 Eta Compute, Inc
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
 * This is part of revision ${version} of the Tensai Software Development Kit.
 *
 ******************************************************************************/
#include "eta_devices_a31r118.h"
#include "gpio_hal.h"
#include "uart_hal.h"
#include "string.h"

static char lastSendString[128];

void bleEnable(void)
{
    HalGpioOutInit(CONFIG_BLE_RST_PIN, 0);
    HalGpioWrite(CONFIG_BLE_RST_PIN,0);
    HalGpioWrite(CONFIG_BLE_RST_PIN,1);
}

void bleStoreLastSendString(char* data)
{
    strncpy(lastSendString,data,sizeof(lastSendString) - 1);
}

uint8_t bleCompareLastSendString(char* data)
{
    uint8_t status = RSP_UKWN;

    uint8_t len = strlen(lastSendString);

    if(len == 0)
    {
        return RSP_UKWN;
    }

    if(lastSendString[len - 1] == '\n')
    {
        lastSendString[len - 1] = '\0';
    }
    if(strlen(data) < len)
    {
        if (strcmp(lastSendString,data) == 0)
        {
            lastSendString[0] = '\0';
            status = RSP_LAST_SEND_CMD;
        }
        else
        {
            status = RSP_UKWN;
        }
    }
    else
    {
        if(strncmp(lastSendString,data,len) == 0)
        {
            lastSendString[0] = '\0';
            status = RSP_LIMITED_MATCH;
        }
        else
        {
            status = RSP_UKWN;
        }
    }

    return status;
}

void bleReset(void)
{
    char txBuf[sizeof(BLE_AT_RESET)];

    strcpy(txBuf,BLE_AT_RESET);
    bleSendString(txBuf);
}

void bleReadBdAddr(void)
{
    char txBuf[180];
    sprintf(txBuf,"AT+ADDR?\r");
    bleSendString(txBuf);
}

void bleSendString(char* data)
{
    bleStoreLastSendString(data);
    HalUartWriteBuf(CONFIG_BLE_UART, (uint8_t *)data, strlen(data), NULL, NULL);
}

void bleInit(void)
{
    bleReset();
}

uint8_t checkForResponse(uint8_t* rxData)
{
    if(strcmp((char*)rxData,INIT_RSP_STR) == 0)
    {
        return RSP_INIT;
    }
    else if(strncmp((char*)rxData,CONN_RSP_STR,strlen(CONN_RSP_STR)) == 0)
    {
        return RSP_CONN;
    }
    else if(strncmp((char*)rxData,MTU_RSP_STR,strlen(MTU_RSP_STR)) == 0)
    {
        return RSP_MTU;
    }
    else if(strncmp((char*)rxData,DISC_RSP_STR,strlen(DISC_RSP_STR)) == 0)
    {
        return RSP_DISC;
    }
    else
    {
        return bleCompareLastSendString((char*)rxData);
    }
}

#if 0
bool bleWriteAdvName(uint8_t* advName)
{
    uint8_t txBuf[180];

    if(strlen(advName) > 170)
        return 0;

    sprintf(txBuf,"AT+NAME=%s\r",advName);
    bleSendString(txBuf);
    return 1;
}

bool bleWriteBdAddr(uint8_t* bdAddr)
{
    uint8_t txBuf[180];

    if(strlen(bdAddr) > 12)
        return 0;

    sprintf(txBuf,"AT+ADDR=%s\r",bdAddr);
    bleSendString(txBuf);
    return 1;
}
#endif