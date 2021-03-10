/*******************************************************************************
 *
 * @file sensor.c
 *
 * @brief Sensor manager
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
#include "config.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "sensor_dev.h"
#include "errno.h"
#include "helper_func.h"
#include "print_util.h"
#include <stddef.h>

#define SENSOR_MAX_CNT (CONFIG_SENSOR_DEV_COUNT)
#define DRIVER_MAX_CNT (CONFIG_SENSOR_DEV_COUNT * 2)

typedef enum {
    ProbeFail = 0,
    ProbeSuccess = 1,
}tSensorProbeSt;

typedef struct {
    char *name;
    tSensorDrvOps *sDops;
    tSensorProbeSt iProbeSt;
} tSensorDevice;

typedef struct {
    uint8_t ui8SensorDevIndex;
    tDrvHandle vDrvHandle;
} tHandleMap;

SemaphoreHandle_t SmLock = NULL;
tSensorDevice sSensorDevList[SENSOR_MAX_CNT];
tHandleMap sHandleMapList [DRIVER_MAX_CNT];

/**
 * Register sensor driver to SM
 *
 * @param Name Sensor driver name
 * @param sDrvOps Sensor driver ops struture pointer
 *
 * @return 0 on success, negative error code on failure.
 */
int32_t SensorDrvRegister(char *Name, tSensorDrvOps *sDrvOps)
{
    int32_t ret = -EINVAL;
    uint8_t ui8Cnt;

    if (!SmLock)
    {
        SmLock = xSemaphoreCreateMutex();
        for(ui8Cnt = 0; ui8Cnt < DRIVER_MAX_CNT; ui8Cnt++)
        {
            sHandleMapList[ui8Cnt].vDrvHandle = INVALID_HANDLE;
        }
    }

    if((Name && sDrvOps))
    {
        if (xTaskGetSchedulerState() == taskSCHEDULER_RUNNING)
            xSemaphoreTake(SmLock, portMAX_DELAY);
        /* Get Free Entry */
        for(ui8Cnt = 0; ui8Cnt < SENSOR_MAX_CNT; ui8Cnt++)
        {
            if(!sSensorDevList[ui8Cnt].name)
                break;
        }

        if (ui8Cnt < SENSOR_MAX_CNT)
        {
            sSensorDevList[ui8Cnt].name = Name;
            sSensorDevList[ui8Cnt].sDops = sDrvOps;
            ret = 0;
        }

        if (xTaskGetSchedulerState() == taskSCHEDULER_RUNNING)
            xSemaphoreGive(SmLock);
    }

    return ret;
}

/**
 * DeRegister sensor driver to SM
 *
 * @param Name Sensor driver name
 * @param sDrvOps Sensor driver ops struture pointer
 *
 * @return 0 on success, negative error code on failure.
 */
int32_t SensorDrvDeRegister(char *Name)
{
    int ret = 0;
    tSensorDevice *sDev = NULL;
    uint8_t ui8Sidx, ui8Didx = 0;
    tDrvHandle sDrvHandle;

    for(ui8Sidx = 0; ui8Sidx < SENSOR_MAX_CNT; ui8Sidx++)
    {
        if(!strcmp(sSensorDevList[ui8Sidx].name, Name))
        {
            sDev =  &sSensorDevList[ui8Sidx];
            break;
        }
    }
    if (sDev)
    {
        for(ui8Didx = 0; ui8Didx < DRIVER_MAX_CNT; ui8Didx++)
        {
            if(sHandleMapList[ui8Didx].ui8SensorDevIndex == ui8Sidx)
            {
                sDrvHandle = sHandleMapList[ui8Didx].vDrvHandle;
                if (sDrvHandle != INVALID_HANDLE)
                {
                    xSemaphoreTake(SmLock, portMAX_DELAY);
                    ret = sDev->sDops->DrvClose(sDrvHandle);
                    if (ret >= 0 )
                    {
                        sHandleMapList[sDrvHandle].vDrvHandle = INVALID_HANDLE;
                    }
                    else
                    {
                        xSemaphoreGive(SmLock);
                        break;
                    }
                }
                xSemaphoreGive(SmLock);

            }
        }
        if (ret >= 0)
        {
            xSemaphoreTake(SmLock, portMAX_DELAY);
            sSensorDevList[ui8Sidx].name = NULL;
            sSensorDevList[ui8Sidx].sDops = NULL;
            xSemaphoreGive(SmLock);
        }

    }
    return ret;
}

/**
 * Sensor Abtaction layer Init
 */
void SalInit(void)
{
    tSensorDevice *sDev = NULL;
    uint8_t ui8Cnt = 0;
    int32_t ret;

    for(ui8Cnt = 0; ui8Cnt < SENSOR_MAX_CNT; ui8Cnt++)
    {
        sDev =  &sSensorDevList[ui8Cnt];
        if(sDev->name)
        {
            ret = sDev->sDops->DrvProbe();
            if (ret == 0)
                sDev->iProbeSt = ProbeSuccess;
        }
    }
}

/**
 * Register sensor driver to SM
 *
 * @param Name Sensor driver name
 *
 * @return tSensorDevice valid sensor handle on success,
 *          negative error code on failure.
 */
tSDrvHandle SalSensorOpen(char* SensorName)
{
    tSensorDevice *sDev = NULL;
    uint8_t ui8Sidx, ui8Didx = 0;
    tSDrvHandle sHandle = INVALID_HANDLE;

    for(ui8Sidx = 0; ui8Sidx < SENSOR_MAX_CNT; ui8Sidx++)
    {
        if(!strcmp(sSensorDevList[ui8Sidx].name, SensorName))
        {
            sDev =  &sSensorDevList[ui8Sidx];
            break;
        }
    }
    if (sDev && (sDev->iProbeSt == ProbeSuccess))
    {
        xSemaphoreTake(SmLock, portMAX_DELAY);
        for(ui8Didx = 0; ui8Didx < DRIVER_MAX_CNT; ui8Didx++)
        {
            if (sHandleMapList[ui8Didx].vDrvHandle == INVALID_HANDLE)
                break;
        }
        if (ui8Didx < DRIVER_MAX_CNT)
        {
            sHandle = sDev->sDops->DrvOpen(SensorName);
            if (sHandle >= 0)
            {
                sHandleMapList[ui8Didx].vDrvHandle = sHandle;
                sHandleMapList[ui8Didx].ui8SensorDevIndex = ui8Sidx;
                sHandle = ui8Didx;
            }
            else
            {
                sHandle = INVALID_HANDLE;
            }
        }
        xSemaphoreGive(SmLock);

    }
    return sHandle;
}

/**
 * Close sensor driver
 *
 * @param sHandle  Sensor driver handle
 *
 * @return 0 on success, negative error code on failure.
 */
int32_t SalSensorClose(tSDrvHandle sDrvHandle)
{
    uint8_t ui8Idx;
    uint32_t ret  = 0;
    tDrvHandle vDrvHandle;
    if ((sDrvHandle != INVALID_HANDLE) && (sDrvHandle < DRIVER_MAX_CNT))
    {
        vDrvHandle = sHandleMapList[sDrvHandle].vDrvHandle;
        if (vDrvHandle != INVALID_HANDLE)
        {
            ui8Idx = sHandleMapList[sDrvHandle].ui8SensorDevIndex;
            xSemaphoreTake(SmLock, portMAX_DELAY);
            ret = sSensorDevList[ui8Idx].sDops->DrvClose(vDrvHandle);
            if (ret >= 0 )
            {
                sHandleMapList[sDrvHandle].vDrvHandle = INVALID_HANDLE;
            }
            xSemaphoreGive(SmLock);
        }
        else
        {
            ret = -EINVAL;
            ecm35xx_printf("SM Close: Error Invalid Handle\r\n");
        }
    }
    return ret;
}

/**
 * Sal Sensor Read
 *
 * @param sDrvHandle  Sensor driver handle
 * @param iSensor Sensor type
 * @param ui8Buf pointer to contain data read from sensor
 * @param ui8Len Lenght of buffer
 *
 * @return 0 on success, negative error code on failure.
 */
int32_t SalSensorRead(tSDrvHandle sDrvHandle, tSensorType iSensor,
                    uint8_t *ui8Buf, uint8_t ui8Len)
{
    int32_t ret = -EINVAL;
    uint8_t ui8Idx;
    tDrvHandle vDrvHandle;
    if ((sDrvHandle != INVALID_HANDLE) && (sDrvHandle < DRIVER_MAX_CNT))
    {
        vDrvHandle = sHandleMapList[sDrvHandle].vDrvHandle;
        if (vDrvHandle != INVALID_HANDLE)
        {
            ui8Idx = sHandleMapList[sDrvHandle].ui8SensorDevIndex;
            ret = sSensorDevList[ui8Idx].sDops->
                DrvRead(vDrvHandle, iSensor, ui8Buf, ui8Len);
        }
        else
        {
            ecm35xx_printf("SM Read: Error Invalid Handle\r\n");
        }
    }

    return ret;
}

/**
 * Sal Ioctl call
 *
 * @param sHandle  Sensor driver handle
 * @param iCmd IOCTL command
 * @param iSensor Sensor type
 * @param vPtr pointer to data for IOCTL command
 *
 * @return 0 on success, negative error code on failure.
 */
int32_t SalSensorIoctl(tSDrvHandle sDrvHandle, tSensorType iSensor,
                    tSensorIOCTL iCmd, void *vPtr)
{
    int32_t ret = -EINVAL;
    uint8_t ui8Idx;
    tDrvHandle vDrvHandle;
    if ((sDrvHandle != INVALID_HANDLE) && (sDrvHandle < DRIVER_MAX_CNT))
    {
        vDrvHandle = sHandleMapList[sDrvHandle].vDrvHandle;
        if (vDrvHandle != INVALID_HANDLE)
        {
            ui8Idx = sHandleMapList[sDrvHandle].ui8SensorDevIndex;
            ret = sSensorDevList[ui8Idx].sDops->
                    DrvIoctl(vDrvHandle, iSensor, iCmd, vPtr);
        }

        else
        {
            ecm35xx_printf("SM IOCTL: Error Invalid Handle\r\n");
        }

    }
    return ret;
}

/**
 * Sal Register Event callback
 *
 * @param sDrvHandle  Sensor driver handle
 * @param iSensor Sensor type
 * @param fEvCb callback function to be called
 * @param sEvinfo  event info structure to be passed to callback function
 * @param vPtr pointer to data to be passed to callback function
 *
 * @return 0 on success, negative error code on failure.
 */
int32_t SalSensorRegisterEventCb(tSDrvHandle sDrvHandle, tSensorType iSensor,
                                tSensorEventCb fEvCb, tSensorEvent iEventMask,
                                void *vPtr)
{
    int32_t ret = 0;
    uint8_t ui8Idx;
    tDrvHandle vDrvHandle;
    if ((sDrvHandle != INVALID_HANDLE) && (sDrvHandle < DRIVER_MAX_CNT))
    {
        vDrvHandle = sHandleMapList[sDrvHandle].vDrvHandle;
        if (vDrvHandle != INVALID_HANDLE)
        {
            ui8Idx = sHandleMapList[sDrvHandle].ui8SensorDevIndex;
            sSensorDevList[ui8Idx].sDops->DrvSetCb(vDrvHandle, iSensor,
                            fEvCb, iEventMask, vPtr);
        }
        else
        {
            ret = -EINVAL;
            ecm35xx_printf("SM EventCB: Error Invalid Handle\r\n");
        }
    }
    return ret;
}
