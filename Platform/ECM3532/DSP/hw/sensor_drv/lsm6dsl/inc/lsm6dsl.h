/* sensor_lsm6dsl.h - header file for LSM6DSL accelerometer, gyroscope and
 * temperature sensor driver
 */

 /*
  * Copyright (c) 2017 Linaro Limited
  *
  * SPDX-License-Identifier: Apache-2.0
  */

#ifndef H_LSM6DSL_
#define H_LSM6DSL_

#include "reg.h"
#include "lsm6dsl_reg.h"
#include "sensor_common.h"

#define MAX_LSM6DSL_FIFO_SIZE (1024 * 2)  //2k 16bit words
#define LSM6DSL_SENSOR_DATA_SIZE 3  //3 word16
#define LSM6DSL_MAX_LIVE_SENSOR_BUFFER_SIZE 6 //6 bytes for sensor data X, Y AND Z component.
#define LSM6DSL_MAX_SENSOR_SAMPLE_SIZE  6
#define LSM_TS_LENGTH 4  //bytes for timestamp.

typedef void (*tNotifyDataCb) (uint16_t* buf, uint16_t sizeBytes);

typedef enum {
    LSM6DSL_SUCCESS = 0,
    LSM6DSL_FAIL = -1
}tLsm6DslStatus;

typedef struct {
    uint16_t ui16BaseOffset;
    uint16_t ui16BatchSizeInBytes;  //batch size
    uint16_t ui16BufferSizeInBytes; //Wrap around point for circular buffer.
    //tBatchDataPkt* batchPktPtr;  //To collect info about batch output
} tSensorBatchBufInfo;

typedef struct {
    uint16_t readIndex;
    uint16_t writeIndex;
    uint16_t ui16BatchFillSizeInBytes; //sample count interms of 16bit length.
    uint16_t batchEnable;
} tBatchState;

typedef struct {
    uint16  ui16PPBufOffset; //Base address of ping pong buffer. Two consecutive buffers for samples
    uint8_t sampleSizeInBytes;
    uint8_t liveDataEnable;
} tSensorLiveDataBufInfo;


typedef enum {
    GYRO_ONLY = 0,
    ACCEL_ONLY = 1,
    GYRO_ACCEL = 2,
}tLsm6DslFifoContentType;


typedef struct lsm6dsl_state_t {
    uint16_t accelTimer;
    uint16_t gyroTimer;
    uint16_t batchTimer;

    uint16_t ui16XlOdr;
    uint16_t ui16GyroOdr;

    uint16_t ui16XlPeriodMs;
    uint16_t ui16GyroPeriodMs;

    tSensorBatchBufInfo sGyroBBinfo;
    tSensorLiveDataBufInfo sGyroLBinfo;
    tBatchState sGyroBatchState;

    tSensorBatchBufInfo sXlBBinfo;
    tSensorLiveDataBufInfo sXlLBinfo;
    tBatchState sXlBatchState;

    uint16_t compositeSensorOnAccelEnable;
    uint16_t fifoTh;
    uint16_t fifoOdr;
    uint8_t accelDecimation;
    uint8_t gyroDecimation;
    tLsm6DslFifoContentType fifoContentType;
    uint16_t fifoPatternSizeInWord16;  //Repeating pattern size in fifo

    uint8_t uiOpenCnt;
    uint8_t ui16XlFs;
    uint8_t ui16GyroFs;

}tLsm6Dsl_state;


extern void sensorRegisterDataCb(tNotifyDataCb cbFn);
extern int lsm6dsl_accel_fs_set(uint16_t val);
extern void ProcessLsmSensorLocalMsg(tdspLocalMsg* msg);

tStatus doProcessingOnLsm6Dsl(uint16_t operation, tUnionSensorArg* arg);
tLsm6DslStatus lsm6DslProbe(tUnionSensorArg* arg);
tLsm6DslStatus lsm6DslOpen(tUnionSensorArg* arg);
tLsm6DslStatus lsm6DslRead(tUnionSensorArg* arg);
tLsm6DslStatus lsm6DslIoctl(tUnionSensorArg* arg);
tLsm6DslStatus lsm6DslClose(tUnionSensorArg* arg);

void pollForLiveSensorData(void);
void pollForBatchSensorData(void);
void initLsm6dslSensor(void);

#endif //H_LSM6DSL_
