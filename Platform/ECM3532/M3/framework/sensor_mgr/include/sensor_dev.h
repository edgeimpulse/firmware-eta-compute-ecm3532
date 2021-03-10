/*******************************************************************************
 *
 * @file sensor_dev.h
 *
 * @brief Sensor Device layer
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
#ifndef H_SENSOR_DEV_
#define H_SENSOR_DEV_
#include "sal.h"
#include <stdint.h>


void SalInit(void);

typedef int32_t tDrvHandle;

typedef tDrvHandle (*tDrvOpen)(char *cDrvName);

typedef int32_t (*tDrvProbe)(void);

typedef int32_t (*tDrvClose)(tDrvHandle sDrvHandle);

typedef int32_t (*tDrvReadSample)(tDrvHandle sDrvHandle, tSensorType iSensor,
                                    uint8_t *ui8Buf, uint8_t ui8Len);

typedef int32_t (*tDrvIoctl)(tSDrvHandle sDrvHandle, tSensorType iSensor,
                                tSensorIOCTL iCmd, void *vPtr);

typedef int32_t (*tDrvSetEventCb)(tDrvHandle sDrvHandle,
                                    tSensorType iSensor, tSensorEventCb fEvCb,
                                    tSensorEvent iEventMask, void *vPtr);
typedef struct {
    tDrvOpen DrvOpen;
    tDrvProbe DrvProbe;
    tDrvClose DrvClose;
    tDrvReadSample DrvRead;
    tDrvIoctl DrvIoctl;
    tDrvSetEventCb DrvSetCb;
} tSensorDrvOps;

/* Sensor Driver Structure */
int32_t SensorDrvRegister(char *Name, tSensorDrvOps *sDrvOps);

int32_t SensorDrvDeRegister(char *Name);
#endif// H_SENSOR_DEV_
