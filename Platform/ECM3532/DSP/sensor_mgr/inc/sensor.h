#ifndef H_SENSOR_
#define H_SENSOR_
#include "sensor_common.h"
#include "dsp_msg.h"
//Local DSP Messages for Sensor
//All local messages have upper 4 bits all set.
#define ISLOCAL_MSG(m) (m&0xF0)

#define SENSOR_OP_ACCEL_ASYNC_READ 0xf0 //TBD use proper place this is not between m3 and dsp
#define SENSOR_OP_GYRO_ASYNC_READ 0xf1
#define SENSOR_OP_BATCH_READ 0xf2
#define SENSOR_OP_FFT_BATCH_READ 0xf3

typedef tStatus (*remoteCmdHandlerCb) (uint16_t operation, tUnionSensorArg* arg);
typedef void ( * localMsgHandlerCb)(tdspLocalMsg* msg);
tStatus registerRemoteCmdHandlerCb ( remoteCmdHandlerCb cbFn ,int sensorID );
tStatus registerLocalMsgHandlerCb ( localMsgHandlerCb cbFn, int sensorID);

void createSensorTask(void);
void sendSensorEventData(uint8_t hwSensorId, uint8_t sensorType, uint16_t evtType, uint16_t index, uint16_t length);


#endif  //H_SENSOR_