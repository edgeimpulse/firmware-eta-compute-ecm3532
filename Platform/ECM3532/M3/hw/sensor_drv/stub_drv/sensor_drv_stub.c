/*******************************************************************************
 *
 * @file sensor_drv_stub.c
 *
 * @brief Sensor Driver Stub
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
 ******************************************************************************/
#include "sensor_dev.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "errno.h"

#define STUB_DRV_NAME "stub_drv"

typedef enum {
    DrvNoInit = 0,
    DrvInit,
    DrvOpen,
}tDrvState;

static uint8_t DrvUseCnt;
SemaphoreHandle_t stub_lock = NULL;

typedef  struct
{
    tDrvState DrvSt;
	int32_t (*DevRead)(uint8_t bus, uint8_t reg_addr,
			 uint8_t *value, uint8_t len);
	int32_t (*DevWrite)(uint8_t bus, uint8_t reg_addr,
			  uint8_t *value, uint8_t len);
}tStubDrv;


int32_t StubDevRead (uint8_t bus, uint8_t reg_addr,
                    uint8_t *value, uint8_t len)
{
    uint8_t cnt;
    if (value)
    {
        for(cnt = 0; cnt < len; cnt++)
            value[cnt] = cnt;

    }
    return 0;
}

int32_t StubDevWrite (uint8_t bus, uint8_t reg_addr,
                    uint8_t *value, uint8_t len)
{
    return 0;
}


tStubDrv sStubDrv = {
    .DrvSt = DrvNoInit,
    .DevRead = StubDevRead,
    .DevWrite = StubDevWrite,
};

static tDrvHandle StubDrvOpen(char *string)
{
    tDrvHandle handle;
    xSemaphoreTake(stub_lock, portMAX_DELAY);
    handle = ++DrvUseCnt;
    xSemaphoreGive(stub_lock);
    return handle;
}

static int32_t StubDrvProbe()
{
    int32_t ret = -1;
    uint8_t buf;
    xSemaphoreTake(stub_lock, portMAX_DELAY);
    sStubDrv.DevRead(0, 0x1, &buf, 1);
    if (buf == 0)
        ret = 0;
    xSemaphoreGive(stub_lock);
    return ret;
}

static int32_t StubDrvClose(tDrvHandle sDrvHandle)
{
    int32_t ret = 0;
    xSemaphoreTake(stub_lock, portMAX_DELAY);
    if(DrvUseCnt)
    {
        DrvUseCnt--;
    }
    else
    {
        ret = -1;
    }

    xSemaphoreGive(stub_lock);
    return ret;
}

static int32_t StubDrvRead(tDrvHandle sDrvHandle, tSensorType iSensor, uint8_t *ui8Buf, uint8_t ui8Len)
{
    static uint8_t ui32Cnt;
    uint8_t i;
    if(!DrvUseCnt)
        return -EIO;
    for (i = 0; i < ui8Len; i++)
        ui8Buf[i] = ui32Cnt++;
    return 0;
}

static int32_t StubDrvIoctl(tSDrvHandle sDrvHandle, tSensorType iSensor,
                                tSensorIOCTL iCmd, void *vPtr)
{

    if(!DrvUseCnt)
        return -EIO;
    switch (iCmd)
    {
        case IOCTL_SDEV_GET_SUPPORTED_SENSOR_TYPES:
        {
            tSensorTypeInfo *sInfo = (tSensorTypeInfo *)vPtr;
            sInfo->ui8Cnt = 2;
            sInfo->iType[0]= SENSOR_TYPE_GYROSCOPE;
            sInfo->iType[1]= SENSOR_TYPE_ACCELEROMETER;
            break;
        }
        default:
            break;
    }

    return 0;
}


static int32_t StubDrvSetEvCb(tDrvHandle sDrvHandle, tSensorType iSensor,
                        tSensorEventCb fEvCb, tSensorEvent iEvMask,
                        void *vCbData)
{
    if(!DrvUseCnt)
        return -EIO;
    return 0;
}

tSensorDrvOps sStubDevOps = {
    .DrvProbe = StubDrvProbe,
	.DrvOpen = StubDrvOpen,
    .DrvClose = StubDrvClose,
    .DrvRead = StubDrvRead,
    .DrvIoctl =  StubDrvIoctl,
    .DrvSetCb = StubDrvSetEvCb,
};

void SensorStubDriverInit(void)
{
    stub_lock = xSemaphoreCreateMutex();
    SensorDrvRegister(STUB_DRV_NAME, &sStubDevOps);
}


void SensorStubDriverDeInit(void)
{
    SensorDrvDeRegister(STUB_DRV_NAME);
    vSemaphoreDelete(stub_lock);
}
