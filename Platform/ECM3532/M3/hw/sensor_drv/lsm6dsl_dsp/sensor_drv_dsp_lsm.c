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
#include "config.h"
#include "print_util.h"
#include "errno.h"
#include "sensor_dev.h"
#include "module_common.h"
#include "sensor_common.h"
#include "workQ_common.h"
#include "sal.h"
#include "rpc.h"
#include "shmem.h"
#include "helper_func.h"
 //FreeRTOS related
#include "FreeRTOS.h"
#include "semphr.h"

#define LSM_DSP_DRV_NAME  CONFIG_LSM6DSL_DSP_NAME

#define MAX_SENSORS_SUPPORTED_ON_LSM6DSL 2 // Accel  and Gyro
#define LSM_ONE_SAMPLE_SIZE 6
 //TBD: define the static fns prototype here.

static int32_t lsmDspDrvProbe(void);
static tDrvHandle lsmDspDrvOpen(char* cDrvName);
static int32_t lsmDspDrvSetEvCb(tDrvHandle sDrvHandle,
                                tSensorType iSensor, tSensorEventCb fEvCb,
                                tSensorEvent iEventMask, void* vDrvdata);
static int32_t lsmDspDrvIoctl(tSDrvHandle sDrvHandle, tSensorType iSensor,
                                tSensorIOCTL iCmd, void* vPtr);
static int32_t lsmDspDrvRead(tDrvHandle sDrvHandle, tSensorType iSensor,
                                uint8_t* ui8Buf, uint8_t ui8Len);

tSensorDrvOps lsmDspDrvops = {
    .DrvProbe = lsmDspDrvProbe,
    .DrvOpen = lsmDspDrvOpen,
    //    .DrvClose = lsmDspDrvClose,
        .DrvRead = lsmDspDrvRead,
        .DrvIoctl = lsmDspDrvIoctl,
        .DrvSetCb = lsmDspDrvSetEvCb,
};
typedef struct eventcbInfo {
    tSensorEvent iEventMask;
    tSensorEventCb fEventCb;
    void* vPtr;
    tSensorType iSensor;
    tDrvHandle sDrvHandle;
}teventcbInfo;

teventcbInfo lsmEventList[MAX_SENSORS_SUPPORTED_ON_LSM6DSL] = { 0 };
SemaphoreHandle_t xLsmFnMutex; // Mutex to make all sensor calls reentrant
SemaphoreHandle_t xLsmEventListMutex; // lock for event table
SemaphoreHandle_t xLsmRespSem; // sempahore to handle all the responses.
static tDrvHandle handle = 0;
static void* livedataBuf = NULL;
static int lsmNotifyResponse(uint32_t header, uint32_t data)
{
    // Give response semaphore
    xSemaphoreGive(xLsmRespSem);
    return 0;
}

// Header

static int lsmNotifyEvent(uint32_t header, uint32_t data)
{
    tSensorEvent eventType = 0;
    tSensorType sensorType = 0;
    int index = 0;
    uint16_t byteLength, readOffset;
    eventType = GET_EVENT_TYPE(header);
    sensorType = GET_SENSOR_TYPE(header);

    // take lock
    xSemaphoreTake(xLsmEventListMutex, portMAX_DELAY);
    // look for the callback function based on iSensor,iEventMask
    for (index = 0;index < MAX_SENSORS_SUPPORTED_ON_LSM6DSL; index++)
    {
        if ((lsmEventList[index].fEventCb) && (lsmEventList[index].iSensor == sensorType) && ((lsmEventList[index].iEventMask) & eventType)) {
            tSensorEventInfo eventInfo;
            eventInfo.iEvent = eventType;
            eventInfo.iSensor = sensorType;
            byteLength = ((data >> 16) & 0xFFFF);
            readOffset = ((data) & 0xFFFF);  // word Offset
            eventInfo.sEvData.ui8Buf = (uint8_t *)SharedMemGetAddress(readOffset);
            eventInfo.sEvData.ui16BufLen = byteLength;

            // call the  function
            lsmEventList[index].fEventCb(&eventInfo, lsmEventList[index].vPtr);
            break;
        }
    }
    xSemaphoreGive(xLsmEventListMutex);
    return 0;
}

int Lsm6dslDspDriverInit(void)
{
    uint8_t      eventHeaderMask = 0;
    // create the mutex for locking all calls  per sensor
    xLsmFnMutex = xSemaphoreCreateMutex();
    if (xLsmFnMutex == NULL) {
        //TBD: Add trace message of error
        return -1;
    }
    // Mutex for event list
    xLsmEventListMutex = xSemaphoreCreateMutex();
    if (xLsmEventListMutex == NULL) {
        //TBD: Add trace message of error
        return -1;
    }
    // create the semaphore for all response synchronisation
    xLsmRespSem = xSemaphoreCreateBinary();
    if (xLsmRespSem == NULL) {
        //TBD: Add trace message of error
        return -1;
    }
    SET_MODULEID(eventHeaderMask, RPC_MODULE_ID_SENSOR);
    SET_EVT_RSP(eventHeaderMask, RPC_RESPONSE);
    SET_SENSOR_ID(eventHeaderMask, LSM6DSL_SENSOR_ID);
    // register with RPC notification  for all responses
    rpcRegisterEventCb(eventHeaderMask, lsmNotifyResponse);
    // register with RPC notification for events
    SET_EVT_RSP(eventHeaderMask, RPC_EVENT);
    rpcRegisterEventCb(eventHeaderMask, lsmNotifyEvent);
    //TBD: do cleanup. ideally all of these  use go to if needed but all should be clean
   // register with Sensor driver subsystem
    if (SensorDrvRegister(LSM_DSP_DRV_NAME, &lsmDspDrvops)) {
        //TBD: Add trace message of error
        return -1;
    }
    return 0;
}

static int32_t lsmDspDrvProbe(void)
{
    tSensorCallParams* sensorCallParams;
    int32_t status = 0;
    sensorCallParams = SharedMemAlloc(sizeof(tSensorCallParams));
    if (!sensorCallParams) {
        //TBD: Add trace message of error
        return -1;
    }
    xSemaphoreTake(xLsmFnMutex, portMAX_DELAY);
    sensorCallParams->sensorHwId = LSM6DSL_SENSOR_ID;
    (sensorCallParams->args).probeArg.status = -1; //No need still doing it
    rpcSubmitWork(RPC_MODULE_ID_SENSOR, SENSOR_OP_PROBE, (void*)sensorCallParams);
    // wait for response
    xSemaphoreTake(xLsmRespSem, portMAX_DELAY);
    status = (sensorCallParams->args).probeArg.status;
    SharedMemFree(sensorCallParams);
    xSemaphoreGive(xLsmFnMutex);
    return status;
}

static int32_t lsmDspDrvOpen(char* cDrvName)
{
    tSensorCallParams* sensorCallParams;
    int32_t status = 0;
    sensorCallParams = SharedMemAlloc(sizeof(tSensorCallParams));
    if (!sensorCallParams) {
        //TBD: Add trace message of error
        return -1;
    }
    //TBD: do string comp on name
    xSemaphoreTake(xLsmFnMutex, portMAX_DELAY);
    sensorCallParams->sensorHwId = LSM6DSL_SENSOR_ID;
    (sensorCallParams->args).openArg.status = -1; //No need still doing it
    rpcSubmitWork(RPC_MODULE_ID_SENSOR, SENSOR_OP_OPEN, (void*)sensorCallParams);
    // wait for response
    xSemaphoreTake(xLsmRespSem, portMAX_DELAY);
    status = (sensorCallParams->args).openArg.status; // ideally DSP should populate this field with a toekn to identify multi instance.
    //  or should we send Something like LSM6DSL_SENSOR_ID +1.TBD: based on that set handle variable.
    SharedMemFree(sensorCallParams);
    xSemaphoreGive(xLsmFnMutex);
    return status;
}

static int32_t lsmDspDrvIoctl(tSDrvHandle sDrvHandle, tSensorType iSensor,
                                tSensorIOCTL iCmd, void* vPtr)
{
    tSensorCallParams* sensorCallParams;
    int32_t status = 0;

     if (iCmd== IOCTL_SDEV_GET_SUPPORTED_SENSOR_TYPES) // local case no RPC needed
     {
        tSensorTypeInfo *sInfo = (tSensorTypeInfo *)vPtr;
        sInfo->ui8Cnt = 2;
        sInfo->iType[0]= SENSOR_TYPE_GYROSCOPE;
        sInfo->iType[1]= SENSOR_TYPE_ACCELEROMETER;
         return status;
    }

    sensorCallParams = SharedMemAlloc(sizeof(tSensorCallParams));
    if (!sensorCallParams) {
        //TBD: Add trace message of error
        return -1;
    }
    xSemaphoreTake(xLsmFnMutex, portMAX_DELAY);
    sensorCallParams->sensorHwId = LSM6DSL_SENSOR_ID;
    (sensorCallParams->args).ioctlArg.status = -1;
    (sensorCallParams->args).ioctlArg.ioctlType = (uint16_t)iCmd;
    (sensorCallParams->args).ioctlArg.sensorType = (uint16_t)iSensor;
    //Fill params wherever necessary
    switch (iCmd)
    {
    case IOCTL_SDEV_SET_ODR:
    {
        (sensorCallParams->args).ioctlArg.ioctlUnionArg.odr = *(uint16_t*)vPtr;
        break;
    }
    case IOCTL_SDEV_SET_FULL_SCALE:
    {
        (sensorCallParams->args).ioctlArg.ioctlUnionArg.fullScale = *(uint16_t*)vPtr;
        break;
    }

    case IOCTL_SDEV_BATCH_BUFFER_INFO:
    {
        tSensorBufInfo* pBatchInfo = (tSensorBufInfo*)vPtr;
        (sensorCallParams->args).ioctlArg.ioctlUnionArg.batchBufInfo.ui16BufOffset = SharedMemGetOffset(pBatchInfo->ui8Buf);
        (sensorCallParams->args).ioctlArg.ioctlUnionArg.batchBufInfo.ui16BatchLen = pBatchInfo->ui16BatchLen;
        (sensorCallParams->args).ioctlArg.ioctlUnionArg.batchBufInfo.ui16BufLen = pBatchInfo->ui16BufLen;
        break;
    }
    case IOCTL_SDEV_LIVE_DATA_ENABLE:
    {

        livedataBuf = SharedMemAlloc(2 * LSM_ONE_SAMPLE_SIZE); // 2 Ping pong buffers
        if (!livedataBuf) {
            // TBD: trace
            return -1;
        }
        (sensorCallParams->args).ioctlArg.ioctlUnionArg.liveBufInfo.u16PingPongBufOffset = SharedMemGetOffset(livedataBuf);
        (sensorCallParams->args).ioctlArg.ioctlUnionArg.liveBufInfo.ui16SizeInBytes = 2 * LSM_ONE_SAMPLE_SIZE;

        break;
    }
    default:
        break;
    }
    rpcSubmitWork(RPC_MODULE_ID_SENSOR, SENSOR_OP_IOCTL, (void*)sensorCallParams);
    // wait for response
    xSemaphoreTake(xLsmRespSem, portMAX_DELAY);
    status = (sensorCallParams->args).ioctlArg.status;
    if (SUCCESS == status) {
        //Post action
        switch (iCmd)
        {
        case IOCTL_SDEV_LIVE_DATA_DISABLE:
        {
            SharedMemFree(livedataBuf);
            break;
        }
        case IOCTL_SDEV_GET_ODR:
        {
            *((uint16_t*)vPtr) = (sensorCallParams->args).ioctlArg.ioctlUnionArg.odr;
        }
        default:
            break;
        }

    }
    SharedMemFree(sensorCallParams);
    xSemaphoreGive(xLsmFnMutex);
    return status;
}


//Read
static int32_t lsmDspDrvRead(tDrvHandle sDrvHandle, tSensorType iSensor, uint8_t* ui8Buf, uint8_t ui8Len)
{
    tSensorCallParams* sensorCallParams;
    uint8_t* tempMappedBuf = NULL;
    int32_t status = 0;
    sensorCallParams = SharedMemAlloc(sizeof(tSensorCallParams));
    if (!sensorCallParams) {
        //TBD: Add trace message of error
        return -1;
    }
    //TBD: do string comp on name
    xSemaphoreTake(xLsmFnMutex, portMAX_DELAY);
    sensorCallParams->sensorHwId = LSM6DSL_SENSOR_ID;
    (sensorCallParams->args).readArg.status = -1; //No need still doing it
    (sensorCallParams->args).readArg.sensorType = (uint16_t) iSensor;
    (sensorCallParams->args).readArg.ui16Len =  (uint16_t)ui8Len;
    tempMappedBuf = SharedMemAlloc(ui8Len);
    if (!sensorCallParams) {
        //TBD: Add trace message of error
        return -1;
    }
    (sensorCallParams->args).readArg.u16BufOffset = SharedMemGetOffset(tempMappedBuf);
    rpcSubmitWork(RPC_MODULE_ID_SENSOR, SENSOR_OP_READ, (void*)sensorCallParams);
    // wait for response
    xSemaphoreTake(xLsmRespSem, portMAX_DELAY);
    status = (sensorCallParams->args).readArg.status; // ideally DSP should populate this field with a toekn to identify multi instance.
    //  or should we send Something like LSM6DSL_SENSOR_ID +1.TBD: based on that set handle variable.
    if (SUCCESS == status) {
        memcpy((void*)ui8Buf, (const void*)tempMappedBuf, ui8Len);
    }
    SharedMemFree(tempMappedBuf);
    SharedMemFree(sensorCallParams);
    xSemaphoreGive(xLsmFnMutex);
    return status;
}
static int32_t lsmDspDrvSetEvCb(tDrvHandle sDrvHandle, tSensorType iSensor, tSensorEventCb fEvCb, tSensorEvent iEventMask, void* vDrvdata)
{
    int index = 0;
    if (sDrvHandle != handle) {
        //TBD: Add trace message of error
        return -1;
    }
    xSemaphoreTake(xLsmEventListMutex, portMAX_DELAY);
    for (index = 0;index < MAX_SENSORS_SUPPORTED_ON_LSM6DSL;index++) {
        if (!(lsmEventList[index].fEventCb)) {
            lsmEventList[index].fEventCb = fEvCb;
            lsmEventList[index].iSensor = iSensor;
            lsmEventList[index].iEventMask = iEventMask;
            lsmEventList[index].vPtr = vDrvdata;
            lsmEventList[index].sDrvHandle = sDrvHandle;

            break;
        }
    }
    xSemaphoreGive(xLsmEventListMutex);
    return 0;
}
