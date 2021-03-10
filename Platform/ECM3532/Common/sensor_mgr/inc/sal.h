/*******************************************************************************
 *
 * @file sal.h
 *
 * @brief Sensor Abtraction Layer
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
#ifndef H_SAL_
#define H_SAL_
#include "config.h"

#ifdef __GNUC__
#include <stdint.h>
#else
#include "reg.h"
#endif
#define INVALID_HANDLE 0xFFFFFFFF

#define TIME_STAMP_SIZE (4)
/**
 * Senor Handle, returned from Sensor Driver Open Call,
 * to be used by App for all subsequent calls
 */

typedef int32_t tSDrvHandle;

/**
 * Possible Sensor TYPES
 */
typedef enum {
 /* No sensor type, used for queries */
    SENSOR_TYPE_NONE = 0,
    /** Senor Type Accelerometer */
    SENSOR_TYPE_ACCELEROMETER,//1
    /** Sensor Type Magnetic field*/
    SENSOR_TYPE_MAGNETIC_FIELD,//2
    /** Sensor Type Gyroscope */
    SENSOR_TYPE_GYROSCOPE,//3
    /** Sensor Type Light */
    SENSOR_TYPE_LIGHT,//4
    /** Sensor Type Temperature */
    SENSOR_TYPE_TEMPERATURE,//5
    /** Sensor Type Ambient temperature */
    SENSOR_TYPE_AMBIENT_TEMPERATURE,//6
    /** Sensor Type Pressure */
    SENSOR_TYPE_PRESSURE,//7
    /** Sesnor Type Proximity */
    SENSOR_TYPE_PROXIMITY,//8
    /** Sensor Type Relative humidity */
    SENSOR_TYPE_RELATIVE_HUMIDITY,//9
    /** Sensor Type Rotation vector (quaternion) */
    SENSOR_TYPE_ROTATION_VECTOR,//10
    /** Sensor Type Altitude */
    SENSOR_TYPE_ALTITUDE,//11
    /** Sensor Type Weight */
    SENSOR_TYPE_WEIGHT,//12
    /** Sensor Type Linear Accelerometer (Without Gravity) */
    SENSOR_TYPE_LINEAR_ACCEL,//13
    /** Sensor Type Gravity */
    SENSOR_TYPE_GRAVITY,//14
    /** Sensor Type Euler Orientation */
    SENSOR_TYPE_EULER,//15
    /** Sensor Type Color */
    SENSOR_TYPE_COLOR,//16
    /** Sensor Type DIE TEMP */
    SENSOR_TYPE_DIE_TEMP,//17
    /*
     * User Defined Sensor type
     * useful for composite sensor.
     */
    /** User defined sensor type 1 */
    SENSOR_TYPE_COMPOSITE_FFT,//18
    /** User defined sensor type 2 */
    SENSOR_TYPE_USER_DEFINED_2,//19
    /** User defined sensor type 3 */
    SENSOR_TYPE_USER_DEFINED_3,//20
    /** User defined sensor type 4 */
    SENSOR_TYPE_USER_DEFINED_4,//21
    /** User defined sensor type 5 */
    SENSOR_TYPE_USER_DEFINED_5//22
} tSensorType;

/**
 * Sensor Manager IOCTL Commands
 */
typedef enum {
    /**
     * SENSOR RESET, Can be implemented in driver for sensor required
     * special reset sequence
     */
    IOCTL_SDEV_RESET_SENSOR_DEV,//0
    /** GET SUPPORTED SENSOR TYPES */
    IOCTL_SDEV_GET_SUPPORTED_SENSOR_TYPES,//1
    /**
     * SENSOR ENABLE, Can be implemented in driver for sensor required
     * special enable sequence
     */
    IOCTL_SDEV_ENABLE_SENSOR,//2
    /**
     * SENSOR DISABLE Can be implemented in driver for sensor required
     * special disable sequence
     */
    IOCTL_SDEV_DISABLE_SENSOR,//3
    /** SET SENSOR ODR */
    IOCTL_SDEV_SET_ODR,//4
    /** GET SENSOR ODR */
    IOCTL_SDEV_GET_ODR,//5
    /**
     * SENSOR CALIBRATE, Can be implemented in driver for sensor required
     * special disable sequence
     */
    IOCTL_SDEV_CALIBRATE,//6
    /** SET SENSOR OFFSET*/
    IOCTL_SDEV_SET_OFFSET,//7
    /** GET SENSOR OFFSET*/
    IOCTL_SDEV_GET_OFFSET,//8
    /** SET SENSOR FULL SCALE*/
    IOCTL_SDEV_SET_FULL_SCALE,//9
    /** GET SENSOR FULL SCALE*/
    IOCTL_SDEV_GET_FULL_SCALE,//10
    /** SET SENSOR UNIT */
    IOCTL_SDEV_SET_UNIT,//11
    /** GET SENSOR UNIT */
    IOCTL_SDEV_GET_UNIT,//12
    /** SET SENSOR BATCHING INFO */
    IOCTL_SDEV_BATCH_BUFFER_INFO,//13
    /** ENABLE BATCHING */
    IOCTL_SDEV_BATCH_ENABLE,//14
    /** DISABLE BATCHING */
    IOCTL_SDEV_BATCH_DISABLE,//15
    /** BATCH FLUSH, Reset batch pointers*/
    IOCTL_SDEV_BATCH_FLUSH,//16
    /** ENABLE ASYNC DATA*/
    IOCTL_SDEV_LIVE_DATA_ENABLE,//17
    /** DISABLE ASYNC DATA*/
    IOCTL_SDEV_LIVE_DATA_DISABLE,//18
    /**
     * Any Custom command can be implemented in driverrequired for sensor
     */
    IOCTL_SDEV_CUSTOM_CMD//19
} tSensorIOCTL;

/**
 * SENSOR Manager Events
 */
typedef enum
{
    /**
     * Batch Data Complete Event
     */
    SENSOR_EVENT_BATCH = (1 << 0),
    /**
     * Async Read Complete Event
     */
    SENSOR_EVENT_LIVE_DATA = (1 << 1)
} tSensorEvent;

/**
 * Batch Buffer Info
 */
typedef struct {
    /**
     * Batch Buffer Pointer
     */
    uint8_t *ui8Buf;
    /**
     * Batch Length in Bytes
     */
    uint16_t ui16BatchLen;
    /**
     * Buffer Length in Bytes
     */
    uint16_t ui16BufLen;
} tSensorBufInfo;

/**
 * Sensor Manager Event Info
 */
typedef struct {
    /**
     * Event Type
     */
    tSensorEvent iEvent;
    /**
     * Sensor Type for which event is received
     */
    tSensorType iSensor;
    /**
     * Sensor Event Data
     */
    tSensorBufInfo sEvData;
} tSensorEventInfo;

/**
 * Sensor Type Info
 */
typedef struct {
    /**
    * Count of Sensor supported by driver
    */
    uint8_t ui8Cnt;
    /**
    * Supported Sensor Types
    */
    tSensorType iType[CONFIG_SENSOR_DEV_COUNT];
} tSensorTypeInfo;

/**
 * Senosr Event callback prototype
 */
typedef int32_t (*tSensorEventCb) (tSensorEventInfo *sEvInfo, void *vPtr);

/**
 * Open sensor driver, to be called by application before any other call to
 * Sensor manager. Returns Sensor Handle to be used for any subsequent call to
 * same driver.
 *
 * @param SensorName Sensor driver name
 *
 * @return tSensorDevice valid sensor handle on success,
 *          negative error code on failure.
 */
tSDrvHandle SalSensorOpen(char* SensorName);

/**
 * Close sensor driver, application can close sensor driver which was
 * opened earlier
 *
 * @param sDrvHandle  Sensor driver handle
 *
 * @return 0 on success, negative error code on failure.
 */
int32_t SalSensorClose(tSDrvHandle sDrvHandle);

/**
 * Sal Sensor Read, Synchronous mode blocking call to
 * read sensor sample from driver
 *
 * @param sDrvHandle  Sensor driver handle
 * @param iSensor Sensor type
 * @param ui8Buf pointer to contain data read from sensor
 * @param ui8Len Length of buffer
 *
 * @return 0 on success, negative error code on failure.
 */
int32_t SalSensorRead(tSDrvHandle sDrvHandle, tSensorType iSensor,
                    uint8_t *ui8Buf, uint8_t ui8Len);

/**
 * Sal Ioctl call, to set sensor parameters identfied by command field
 *
 * @param sDrvHandle  Sensor driver handle
 * @param iCmd IOCTL command
 * @param iSensor Sensor type
 * @param vPtr pointer to data for IOCTL command
 *
 * @return 0 on success, negative error code on failure.
 */
int32_t SalSensorIoctl(tSDrvHandle sDrvHandle, tSensorType iSensor,
                    tSensorIOCTL iCmd, void *vPtr);

/**
 * Sal Register Event callback, registers a callback with driver
 * to receive data in Asynchronous mode
 *
 * @param sDrvHandle  Sensor driver handle
 * @param iSensor Sensor type
 * @param fEventCb callback function to be called
 * @param iEventMask event for which callback to be registered
 * @param vPtr pointer to data to be passed to callback function
 *
 * @return 0 on success, negative error code on failure.
 */
int32_t SalSensorRegisterEventCb(tSDrvHandle sDrvHandle, tSensorType iSensor,
            tSensorEventCb fEventCb, tSensorEvent iEventMask, void *vPtr);

#endif//# H_SAL_
