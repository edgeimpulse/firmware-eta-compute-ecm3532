/* fftCopositeSensor.c - Driver for composite FFT accelerometer sensor
 */

#include "sal.h"
#include "sensor_common.h"
#include "sensor.h"
#include "lsm6dsl.h"
#include "lsm6dsl_reg.h"
#include "module_common.h"
#include "fftCompositeSensor.h"
#include "m3Interface.h"
#include "cf6_chess.h"
#include "eta_csp_status.h"
#include "eta_csp_isr.h"
#include "eta_csp_i2c.h"
#include "eta_csp_mailbox.h"
#include "dspI2c.h"
#include "fft.h"

#include "utility.h"


#if (CONFIG_LSM6DSL_FFT_DSP == 1)
extern void LsmSensorReadCb(void* lsmReadType);
extern void createBatchTimerForCompositeSensor(void);

static tFftCompositeSensor_state FftCompositeSensor_State;

tStatus  doProcessingOnFftCompositeSensor(uint16_t operation, tUnionSensorArg* arg)
{
    tStatus status = SUCCESS;
    switch (operation)
    {
    case SENSOR_OP_PROBE:
        status = lsm6DslProbe(arg);
        break;
    case SENSOR_OP_OPEN:
        status = lsm6DslOpen(arg);
        {
            uint16_t fullScalval = 2;  //accel dynamic range set to +-4g
            //set default accel range to +-4g
            TRACE("Setting default +/- 4g FULL SCALE for Accel\n\r");
            status = lsm6dsl_accel_fs_set(fullScalval);
        }
        break;
    case SENSOR_OP_READ:
        //status = lsm6DslRead(arg);
        arg->openArg.status = FAIL;
        status = FAIL;
        break;
    case SENSOR_OP_IOCTL:
        status = FftCompositeSensorIoctl(arg);
        break;
    case SENSOR_OP_CLOSE:
        status = lsm6DslClose(arg);
        break;

    default:
        status = FAIL;
        break;

    }

    return status;
}

tStatus FftCompositeSensorIoctl(tUnionSensorArg* arg)
{
    tIOCTLArguments* ioctlArg = &(arg->ioctlArg);
    tStatus status = SUCCESS;
    uint16_t val;
    uint16_t odr;

    //set odr to suitable rate at compile time.
    switch (ioctlArg->ioctlType)
    {
    case IOCTL_SDEV_SET_ODR:
        if ((ioctlArg->sensorType == SENSOR_TYPE_ACCELEROMETER) || (ioctlArg->sensorType == SENSOR_TYPE_COMPOSITE_FFT))
        {
            ioctlArg->sensorType = SENSOR_TYPE_ACCELEROMETER;
            status = lsm6DslIoctl(arg);
        }
        else
            status = FAIL;
        break;
    case IOCTL_SDEV_GET_ODR:
        if ((ioctlArg->sensorType == SENSOR_TYPE_ACCELEROMETER) || (ioctlArg->sensorType == SENSOR_TYPE_COMPOSITE_FFT))
        {
            ioctlArg->sensorType = SENSOR_TYPE_ACCELEROMETER;
            status = lsm6DslIoctl(arg);
        }
        break;

    case IOCTL_SDEV_BATCH_BUFFER_INFO:
    {
        tSensorDspBufInfo* batch_dspBufInfo = &ioctlArg->ioctlUnionArg.batchBufInfo;;
        if (ioctlArg->sensorType == SENSOR_TYPE_COMPOSITE_FFT)
        {
            if ((FFT_OUTPUT_BUF_SIZE) != batch_dspBufInfo->ui16BatchLen)
            {
                status = FAIL;
            }
            else
            {
                FftCompositeSensor_State.ui16BaseOffset = batch_dspBufInfo->ui16BufOffset;
                FftCompositeSensor_State.ui16BatchSizeInBytes = batch_dspBufInfo->ui16BatchLen;
                FftCompositeSensor_State.ui16BufferSizeInBytes = batch_dspBufInfo->ui16BufLen;

                TRACE("Batch XL_FFTinfo: offset:0x%x, batchLen=0x%x, bufLen=0x%x\n\r",
                    batch_dspBufInfo->ui16BufOffset, batch_dspBufInfo->ui16BatchLen, batch_dspBufInfo->ui16BufLen);
                createBatchTimerForCompositeSensor();
            }
        }
        else
        {
            TRACE("Ioctl Batch buffer info error\n\r", val);
            status = FAIL;
        }
    }
    break;

    case IOCTL_SDEV_BATCH_ENABLE:
        status = lsm6DslIoctl(arg);  //Configure lsm sensor fifo.
        FftCompositeSensor_State.compositeSensorOnAccelEnable = 1;
        break;

    default:
        TRACE("Unsupported IOCTL:\n\r", ioctlArg->ioctlType);
        status = FAIL;
        break;
    }
    arg->ioctlArg.status = status;
    return status;
}

void GetSensorData(uint16_t* data, uint16_t size)
{
    static uint16_t index = 0;
    //TRACE("In Composite Sensor Callback:\n\r");
    //TRACE("Ax=%d, Ay=%d, Az=%d\n\r", data[0], data[1], data[2]);
    chess_dont_warn_dead(size);

    //It accumulates sensor samples till it reaches frame_size of 512 samples in deinterleaved form.
    deInterleaveSensorData(data);
    index++;
    if (N_FFT == index)
    {
        index = 0;
        doFFTOnSensorData(FftCompositeSensor_State.ui16BaseOffset, FFT_OUTPUT_BUF_SIZE);  //For all 3 x,y,z components
        //send mbox command
        TRACE("!!FFT BATCH evt: \n\r");
        sendSensorEventData((uint8_t)LSM6DSL_FFT_SENSOR_ID, (uint8_t)SENSOR_TYPE_COMPOSITE_FFT, (uint16_t)SENSOR_EVENT_BATCH,
            (uint16_t)(FftCompositeSensor_State.ui16BaseOffset), (uint16_t)FftCompositeSensor_State.ui16BatchSizeInBytes);
    }

}

void initFFTCompositeSensor(void)
{
    sensorRegisterDataCb((tNotifyDataCb)GetSensorData);
    registerRemoteCmdHandlerCb(doProcessingOnFftCompositeSensor, LSM6DSL_FFT_SENSOR_ID);
    registerLocalMsgHandlerCb(ProcessLsmSensorLocalMsg, LSM6DSL_FFT_SENSOR_ID);
}
#endif

