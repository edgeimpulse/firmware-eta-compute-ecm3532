/* fftCompositeSensor.h - header file for composite sensor example of FFT on accelerometer data
 *
 */

#ifndef H_FFT_COMPOSITE_SENSOR_
#define H_FFT_COMPOSITE_SENSOR_
#include "sensor_common.h"

#include "reg.h"
#include "sensor.h"

#if (CONFIG_LSM6DSL_FFT_DSP == 1)

typedef struct FftCompositeSensor_state_t {
    uint16_t compositeSensorOnAccelEnable;
    uint16_t ui16BaseOffset;
    uint16_t ui16BatchSizeInBytes;
    uint16_t ui16BufferSizeInBytes;
}tFftCompositeSensor_state;


void initFFTCompositeSensor(void);



tStatus  doProcessingOnFftCompositeSensor(uint16_t operation, tUnionSensorArg* arg);
tStatus FftCompositeSensorIoctl(tUnionSensorArg* arg);
#endif

#endif //H_FFT_COMPOSITE_SENSOR_