#ifndef H_SENSOR_COMMON_
#define H_SENSOR_COMMON_

//Remember to include config.h in your sources before this file

#ifdef __GNUC__
#include <stdint.h>
#else
#include "reg.h"
#endif
#include "config.h"

#ifndef CONFIG_LSM6DSL_DSP
    #define CONFIG_LSM6DSL_DSP 0
#endif

#ifndef CONFIG_LSM6DSL_FFT_DSP
    #define CONFIG_LSM6DSL_FFT_DSP 0
#endif
#ifndef CONFIG_SENSOR_BNO055
    #define CONFIG_SENSOR_BNO055 0
#endif
#ifndef CONFIG_SENSOR_TDK_ICM_20602
    #define CONFIG_SENSOR_TDK_ICM_20602 0
#endif
// Add a new sensnor's entry here

#define MAX_PERMISSABLE_SENSOR_DSP 4
//Modify this when you add a new sensor
#define TOTAL_SENSOR_COUNT   (CONFIG_LSM6DSL_DSP + CONFIG_LSM6DSL_FFT_DSP + CONFIG_SENSOR_BNO055 + CONFIG_SENSOR_TDK_ICM_20602)

# if ( TOTAL_SENSOR_COUNT >MAX_PERMISSABLE_SENSOR_DSP)
    #error DSP ensor limit exhausted
#endif

#if ( CONFIG_LSM6DSL_DSP  == 1)
    #define LSM6DSL_SENSOR_ID    (CONFIG_LSM6DSL_DSP -1)
#else
    #define LSM6DSL_SENSOR_ID  -1
#endif

#if ( CONFIG_SENSOR_BNO055  == 1)
    #define BNO055_SENSOR_ID     (CONFIG_LSM6DSL_DSP +CONFIG_SENSOR_BNO055 -1)
#else
    #define BOSCH_SBNO055_SENSOR_ID  -1
#endif

#if ( CONFIG_SENSOR_TDK_ICM_20602  == 1)
    #define ICM_20602_SENSOR_ID     (CONFIG_LSM6DSL_DSP +CONFIG_SENSOR_BNO055 + CONFIG_SENSOR_TDK_ICM_20602 -1)
#else
    #define ICM_20602_SENSOR_ID  -1
#endif

#if (CONFIG_LSM6DSL_FFT_DSP == 1)
    #define  LSM6DSL_FFT_SENSOR_ID  (CONFIG_LSM6DSL_DSP +CONFIG_SENSOR_BNO055 + CONFIG_SENSOR_TDK_ICM_20602 + CONFIG_LSM6DSL_FFT_DSP -1)
#endif

//Make a new sensor ID here

#define IN
#define OUT
#define INOUT

//Sensor Commands (8 bits)
#define SENSOR_OP_PROBE 0x01
#define SENSOR_OP_OPEN  0x02
#define SENSOR_OP_IOCTL 0x03
#define SENSOR_OP_READ  0x04
#define SENSOR_OP_WRITE 0x05
#define SENSOR_OP_CLOSE 0x06




/*
 *
 * HEADER FORMAT
 *
 * D[31-24]     D[23-16]       D[15-8]      D[7-5]        D4 D3        D2         D1 D0
 * -------------------------- ------------- ------------ ------------ ---------- -----------
 *  |        RSVD            | SENSOR TYPE | EVENT TYPE |HW SENSOR ID| EVNT/RSP | MODULE ID|
 * -------------------------- ------------- ------------ ------------ ---------- -----------
 *
 *
 */

#define HW_SENSOR_ID_NUM_BITS     2
#define HW_SENSOR_ID_START_POS    3

#define GET_HW_SENSOR_ID(X) (GETBITS(X,HW_SENSOR_ID_START_POS,HW_SENSOR_ID_NUM_BITS) )
#define SET_SENSOR_ID(X,sensorId) (WRITETO(X,HW_SENSOR_ID_START_POS,HW_SENSOR_ID_NUM_BITS,sensorId))

#define SENSOR_TYPE_NUM_BITS     8
#define SENSOR_TYPE_START_POS    8

#define GET_SENSOR_TYPE(X) (GETBITS(X,SENSOR_TYPE_START_POS,SENSOR_TYPE_NUM_BITS) )
#define SET_SENSOR_TYPE(X,sensorType) (WRITETO(X,SENSOR_TYPE_START_POS,SENSOR_TYPE_NUM_BITS,sensorType))

#define EVENT_TYPE_NUM_BITS     3
#define EVENT_TYPE_START_POS    5

#define GET_EVENT_TYPE(X) (GETBITS(X,EVENT_TYPE_START_POS,EVENT_TYPE_NUM_BITS) )
#define SET_EVENT_TYPE(X,eventType) (WRITETO(X,EVENT_TYPE_START_POS,EVENT_TYPE_NUM_BITS,eventType))

typedef enum status {
    SUCCESS = 0,
    FAIL = -1
} tStatus;

typedef struct {
    uint16_t u16PingPongBufOffset;  //Two consecutive buffers must be allocated.
    uint16_t ui16SizeInBytes;  //Ping Ping buffer size = twice sensor sample size.
} tSensorLiveDspBufInfo;

typedef struct {
    uint16_t ui16BufOffset;
    uint16_t ui16BatchLen;
    uint16_t ui16BufLen;
} tSensorDspBufInfo;

typedef struct probeArguments_t {
    OUT uint16_t status;
} tProbeArguments;

typedef struct openArguments_t {
    OUT uint16_t status;
} tOpenArguments;

typedef struct readArguments_t {
    OUT uint16_t status;
    IN  uint16_t sensorType;
    INOUT uint16_t u16BufOffset;
    IN uint16_t  ui16Len;
} tReadArguments;



typedef union tUnionIOCTLArg_t {
    uint16_t        odr;                //for IOCTL_SDEV_SET_ODR
    uint16_t        fullScale;          //IOCTL_SDEV_SET_FULL_SCALE
    tSensorDspBufInfo  batchBufInfo;      //for IOCTL_SDEV_BATCH_BUFFER_INFO
    tSensorLiveDspBufInfo  liveBufInfo;   //IOCTL_SDEV_LIVE_DATA_ENABLE
} tUnionIOCTLArg;

typedef struct ioctlArguments_t {
    OUT uint16_t status;
    IN uint16_t ioctlType;  //like set or get functions for sensor attributes like odr, range, batch buffer info.
    IN uint16_t sensorType;
//    IN void* vPtr;
    IN tUnionIOCTLArg  ioctlUnionArg;  //argements for specified sensor operation.

} tIOCTLArguments;

typedef struct closeArguments_t {
    OUT uint16_t status;
} tCloseArguments;

typedef union additionalCmdArg_t {
    tProbeArguments probeArg;
    tOpenArguments  openArg;
    tReadArguments  readArg;
    tIOCTLArguments ioctlArg;
    tCloseArguments closeArg;
} tUnionSensorArg;

typedef struct {
    uint16_t sensorHwId;  //Ids of sensor vendor name like lsm6dsl.. etc
    tUnionSensorArg  args;  //argements for specified sensor operation.
} tSensorCallParams;

#endif  //H_SENSOR_COMMON_
