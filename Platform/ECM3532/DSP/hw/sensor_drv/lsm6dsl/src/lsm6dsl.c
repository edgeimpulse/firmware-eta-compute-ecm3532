/* lsm6dsl.c - Driver for LSM6DSL accelerometer and gyroscope
 */

 /*
  * Copyright (c) 2017 Linaro Limited
  *
  * SPDX-License-Identifier: Apache-2.0
  *
  * Copyright (C) 2019 Eta Compute, Inc
  * Modified driver to fit CoolFlux DSP16
  * Added FIFO mode
  */


#include "sal.h"
#include "sensor_common.h"
#include "sensor.h"
#include "timer.h"
#include "lsm6dsl.h"
#include "lsm6dsl_reg.h"
#include "module_common.h"

#include "cf6_chess.h"
#include "eta_csp_status.h"
#include "eta_csp_isr.h"
#include "eta_csp_i2c.h"
#include "eta_csp_mailbox.h"
#include "dspI2c.h"

#include "utility.h"
#include "fft.h"
#include "m3Interface.h"

static tLsm6Dsl_state lsm6Dsl_State;
static uint16_t odr_map[] = { 0, 12, 26, 52, 104, 208, 416, 833, 1660, 3330, 6660 };

uint16_t AccelSensorBuf[LSM6DSL_MAX_LIVE_SENSOR_BUFFER_SIZE / 2];
uint16_t GyroSensorBuf[LSM6DSL_MAX_LIVE_SENSOR_BUFFER_SIZE / 2];
extern void LsmSensorReadCb(void* lsmReadType);


#define NOTIFY_XL_FN_LIST_SIZE  4
tNotifyDataCb  notifyAccelFnList[4] = { 0 };
void sensorRegisterDataCb(tNotifyDataCb cbFn)
{
    int i;
    for (i = 0; i < NOTIFY_XL_FN_LIST_SIZE; i++)
    {
        if (!(notifyAccelFnList[i]))
        {
            notifyAccelFnList[i] = cbFn;
            break;
        }
        if (i == (NOTIFY_XL_FN_LIST_SIZE - 1))
        {
            TRACE("Issue in sensor event call back registration");
        }
    }
    return;
}


/*
 * convert odr to register value
 */
static uint16_t lsm6dsl_odr_to_val(uint16_t odr)
{
    uint16_t i = 0;

    for (i = 0; i < ARRAY_SIZE(odr_map); i++) {
        if (odr == odr_map[i]) {
            return i;
        }
    }

    return i;
}

/*
 * find closet from array
 */
static uint16_t findClosestIndex(uint16_t arr[], uint16_t n, uint16_t target)
{
    uint16_t index = 0;
    if (target <= arr[0])
        return 0;
    if (target >= arr[n - 1])
        return n - 1;

    // Doing binary search
    uint16_t i = 0, j = n, mid = 0;
    while (i < j) {
        mid = (i + j) / 2;
        index = mid;
        if (arr[mid] == target)
        {
            index = mid;
            break;
        }

        /* If target is less than array element,
            then search in left */
        if (target < arr[mid]) {

            // If target is greater than previous
            // to mid, return closest of two
            if (mid > 0 && target > arr[mid - 1])
            {
                if (target - arr[mid - 1] >= arr[mid] - target)
                    index = mid;
                else
                    index = (mid - 1);
                break;
            }

            /* Repeat for left half */
            j = mid;
        }

        // If target is greater than mid
        else {
            if (mid < n - 1 && target < arr[mid + 1])
            {
                if (target - arr[mid] >= arr[mid + 1] - target)
                    index = mid + 1;
                else
                    index = mid;
                break;

            }
            // update i
            i = mid + 1;
        }
    }

    // Only single element left after search
    return index;
}

/**
  * @brief  Accelerometer data rate selection.[set]
  *
  * @param  data    Read / write interface definitions
  * @param  val    Change the values of odr_xl in reg CTRL1_XL
  * @retval        Interface status (MANDATORY: return 0 -> no Error).
  *
  */
static int lsm6dsl_accel_data_rate_set(tLsm6Dsl_state* data, lsm6dsl_odr_xl_t val)
{
    int result;

    result = i2c_update_reg(LSM6DSL_SLAVE_ADDR, LSM6DSL_REG_CTRL1_XL, LSM6DSL_MASK_CTRL1_XL_ODR_XL,
        (val << LSM6DSL_SHIFT_CTRL1_XL_ODR_XL));
    if (result)
    {
        TRACE("Error in setting accel ODR");
        data->ui16XlOdr = odr_map[0];
        return -1;
    }

    data->ui16XlOdr = odr_map[val];
    if (data->ui16XlOdr)
    {
        data->accelTimer = createTimer(LsmSensorReadCb, (void*)SENSOR_OP_ACCEL_ASYNC_READ);
        data->ui16XlPeriodMs = (1000 / data->ui16XlOdr);
        //data->ui16XlPeriodMs = 500;
    }
    return 0;
}

static int lsm6dsl_gyro_data_rate_set(tLsm6Dsl_state* data, lsm6dsl_odr_g_t val)
{
    int result;

    result = i2c_update_reg(LSM6DSL_SLAVE_ADDR, LSM6DSL_REG_CTRL2_G, LSM6DSL_MASK_CTRL2_G_ODR_G,
        (val << LSM6DSL_SHIFT_CTRL2_G_ODR_G));
    if (result)
    {
        TRACE("Error in setting Gyro ODR");
        data->ui16GyroOdr = odr_map[0];
        return -1;
    }

    data->ui16GyroOdr = odr_map[val];
    if (data->ui16GyroOdr)
    {
        data->gyroTimer = createTimer(LsmSensorReadCb, (void*)SENSOR_OP_GYRO_ASYNC_READ);
        data->ui16GyroPeriodMs = 1000 / data->ui16GyroOdr;
    }

    return 0;
}

/**
  * @brief  Accel full-scale selection.[set]
  *
  * @param  data    Read / write interface definitions
  * @param  val    Change the values of fs_g in reg LSM6DSL_REG_CTRL1_XL
  * @retval        Interface status (MANDATORY: return 0 -> no Error).
  *
  */
int lsm6dsl_accel_fs_set(uint16_t val)
{
    int result;
    result = i2c_update_reg(LSM6DSL_SLAVE_ADDR, LSM6DSL_REG_CTRL1_XL, LSM6DSL_MASK_CTRL1_XL_FS_XL,
        (val << LSM6DSL_SHIFT_CTRL1_XL_FS_XL));

    if (result)
    {
        TRACE("Error in setting accel full scale");
        lsm6Dsl_State.ui16XlFs = LSM6DSL_XL_FS_ND;
        return -1;
    }

    lsm6Dsl_State.ui16XlFs = val;
    return 0;
}

/**
  * @brief  Gyroscope full-scale selection.[set]
  *
  * @param  data    Read / write interface definitions
  * @param  val    Change the values of fs_g in reg CTRL2_G
  * @retval        Interface status (MANDATORY: return 0 -> no Error).
  *
  */
static int lsm6dsl_gy_fs_set(tLsm6Dsl_state* data, lsm6dsl_fs_g_t val)
{
    int result;

    result = i2c_update_reg(LSM6DSL_SLAVE_ADDR, LSM6DSL_REG_CTRL2_G, LSM6DSL_MASK_CTRL2_G_FS_G,
        (val << LSM6DSL_SHIFT_CTRL2_G_FS_G));
    if (result)
    {
        TRACE("Error in setting Gyro Full Scale");
        data->ui16GyroFs = LSM6DSL_GY_FS_ND;
        return -1;
    }
    data->ui16GyroFs = val;

    return 0;
}

static void lsm6dsl_fifo_watermark_set(uint16_t val)
{
    int result;
    char tempVal;
    tempVal = (val >> 8) & 0x7;  //3 bit from MS byte

    result = i2c_write_reg(LSM6DSL_SLAVE_ADDR, LSM6DSL_REG_FIFO_CTRL1, (val & 0xff));
    if (result)
    {
        TRACE("Error in setting fifo threshold ctrl1\n\r");
        return;
    }
    result = i2c_update_reg(LSM6DSL_SLAVE_ADDR, LSM6DSL_REG_FIFO_CTRL2,
        LSM6DSL_MASK_FIFO_CTRL2_FTH, (tempVal << LSM6DSL_SHIFT_FIFO_CTRL2_FTH));
    if (result)
    {
        TRACE("Error in setting fifo threshold ctrl2[2:0]\n\r");
        return;
    }
    return;
}

tStatus  doProcessingOnLsm6Dsl(uint16_t operation, tUnionSensorArg* arg)
{
    tStatus status = SUCCESS;
    switch (operation)
    {
    case SENSOR_OP_PROBE:
        status = lsm6DslProbe(arg);
        break;
    case SENSOR_OP_OPEN:
        status = lsm6DslOpen(arg);
        break;
    case SENSOR_OP_READ:
        status = lsm6DslRead(arg);
        break;
    case SENSOR_OP_IOCTL:
        status = lsm6DslIoctl(arg);
        break;
    case SENSOR_OP_CLOSE:
        status = lsm6DslClose(arg);
        break;

    default:
        status = LSM6DSL_FAIL;
        break;

    }

    return LSM6DSL_SUCCESS;
}


static void lsm_state_init(void)
{
    lsm6Dsl_State.uiOpenCnt = 0;

    lsm6Dsl_State.sXlBBinfo.ui16BaseOffset = 0;
    lsm6Dsl_State.sXlBBinfo.ui16BatchSizeInBytes = 0;

    lsm6Dsl_State.sGyroBBinfo.ui16BaseOffset = 0;
    lsm6Dsl_State.sGyroBBinfo.ui16BatchSizeInBytes = 0;

    lsm6Dsl_State.sXlLBinfo.ui16PPBufOffset = 0;
    lsm6Dsl_State.sXlLBinfo.sampleSizeInBytes = 0;

    lsm6Dsl_State.sGyroLBinfo.ui16PPBufOffset = 0;
    lsm6Dsl_State.sGyroLBinfo.sampleSizeInBytes = 0;

    lsm6Dsl_State.fifoOdr = 0;
    lsm6Dsl_State.compositeSensorOnAccelEnable = 0;

    lsm6Dsl_State.accelTimer = INVALID_TIMER_HANDLE;
    lsm6Dsl_State.gyroTimer = INVALID_TIMER_HANDLE;
    lsm6Dsl_State.batchTimer = INVALID_TIMER_HANDLE;
}

tLsm6DslStatus lsm6DslProbe(tUnionSensorArg* arg)
{
    char chip_id;
    tLsm6DslStatus status = LSM6DSL_SUCCESS;
#if 0
    if (i2c_update_reg(LSM6DSL_SLAVE_ADDR, LSM6DSL_REG_CTRL3_C, LSM6DSL_MASK_CTRL3_C_BOOT,
        (1 << LSM6DSL_SHIFT_CTRL3_C_BOOT)))
    {
        TRACE("failed to reboot device\n");
        status = LSM6DSL_FAIL;
    }
    /* Wait sensor turn-on time as per datasheet */
    delayMs((unsigned long int)35);
#endif

    if (i2c_read_reg(LSM6DSL_SLAVE_ADDR, LSM6DSL_REG_WHO_AM_I, &chip_id, 1))
    {
        TRACE("failed reading chip id\r\n");
        status = LSM6DSL_FAIL;
    }

    if (chip_id != LSM6DSL_VAL_WHO_AM_I) {
        TRACE("invalid chip id 0x%x\r\n", chip_id);
        status = LSM6DSL_FAIL;
    }
    TRACE("chip id 0x%x\r\n", chip_id);

    if (i2c_update_reg(LSM6DSL_SLAVE_ADDR,
        LSM6DSL_REG_CTRL3_C,
        LSM6DSL_MASK_CTRL3_C_SW_RESET,
        1 << LSM6DSL_SHIFT_CTRL3_C_SW_RESET))
    {
        TRACE("soft reset failed\r\n");
        status = LSM6DSL_FAIL;
    }

    if (i2c_update_reg(LSM6DSL_SLAVE_ADDR,
        LSM6DSL_REG_CTRL3_C,
        LSM6DSL_MASK_CTRL3_C_BDU |
        LSM6DSL_MASK_CTRL3_C_IF_INC,
        1 << LSM6DSL_SHIFT_CTRL3_C_BDU |
        1 << LSM6DSL_SHIFT_CTRL3_C_IF_INC))
    {
        TRACE("bdu update and auto inc failed\r\n");
        status = LSM6DSL_FAIL;
    }

#if 0
    if (i2c_write_reg(LSM6DSL_SLAVE_ADDR, LSM6DSL_REG_CTRL4_C, LSM6DSL_MASK_CTRL4_C_INT2_ON_INT1))
    {
        TRACE("failed to set interrupt enable INT2_on_INT1");
        status = LSM6DSL_FAIL;
    }
#endif

    //Caution: Set Accel analog BW to 400Hz. When sample rate is above 800Hz we might need to change this settings.
    if (i2c_update_reg(LSM6DSL_SLAVE_ADDR, LSM6DSL_REG_CTRL1_XL, LSM6DSL_REG_CTRL1_XL_MASK, LSM6DSL_XL_ANA_BW_400Hz))
    {
        TRACE("setting of Analog BW failed\r\n");
        status = LSM6DSL_FAIL;
    }

    lsm_state_init();


    arg->probeArg.status = status;
    return status;
}

tLsm6DslStatus lsm6DslOpen(tUnionSensorArg* arg)
{
    lsm6Dsl_State.uiOpenCnt++;

    arg->openArg.status = LSM6DSL_SUCCESS;
    return LSM6DSL_SUCCESS;
}

tLsm6DslStatus lsm6DslRead(tUnionSensorArg* arg)
{
    tLsm6DslStatus status = LSM6DSL_SUCCESS;
    tReadArguments readArg = arg->readArg;
    uint16_t localTempBuffer[LSM6DSL_MAX_LIVE_SENSOR_BUFFER_SIZE];
    volatile uint16_t chess_storage(IOMEM)* tempIoMemBuffer;
    tempIoMemBuffer = (uint16_t chess_storage(IOMEM)*)getSharedMemoryAddress(readArg.u16BufOffset);

    if (readArg.ui16Len < LSM6DSL_MAX_LIVE_SENSOR_BUFFER_SIZE)
    {
        TRACE("live sensor data buffer length < %d\r\n", LSM6DSL_MAX_LIVE_SENSOR_BUFFER_SIZE);
        arg->readArg.status = LSM6DSL_FAIL;
        return LSM6DSL_FAIL;
    }

    switch (readArg.sensorType) {
    case SENSOR_TYPE_ACCELEROMETER:
        if (i2c_read_reg(LSM6DSL_SLAVE_ADDR, LSM6DSL_REG_OUTX_L_XL, (char *)localTempBuffer, (uint16_t)readArg.ui16Len))
        {
            TRACE("Issue in lsm6Dsl Accel Read\r\n");
            status = LSM6DSL_FAIL;
        }
        break;
    case SENSOR_TYPE_GYROSCOPE:
        if (i2c_read_reg(LSM6DSL_SLAVE_ADDR, LSM6DSL_REG_OUTX_L_G, (char*)localTempBuffer, (uint16_t)readArg.ui16Len))
        {
            TRACE("Issue in lsm6Dsl Gyro Read\r\n");
            status = LSM6DSL_FAIL;
        }
        break;
    default:
        status = LSM6DSL_FAIL;
    }

    //copy sensor data from local x memory to iomem shared memory
    memcpy_xmem_to_iomem((void chess_storage(IOMEM)*)tempIoMemBuffer, (void *)localTempBuffer, (uint16_t)readArg.ui16Len);

    arg->readArg.status = status;
    return status;
}

static void lsm6dsl_batch_enable(uint16_t sensorType)
{
    int result;
    char tempVal;
    uint16_t fifoOdr;
    uint8_t accelDecimation = 0;
    uint8_t gyroDecimation = 0;
    tSensorBatchBufInfo* accelBatchInfo = &lsm6Dsl_State.sXlBBinfo;
    tSensorBatchBufInfo* gyroBatchInfo = &lsm6Dsl_State.sGyroBBinfo;

    if (SENSOR_TYPE_ACCELEROMETER == sensorType)
    {
        if ((accelBatchInfo->ui16BatchSizeInBytes) && (lsm6Dsl_State.ui16XlOdr == 0))
        {
            lsm6Dsl_State.sXlBatchState.batchEnable = 0;
            TRACE("Issue in setting accel batching\n\r");
            return;
        }
        else
        {
            TRACE("XL batch active:\n\r");
            lsm6Dsl_State.sXlBatchState.readIndex = lsm6Dsl_State.sXlBatchState.writeIndex = 0;
            lsm6Dsl_State.sXlBatchState.ui16BatchFillSizeInBytes = 0;
            lsm6Dsl_State.sXlBatchState.batchEnable = 1;
        }
        //special case when gyro enabled before accel
        if (lsm6Dsl_State.sGyroBatchState.batchEnable)
        {
            lsm6Dsl_State.sGyroBatchState.readIndex = lsm6Dsl_State.sXlBatchState.writeIndex = 0;
            lsm6Dsl_State.sGyroBatchState.ui16BatchFillSizeInBytes = 0;
        }
    }

    if (SENSOR_TYPE_GYROSCOPE == sensorType)
    {
        if ((gyroBatchInfo->ui16BatchSizeInBytes) && (lsm6Dsl_State.ui16GyroOdr == 0))
        {
            lsm6Dsl_State.sGyroBatchState.batchEnable = 0;
            TRACE("Issue in setting gyro batching\n\r");
            return;
        }
        else
        {
            TRACE("gyro batch active:\n\r");
            lsm6Dsl_State.sGyroBatchState.readIndex = lsm6Dsl_State.sXlBatchState.writeIndex = 0;
            lsm6Dsl_State.sGyroBatchState.ui16BatchFillSizeInBytes = 0;
            lsm6Dsl_State.sGyroBatchState.batchEnable = 1;
        }
        //special case when accel enabled before gyro
        if (lsm6Dsl_State.sXlBatchState.batchEnable)
        {
            lsm6Dsl_State.sXlBatchState.readIndex = lsm6Dsl_State.sXlBatchState.writeIndex = 0;
            lsm6Dsl_State.sXlBatchState.ui16BatchFillSizeInBytes = 0;
        }
    }
    if (SENSOR_TYPE_COMPOSITE_FFT == sensorType)
    {
        lsm6Dsl_State.compositeSensorOnAccelEnable = 1;
    }

    //FIFO setting when both gyro and accel are enabled
    fifoOdr = MAX(lsm6Dsl_State.ui16GyroOdr, lsm6Dsl_State.ui16XlOdr);

    //Both gyro and accel enabled.
    if ((lsm6Dsl_State.sGyroBatchState.batchEnable) && (lsm6Dsl_State.sXlBatchState.batchEnable))
    {
        gyroDecimation = fifoOdr / lsm6Dsl_State.ui16GyroOdr;
        accelDecimation = fifoOdr / lsm6Dsl_State.ui16XlOdr;
        lsm6Dsl_State.fifoContentType = GYRO_ACCEL;
    }
    else if ((gyroBatchInfo->ui16BatchSizeInBytes) && (lsm6Dsl_State.sGyroBatchState.batchEnable))  //only gyro enabled.
    {
        gyroDecimation = LSM6DSL_FIFO_XL_NO_DEC; //decimation factor 1
        accelDecimation = LSM6DSL_FIFO_GY_DISABLE;  //0  No accel data in fifo
        lsm6Dsl_State.fifoContentType = GYRO_ONLY;
    }
    else if ((accelBatchInfo->ui16BatchSizeInBytes) && (lsm6Dsl_State.sXlBatchState.batchEnable)) //only accel enabled.
    {
        gyroDecimation = LSM6DSL_FIFO_GY_DISABLE; //0 No gyro data into fifo
        accelDecimation = LSM6DSL_FIFO_XL_NO_DEC;  //decimation factor 1
        lsm6Dsl_State.fifoContentType = ACCEL_ONLY;
    }
    else if (lsm6Dsl_State.compositeSensorOnAccelEnable)
    {
        gyroDecimation = LSM6DSL_FIFO_GY_DISABLE; //0 No gyro data into fifo
        accelDecimation = LSM6DSL_FIFO_XL_NO_DEC;  //decimation factor 1
        lsm6Dsl_State.fifoContentType = ACCEL_ONLY;
    }

    //store fifo config into into lsm state variable.
    lsm6Dsl_State.gyroDecimation = gyroDecimation;
    lsm6Dsl_State.accelDecimation = accelDecimation;
    lsm6Dsl_State.fifoPatternSizeInWord16 = (gyroDecimation + accelDecimation) * LSM6DSL_SENSOR_DATA_SIZE;
#ifdef CONFIG_SENSOR_TIMESTAMP
    lsm6Dsl_State.fifoPatternSizeInWord16 += (MAX(gyroDecimation, accelDecimation)) * LSM6DSL_SENSOR_DATA_SIZE;//Time stamp is 6 bytes same as sensor data size.
#endif
    lsm6Dsl_State.fifoOdr = fifoOdr;
    TRACE("gyroDec = %d, accelDec = %d \n\r", gyroDecimation, accelDecimation);
    TRACE("fifoODR = %d, FIFO patternSize = %d\n\r", fifoOdr, lsm6Dsl_State.fifoPatternSizeInWord16);

    //Configure FIFO
    //FIFO disable (flush)
    result = i2c_write_reg(LSM6DSL_SLAVE_ADDR, LSM6DSL_REG_FIFO_CTRL5, LSM6DSL_BYPASS_MODE);
    if (result)
    {
        TRACE("Issue in disabling FIFO\n\r");
        return;
    }

    //set decimation factor
    tempVal = (gyroDecimation << LSM6DSL_SHIFT_FIFO_CTRL3_DEC_FIFO_GYRO) |
        (accelDecimation << LSM6DSL_SHIFT_FIFO_CTRL3_DEC_FIFO_XL);
    result = i2c_write_reg(LSM6DSL_SLAVE_ADDR, LSM6DSL_REG_FIFO_CTRL3, tempVal);
    if (result)
    {
        TRACE("Issue in setting decimation factors for accel and gyro (FIFO settings)\n\r");
        return;
    }
    //FIFO data rate set
    tempVal = (char)lsm6dsl_odr_to_val(fifoOdr);
    result = i2c_update_reg(LSM6DSL_SLAVE_ADDR, LSM6DSL_REG_FIFO_CTRL5,
        LSM6DSL_MASK_FIFO_CTRL5_ODR_FIFO, (tempVal << LSM6DSL_SHIFT_FIFO_CTRL5_ODR_FIFO));
    if (result)
    {
        TRACE("Issue in setting FIFO ODR\n\r");
        return;
    }

    //Optional setting for FIFO threshold, since there is no interrupt from sensor to DSP we are setting max possible threshold.
    lsm6Dsl_State.fifoTh = MAX_LSM6DSL_FIFO_SIZE;
    lsm6dsl_fifo_watermark_set(lsm6Dsl_State.fifoTh);

    //TRACE("gyroBatchSize = %d, accelBatchSize = %d\n\r", gyroBatchInfo->ui16BatchSizeInBytes, accelBatchInfo->ui16BatchSizeInBytes);

#ifdef CONFIG_SENSOR_TIMESTAMP
    i2c_update_reg(LSM6DSL_SLAVE_ADDR, LSM6DSL_REG_CTRL10_C,
        LSM6DSL_MASK_CTRL10_C_TIMER_EN, (1 << LSM6DSL_SHIFT_CTRL10_C_TIMER_EN));
    i2c_update_reg(LSM6DSL_SLAVE_ADDR, LSM6DSL_REG_FIFO_CTRL4,
        LSM6DSL_MASK_FIFO_CTRL4_DEC_DS4_FIFO, (LSM6DSL_FIFO_DS4_NO_DEC << LSM6DSL_SHIFT_FIFO_CTRL4_DEC_DS4_FIFO));

    i2c_update_reg(LSM6DSL_SLAVE_ADDR, LSM6DSL_REG_FIFO_CTRL2,
        LSM6DSL_MASK_FIFO_CTRL2_TIMER_PEDO_FIFO_EN, (1 << LSM6DSL_SHIFT_FIFO_CTRL2_TIMER_PEDO_FIFO_EN));
#endif

    //Enable FIFO in streaming mode.
    result = i2c_update_reg(LSM6DSL_SLAVE_ADDR, LSM6DSL_REG_FIFO_CTRL5, LSM6DSL_MASK_FIFO_CTRL5_FIFO_MODE,
        (LSM6DSL_STREAM_MODE << LSM6DSL_SHIFT_FIFO_CTRL5_FIFO_MODE));

    //Configure interrupts for fifo threshold and fifo full.
    //Since there are no interrupts from sensor to DSP, interrupts are not configured.

    if (lsm6Dsl_State.compositeSensorOnAccelEnable)
    {
        uint16_t timeout = 0;
        if (lsm6Dsl_State.ui16XlOdr <= 208)
            timeout = 1000;  //Assign max possible time out
        else
            timeout = (32000) / lsm6Dsl_State.ui16XlOdr * 8;  //timeout for 256 samples
        TRACE("Configuring composite sensor batch timer for timout period = %d\n\r", timeout);
        startTimer(lsm6Dsl_State.batchTimer, timeout, 1);  
		//start periodic timer
    }
    else
    {
        uint16_t timeout = 0;
        if (lsm6Dsl_State.fifoOdr <= 104)
            timeout = 1000;  //Assign max possible time out
        else
            //112 samples in worst case [accel+gyro+TS] will occupy approximately 1/2 fifo size.
            timeout = (16000 / fifoOdr) * 7;  //time out for 112 samples (2kWords == 1/2 fifo size) approximately.
        startTimer(lsm6Dsl_State.batchTimer, timeout, 1);  //start periodic timer
        TRACE("Configuring batch timer for timout period = %d\n\r", timeout);
    }

    return;
}

tLsm6DslStatus lsm6DslIoctl(tUnionSensorArg* arg)
{
    uint16_t val;
    uint16_t odr;

    tIOCTLArguments *ioctlArg = &(arg->ioctlArg);
    tLsm6DslStatus status = LSM6DSL_SUCCESS;
    TRACE("Processing IOCTL type = %d\n\r", ioctlArg->ioctlType);

    switch (ioctlArg->ioctlType)
    {
    case IOCTL_SDEV_SET_ODR:
        //val = *(uint16_t*)ioctlArg->vPtr;
        val = ioctlArg->ioctlUnionArg.odr;
        TRACE("IOCTL Set ODR val = %d\n\r", val);
        odr = findClosestIndex(odr_map, ARRAY_SIZE(odr_map), (uint16_t)val);
        if (ioctlArg->sensorType == SENSOR_TYPE_ACCELEROMETER)
        {
            status = lsm6dsl_accel_data_rate_set(&lsm6Dsl_State, odr);
        }
        else if (ioctlArg->sensorType == SENSOR_TYPE_GYROSCOPE)
        {
            status = lsm6dsl_gyro_data_rate_set(&lsm6Dsl_State, odr);
        }
        else
            status = LSM6DSL_FAIL;
        break;
    case IOCTL_SDEV_GET_ODR:
        if (ioctlArg->sensorType == SENSOR_TYPE_ACCELEROMETER)
        {
            ioctlArg->ioctlUnionArg.odr = lsm6Dsl_State.ui16XlOdr;
        }
        if (ioctlArg->sensorType == SENSOR_TYPE_GYROSCOPE)
        {
            ioctlArg->ioctlUnionArg.odr = lsm6Dsl_State.ui16GyroOdr;
        }
        status = LSM6DSL_SUCCESS;
        break;

    case IOCTL_SDEV_BATCH_BUFFER_INFO:
    {
        tSensorDspBufInfo* batch_dspBufInfo = &ioctlArg->ioctlUnionArg.batchBufInfo;;
        if (ioctlArg->sensorType == SENSOR_TYPE_ACCELEROMETER)
        {
            if (!batch_dspBufInfo->ui16BufLen)
            {
                status = LSM6DSL_FAIL;
            }
            else
            {
                lsm6Dsl_State.sXlBBinfo.ui16BaseOffset = batch_dspBufInfo->ui16BufOffset;
                lsm6Dsl_State.sXlBBinfo.ui16BatchSizeInBytes = batch_dspBufInfo->ui16BatchLen;
                lsm6Dsl_State.sXlBBinfo.ui16BufferSizeInBytes = batch_dspBufInfo->ui16BufLen;

                TRACE("Batch XLinfo: offset:0x%x, batchLen=0x%x, bufLen=0x%x\n\r",
                    batch_dspBufInfo->ui16BufOffset, batch_dspBufInfo->ui16BatchLen, batch_dspBufInfo->ui16BufLen);
            }
        }
        if (ioctlArg->sensorType == SENSOR_TYPE_GYROSCOPE)
        {
            if (!batch_dspBufInfo->ui16BufLen)
            {
                status = LSM6DSL_FAIL;
            }
            else
            {
                lsm6Dsl_State.sGyroBBinfo.ui16BaseOffset = batch_dspBufInfo->ui16BufOffset;
                lsm6Dsl_State.sGyroBBinfo.ui16BatchSizeInBytes = batch_dspBufInfo->ui16BatchLen;
                lsm6Dsl_State.sGyroBBinfo.ui16BufferSizeInBytes = batch_dspBufInfo->ui16BufLen;
                TRACE("Batch gyroinfo: offset:0x%x, batchLen=0x%x, bufLen=0x%x\n\r",
                    batch_dspBufInfo->ui16BufOffset, batch_dspBufInfo->ui16BatchLen, batch_dspBufInfo->ui16BufLen);
            }
        }
        if (INVALID_TIMER_HANDLE == lsm6Dsl_State.batchTimer)
        {
            lsm6Dsl_State.batchTimer = createTimer(LsmSensorReadCb, (void*)SENSOR_OP_BATCH_READ);
        }
        break;
    }
    case IOCTL_SDEV_BATCH_ENABLE:
        lsm6dsl_batch_enable(ioctlArg->sensorType);
        break;
    case IOCTL_SDEV_LIVE_DATA_ENABLE:
    {
        tSensorLiveDspBufInfo* live_info;
        live_info = &ioctlArg->ioctlUnionArg.liveBufInfo;
        if (ioctlArg->sensorType == SENSOR_TYPE_ACCELEROMETER)
        {
            if (!live_info->ui16SizeInBytes)
            {
                lsm6Dsl_State.sXlLBinfo.liveDataEnable = 0;
                status = LSM6DSL_FAIL;
            }
            else
            {
                TRACE("Processing accel live data enable:\n\r");
                lsm6Dsl_State.sXlLBinfo.ui16PPBufOffset = live_info->u16PingPongBufOffset;
                lsm6Dsl_State.sXlLBinfo.sampleSizeInBytes = live_info->ui16SizeInBytes / 2;  //ping pong buffer is twice sample size
                lsm6Dsl_State.sXlLBinfo.liveDataEnable = 1;

                //start timer for accel data read.
                TRACE("Configuring accel timer for timout period = %d\n\r", lsm6Dsl_State.ui16XlPeriodMs);
                startTimer(lsm6Dsl_State.accelTimer, lsm6Dsl_State.ui16XlPeriodMs, 1);  //start periodic timer
            }
        }
        if (ioctlArg->sensorType == SENSOR_TYPE_GYROSCOPE)
        {
            if ((!live_info->ui16SizeInBytes))
            {
                status = LSM6DSL_FAIL;
                lsm6Dsl_State.sGyroLBinfo.liveDataEnable = 0;
            }
            else
            {
                TRACE("Processing gyro live data enable:\n\r");
                lsm6Dsl_State.sGyroLBinfo.ui16PPBufOffset = live_info->u16PingPongBufOffset;
                lsm6Dsl_State.sGyroLBinfo.sampleSizeInBytes = live_info->ui16SizeInBytes / 2;
                lsm6Dsl_State.sGyroLBinfo.liveDataEnable = 1;

                //start timer for accel data read.
                TRACE("Configuring gyro timer for timout period = %d\n\r", lsm6Dsl_State.ui16GyroPeriodMs);
                startTimer(lsm6Dsl_State.gyroTimer, lsm6Dsl_State.ui16GyroPeriodMs, 1); //start periodic timer
            }
        }
        break;
    }

    case IOCTL_SDEV_SET_FULL_SCALE:
        val = ioctlArg->ioctlUnionArg.fullScale;

        if (ioctlArg->sensorType == SENSOR_TYPE_ACCELEROMETER)
        {
            TRACE("Setting IOCTL FULL SCALE for Accel val = %d\n\r", val);
            status = lsm6dsl_accel_fs_set(val);
        }
        else if (ioctlArg->sensorType == SENSOR_TYPE_GYROSCOPE)
        {
            TRACE("Setting IOCTL FULL SCALE for Gyro val = %d\n\r", val);
            status = lsm6dsl_gy_fs_set(&lsm6Dsl_State, val);
        }
        //else
        //    status = LSM6DSL_FAIL;
        break;

    case IOCTL_SDEV_BATCH_FLUSH:
        if (ioctlArg->sensorType == SENSOR_TYPE_ACCELEROMETER)
        {
            lsm6Dsl_State.sXlBatchState.readIndex = lsm6Dsl_State.sXlBatchState.writeIndex = 0;
            lsm6Dsl_State.sXlBatchState.ui16BatchFillSizeInBytes = 0;
            TRACE("Flush done for XL:\n\r");
        }
        else if (ioctlArg->sensorType == SENSOR_TYPE_GYROSCOPE)
        {
            //Reset batching info
            lsm6Dsl_State.sGyroBatchState.readIndex = lsm6Dsl_State.sGyroBatchState.writeIndex = 0;
            lsm6Dsl_State.sGyroBatchState.ui16BatchFillSizeInBytes = 0;
            TRACE("Flush done for Gyro:\n\r");
        }
        else
        {
            TRACE("Error: Invalid sensorType for Flush command:\n\r");
        }
        break;

    case IOCTL_SDEV_BATCH_DISABLE:
        if (ioctlArg->sensorType == SENSOR_TYPE_ACCELEROMETER)
        {
            lsm6Dsl_State.sXlBatchState.readIndex = lsm6Dsl_State.sXlBatchState.writeIndex = 0;
            lsm6Dsl_State.sXlBatchState.ui16BatchFillSizeInBytes = 0;
            lsm6Dsl_State.sXlBatchState.batchEnable = 0;
            lsm6Dsl_State.sXlBBinfo.ui16BaseOffset = 0;
            lsm6Dsl_State.sXlBBinfo.ui16BatchSizeInBytes = 0;
            lsm6Dsl_State.ui16XlOdr = 0;

            if (lsm6Dsl_State.sGyroBatchState.batchEnable)
            {
                //Re-enable sensor fifo for gyro only.
                lsm6dsl_batch_enable(SENSOR_TYPE_GYROSCOPE);
            }
            TRACE("Accel Batch Disabled:\n\r");
        }
        if (ioctlArg->sensorType == SENSOR_TYPE_GYROSCOPE)
        {
            //Reset batching info
            lsm6Dsl_State.sGyroBatchState.readIndex = lsm6Dsl_State.sGyroBatchState.writeIndex = 0;
            lsm6Dsl_State.sGyroBatchState.ui16BatchFillSizeInBytes = 0;
            lsm6Dsl_State.sGyroBatchState.batchEnable = 0;
            lsm6Dsl_State.sGyroBBinfo.ui16BaseOffset = 0;
            lsm6Dsl_State.sGyroBBinfo.ui16BatchSizeInBytes = 0;
            lsm6Dsl_State.ui16GyroOdr = 0;

            if (lsm6Dsl_State.sXlBatchState.batchEnable)
            {
                //re-enable sensor fifo for accel only
                lsm6dsl_batch_enable(SENSOR_TYPE_ACCELEROMETER);
            }
            TRACE("Gyro Batch Disabled:\n\r");
        }
        if ((0 == lsm6Dsl_State.sGyroBatchState.batchEnable) && (0 == lsm6Dsl_State.sXlBatchState.batchEnable))
        {
            //when both accel and gyro batches are disabled then stop batch timer.
            stopTimer(lsm6Dsl_State.batchTimer);
        }

        break;

    case IOCTL_SDEV_LIVE_DATA_DISABLE:
        //live buffer info
        if (ioctlArg->sensorType == SENSOR_TYPE_ACCELEROMETER)
        {
            TRACE("Reseting live buffer info for accel:\n\r");
            lsm6Dsl_State.sXlLBinfo.ui16PPBufOffset = 0;
            lsm6Dsl_State.sXlLBinfo.sampleSizeInBytes = 0;
            lsm6Dsl_State.sXlLBinfo.liveDataEnable = 0;
            TRACE("Accel live data Disabled:\n\r");
            stopTimer(lsm6Dsl_State.accelTimer);
        }
        else if (ioctlArg->sensorType == SENSOR_TYPE_GYROSCOPE)
        {
            TRACE("Reseting live buffer info for gyro:\n\r");
            lsm6Dsl_State.sGyroLBinfo.ui16PPBufOffset = 0;
            lsm6Dsl_State.sGyroLBinfo.sampleSizeInBytes = 0;
            lsm6Dsl_State.sGyroLBinfo.liveDataEnable = 0;
            TRACE("gyro live data Disabled:\n\r");
            stopTimer(lsm6Dsl_State.gyroTimer);
        }
        else
        {
            TRACE("Reseting live buffer info failed:\n\r");
            status = LSM6DSL_FAIL;
        }
        break;

    default:
        TRACE("Unsupported IOCTL:\n\r", ioctlArg->ioctlType);
        status = LSM6DSL_FAIL;
        break;
    };

    arg->ioctlArg.status = status;
    return status;
}

tLsm6DslStatus lsm6DslClose(tUnionSensorArg* arg)
{
    lsm6Dsl_State.uiOpenCnt--;
    arg->closeArg.status = LSM6DSL_SUCCESS;
    return arg->closeArg.status;
}

#if 0
//This function checks Status Reg (1Eh) if new accel or gyro data is available.
//If new data available, prepare response and send it to M3 via mail box.
void pollForLiveSensorData(void)
{
    uint16_t accelBaseOffset;
    uint16_t gyroBaseOffset;
    uint16_t accelSampleSizeInBytes;
    uint16_t gyroSampleSizeInBytes;

    uint16_t tempSensorBuf[LSM6DSL_MAX_LIVE_SENSOR_BUFFER_SIZE/2];
    volatile uint16_t chess_storage(IOMEM)* tempIoMemBuffer;
    uint16_t offset;
    static uint16_t accelPP = 0;
    static uint16_t gyroPP = 0;

    char statusRegVal;

    if ((0 == lsm6Dsl_State.sXlLBinfo.liveDataEnable) && (0 == lsm6Dsl_State.sGyroLBinfo.liveDataEnable))
    {
        return;
    }

    accelBaseOffset = lsm6Dsl_State.sXlLBinfo.ui16PPBufOffset;
    accelSampleSizeInBytes = lsm6Dsl_State.sXlLBinfo.sampleSizeInBytes;

    gyroBaseOffset = lsm6Dsl_State.sGyroLBinfo.ui16PPBufOffset;
    gyroSampleSizeInBytes = lsm6Dsl_State.sGyroLBinfo.sampleSizeInBytes;

    if (i2c_read_reg(LSM6DSL_SLAVE_ADDR, LSM6DSL_REG_STATUS_REG, &statusRegVal, 1))
    {
        TRACE("Issue in reading status register\r\n");
        return;
    }

    if (0 == statusRegVal)
    {
        return;  //Sensor data is not ready.
    }


    if (lsm6Dsl_State.sXlLBinfo.liveDataEnable)
    {
        //Poll for accel live data.
        if ((accelSampleSizeInBytes) && (statusRegVal & LSM6DSL_MASK_STATUS_REG_XLDA))
        {

            if (i2c_read_reg(LSM6DSL_SLAVE_ADDR, LSM6DSL_REG_OUTX_L_XL, (char*)tempSensorBuf, accelSampleSizeInBytes))
            {
                TRACE("Issue in reading accel live data\r\n");
                return;
            }
            offset = accelBaseOffset;
            if (accelPP)
            {
                offset += accelSampleSizeInBytes;
            }
            tempIoMemBuffer = (uint16_t chess_storage(IOMEM)*)getSharedMemoryAddress(offset);

            //copy sensor data from local x memory to iomem shared memory
            TRACE("adress=0x%x, Az=%d\n\r", tempIoMemBuffer, tempSensorBuf[2]);
            memcpy_xmem_to_iomem((void chess_storage(IOMEM)*)tempIoMemBuffer, (void*)tempSensorBuf, (uint16_t)accelSampleSizeInBytes);

            sendSensorEventData((uint8_t)LSM6DSL_SENSOR_ID, (uint8_t)SENSOR_TYPE_ACCELEROMETER, (uint16_t)SENSOR_EVENT_LIVE_DATA,
                (uint16_t)offset, (uint16_t)accelSampleSizeInBytes);

            accelPP = (accelPP + 1) & 1;
        }
    }

    //Poll for gyro live data.
    if (lsm6Dsl_State.sGyroLBinfo.liveDataEnable)
    {
        if ((gyroSampleSizeInBytes) && (statusRegVal & LSM6DSL_MASK_STATUS_REG_GDA))
        {
            if (i2c_read_reg(LSM6DSL_SLAVE_ADDR, LSM6DSL_REG_OUTX_L_G, (char*)tempSensorBuf, gyroSampleSizeInBytes))
            {
                TRACE("Issue in reading gyro live data\r\n");
                return;
            }
            offset = gyroBaseOffset;
            if (gyroPP)
            {
                offset += gyroSampleSizeInBytes;
            }
            tempIoMemBuffer = (uint16_t chess_storage(IOMEM)*)getSharedMemoryAddress(offset);
            //copy sensor data from local x memory to iomem shared memory
            TRACE("adress=0x%x,                         Gz=%d\n\r", tempIoMemBuffer, tempSensorBuf[2]);
            memcpy_xmem_to_iomem((void chess_storage(IOMEM)*)tempIoMemBuffer, (void*)tempSensorBuf, (uint16_t)gyroSampleSizeInBytes);

            sendSensorEventData((uint8_t)LSM6DSL_SENSOR_ID, (uint8_t)SENSOR_TYPE_GYROSCOPE, (uint16_t)SENSOR_EVENT_LIVE_DATA,
                (uint16_t)(offset), (uint16_t)gyroSampleSizeInBytes);
            gyroPP = (gyroPP + 1) & 1;  //Alternate ping pong index between 0 and 1
        }
    }
}
#endif


static int lsm6dsl_fifo_data_level_get(uint16_t* ui16FifoCount)
{
    int result;
    uint8_t val1;
    uint8_t val2;
    result = i2c_read_reg(LSM6DSL_SLAVE_ADDR, LSM6DSL_REG_FIFO_STATUS1, (char*)(&val1), 1);
    if (result)
    {
        TRACE("Issue in reading fifo count (fifo_status1)\n\r");
        return -1;
    }
    result = i2c_read_reg(LSM6DSL_SLAVE_ADDR, LSM6DSL_REG_FIFO_STATUS2, (char*)(&val2), 1);
    if (result)
    {
        TRACE("Issue in reading fifo count (fifo_status2)\n\r");
        return -1;
    }

    *ui16FifoCount = (((uint16_t)(val2 & LSM6DSL_MASK_FIFO_STATUS2_DIFF_FIFO)) << 8) | (uint16_t)val1;

    return 0;
}


static void consumeGyroBatchData(void* tempSensorBuffer, uint16_t numBytesRead)
{
    tSensorBatchBufInfo* gyroInfo = &lsm6Dsl_State.sGyroBBinfo;
    tBatchState* sGyroBatchState = &lsm6Dsl_State.sGyroBatchState;
    //copy sensor data from local xmem to IOMEM
    if (sGyroBatchState->batchEnable)
    {
        volatile uint16_t chess_storage(IOMEM)* tempIoMemBuffer;
        tempIoMemBuffer = (uint16_t chess_storage(IOMEM)*)getSharedMemoryAddress(gyroInfo->ui16BaseOffset + sGyroBatchState->writeIndex);
        memcpy_xmem_to_iomem((void chess_storage(IOMEM)*)tempIoMemBuffer, (void*)tempSensorBuffer, (uint16_t)numBytesRead);

        sGyroBatchState->writeIndex += numBytesRead;
        sGyroBatchState->ui16BatchFillSizeInBytes += numBytesRead;

        if (sGyroBatchState->writeIndex >= gyroInfo->ui16BufferSizeInBytes)
        {
            sGyroBatchState->writeIndex = 0; //modulo wrap around
        }
        if (sGyroBatchState->ui16BatchFillSizeInBytes >= gyroInfo->ui16BatchSizeInBytes)  //One batch of samples completed.
        {
            TRACE("!!!!!!BATCH evt details: gyro\n\r");
            TRACE("wptr=%x, rptr=%x:\n\r", sGyroBatchState->writeIndex, sGyroBatchState->readIndex);
            sendSensorEventData((uint8_t)LSM6DSL_SENSOR_ID, (uint8_t)SENSOR_TYPE_GYROSCOPE, (uint16_t)SENSOR_EVENT_BATCH,
                (uint16_t)(gyroInfo->ui16BaseOffset + sGyroBatchState->readIndex), (uint16_t)sGyroBatchState->ui16BatchFillSizeInBytes);

            sGyroBatchState->ui16BatchFillSizeInBytes = 0;
            sGyroBatchState->readIndex = sGyroBatchState->writeIndex;
        }
    }//if (sGyroBatchState->batchEnable)
}

static void consumeGyroData(void *tempSensorBuffer, uint16_t numBytesRead)
{
    consumeGyroBatchData(tempSensorBuffer, numBytesRead);
}
static void extractFifoDataGyroOnly(uint16_t ui16Count)
{
    uint16_t tempSensorBuffer[(LSM6DSL_MAX_SENSOR_SAMPLE_SIZE + LSM_TS_LENGTH)/2];
    uint16_t numBytesRead = 0;
#ifdef CONFIG_SENSOR_TIMESTAMP
    uint16_t dummyBytes;
#endif
    TRACE("fifo data Gyro wordCount = %d\n\r", ui16Count);
    while (ui16Count > 0)
    {
        i2c_read_reg(LSM6DSL_SLAVE_ADDR, LSM6DSL_REG_FIFO_DATA_OUT_L, (char*)(&tempSensorBuffer[0]), 2);
        i2c_read_reg(LSM6DSL_SLAVE_ADDR, LSM6DSL_REG_FIFO_DATA_OUT_L, (char*)(&tempSensorBuffer[1]), 2);
        i2c_read_reg(LSM6DSL_SLAVE_ADDR, LSM6DSL_REG_FIFO_DATA_OUT_L, (char*)(&tempSensorBuffer[2]), 2);
        ui16Count -= 3;  //ui16Count is interms of word16
        numBytesRead = 6;
#ifdef CONFIG_SENSOR_TIMESTAMP
        //Out of 6 bytes, Only 4 bytes are time stamp. 
        //Other two bytes are related to Step count and hence it is read from FIFO and discared.
        //time stamp is in a kind of big endian format hence order needs to be swapped.
        i2c_read_reg(LSM6DSL_SLAVE_ADDR, LSM6DSL_REG_FIFO_DATA_OUT_L, (char*)(&tempSensorBuffer[4]), 2); //MSW
        i2c_read_reg(LSM6DSL_SLAVE_ADDR, LSM6DSL_REG_FIFO_DATA_OUT_L, (char*)(&tempSensorBuffer[3]), 2); //LSW
        tempSensorBuffer[3] = tempSensorBuffer[3] >> 8;  //extract 8 bit MSB of timestamp.
        //step count bytes are read in to dummyBytes and discarded.
        i2c_read_reg(LSM6DSL_SLAVE_ADDR, LSM6DSL_REG_FIFO_DATA_OUT_L, (char*)&dummyBytes, 2);
        ui16Count -= 3;  //ui16Count is interms of word16
        numBytesRead += 4;
#endif

        consumeGyroData(tempSensorBuffer, numBytesRead);
    }
}

//copy sensor data from local xmem to IOMEM
static void consumeAccelBatchData(void* tempSensorBuffer, uint16_t numBytesRead)
{
    tSensorBatchBufInfo* accelInfo = &lsm6Dsl_State.sXlBBinfo;
    tBatchState* sXlBatchState = &lsm6Dsl_State.sXlBatchState;

    if (sXlBatchState->batchEnable)
    {
        volatile uint16_t chess_storage(IOMEM)* tempIoMemBuffer;
        tempIoMemBuffer = (uint16_t chess_storage(IOMEM)*)getSharedMemoryAddress(accelInfo->ui16BaseOffset + sXlBatchState->writeIndex);
        memcpy_xmem_to_iomem((void chess_storage(IOMEM)*)tempIoMemBuffer, (void*)tempSensorBuffer, (uint16_t)numBytesRead);

        sXlBatchState->writeIndex += numBytesRead;
        sXlBatchState->ui16BatchFillSizeInBytes += numBytesRead;

        if (sXlBatchState->writeIndex >= accelInfo->ui16BufferSizeInBytes)
        {
            sXlBatchState->writeIndex = 0; //modulo wrap around
        }
        if (sXlBatchState->ui16BatchFillSizeInBytes >= accelInfo->ui16BatchSizeInBytes)  //One batch of samples completed.
        {
            TRACE("!!!!!!BATCH evt details: accel\n\r");
            TRACE("wptr=%x, rptr=%x:\n\r", sXlBatchState->writeIndex, sXlBatchState->readIndex);
            sendSensorEventData((uint8_t)LSM6DSL_SENSOR_ID, (uint8_t)SENSOR_TYPE_ACCELEROMETER, (uint16_t)SENSOR_EVENT_BATCH,
                (uint16_t)(accelInfo->ui16BaseOffset + sXlBatchState->readIndex), (uint16_t)sXlBatchState->ui16BatchFillSizeInBytes);

            sXlBatchState->ui16BatchFillSizeInBytes = 0;
            sXlBatchState->readIndex = sXlBatchState->writeIndex;
        }
    }//if (sXlBatchState->batchEnable)
}

static void consumeAccelData(void *tempSensorBuffer, uint16_t numBytesRead)
{
    int i;

    consumeAccelBatchData(tempSensorBuffer, numBytesRead);
    //Call all registered callbacks
    for (i = 0; i < NOTIFY_XL_FN_LIST_SIZE; i++)
    {
        if (notifyAccelFnList[i])
            notifyAccelFnList[i](tempSensorBuffer, numBytesRead);
    }
}

static void extractFifoDataAccelOnly(uint16_t ui16Count)
{
    uint16_t tempSensorBuffer[(LSM6DSL_MAX_SENSOR_SAMPLE_SIZE + LSM_TS_LENGTH)/2];
    uint16_t numBytesRead = 0;
#ifdef CONFIG_SENSOR_TIMESTAMP
    uint16_t dummyBytes;
#endif
    TRACE("fifo data xl wordCount = %d\n\r", ui16Count);
    while (ui16Count > 0)
    {
        i2c_read_reg(LSM6DSL_SLAVE_ADDR, LSM6DSL_REG_FIFO_DATA_OUT_L, (char*)(&tempSensorBuffer[0]), 2);
        i2c_read_reg(LSM6DSL_SLAVE_ADDR, LSM6DSL_REG_FIFO_DATA_OUT_L, (char*)(&tempSensorBuffer[1]), 2);
        i2c_read_reg(LSM6DSL_SLAVE_ADDR, LSM6DSL_REG_FIFO_DATA_OUT_L, (char*)(&tempSensorBuffer[2]), 2);
        ui16Count -= 3;  //ui16Count is interms of word16
        numBytesRead = 6;
#ifdef CONFIG_SENSOR_TIMESTAMP
        //Out of 6 bytes, Only 4 bytes are time stamp. 
        //Other two bytes are related to Step count and hence it is read from FIFO and discared.
        //time stamp is in a kind of big endian format hence order needs to be swapped.
        i2c_read_reg(LSM6DSL_SLAVE_ADDR, LSM6DSL_REG_FIFO_DATA_OUT_L, (char*)(&tempSensorBuffer[4]), 2);
        i2c_read_reg(LSM6DSL_SLAVE_ADDR, LSM6DSL_REG_FIFO_DATA_OUT_L, (char*)(&tempSensorBuffer[3]), 2);
        tempSensorBuffer[3] = tempSensorBuffer[3] >> 8;  //Extract MSB of time stamp.
        //step count bytes are read in to dummyBytes and discarded.
        i2c_read_reg(LSM6DSL_SLAVE_ADDR, LSM6DSL_REG_FIFO_DATA_OUT_L, (char*)&dummyBytes, 2);
        ui16Count -= 3;  //ui16Count is interms of word16
        numBytesRead += 4;
#endif

        consumeAccelData(tempSensorBuffer, numBytesRead);
    }
}

static void extractFifoDataGyroAccel(uint16_t ui16Count)
{
    uint16_t tempSensorBufferGyro[(LSM6DSL_MAX_SENSOR_SAMPLE_SIZE + LSM_TS_LENGTH) / 2];
    uint16_t tempSensorBufferAccel[(LSM6DSL_MAX_SENSOR_SAMPLE_SIZE + LSM_TS_LENGTH) / 2];

    uint16_t numBytesGyro = 0;
    uint16_t gyroPhase = 0;

    uint16_t numBytesAccel = 0;
    uint16_t accelPhase = 0;

#ifdef CONFIG_SENSOR_TIMESTAMP
    uint16_t timeStamp[2];
    uint16_t dummyBytes;
#endif

    TRACE("fifo data GyroAccel wordCount = %d\n\r", ui16Count);

    while (ui16Count > 0)
    {
        //gyro phase will vary from 0 to gyroDecimation
        if (0 == gyroPhase)
        {
            //extract gyro samples
            i2c_read_reg(LSM6DSL_SLAVE_ADDR, LSM6DSL_REG_FIFO_DATA_OUT_L, (char*)(&tempSensorBufferGyro[0]), 2);
            i2c_read_reg(LSM6DSL_SLAVE_ADDR, LSM6DSL_REG_FIFO_DATA_OUT_L, (char*)(&tempSensorBufferGyro[1]), 2);
            i2c_read_reg(LSM6DSL_SLAVE_ADDR, LSM6DSL_REG_FIFO_DATA_OUT_L, (char*)(&tempSensorBufferGyro[2]), 2);

            ui16Count -= 3;  //ui16Count is interms of word16
            numBytesGyro = 6;
        }

        //accel phase will vary from 0 to accelDecimation
        if (0 == accelPhase)
        {
            //extract accel samples
            i2c_read_reg(LSM6DSL_SLAVE_ADDR, LSM6DSL_REG_FIFO_DATA_OUT_L, (char*)(&tempSensorBufferAccel[0]), 2);
            i2c_read_reg(LSM6DSL_SLAVE_ADDR, LSM6DSL_REG_FIFO_DATA_OUT_L, (char*)(&tempSensorBufferAccel[1]), 2);
            i2c_read_reg(LSM6DSL_SLAVE_ADDR, LSM6DSL_REG_FIFO_DATA_OUT_L, (char*)(&tempSensorBufferAccel[2]), 2);

            ui16Count -= 3;  //ui16Count is interms of word16
            numBytesAccel = 6;
        }

#ifdef CONFIG_SENSOR_TIMESTAMP
        //time stamp is present for every cycle.
        //Out of 6 bytes, Only 4 bytes are time stamp. 
        //Other two bytes are related to Step count and hence it is read from FIFO and discared.
        //time stamp is in a kind of big endian format hence order needs to be swapped.
        i2c_read_reg(LSM6DSL_SLAVE_ADDR, LSM6DSL_REG_FIFO_DATA_OUT_L, (char*)(&timeStamp[1]), 2);
        i2c_read_reg(LSM6DSL_SLAVE_ADDR, LSM6DSL_REG_FIFO_DATA_OUT_L, (char*)(&timeStamp[0]), 2);
        timeStamp[0] = timeStamp[0] >> 8; //Extract MSB of timestamp.
        //step count bytes are read in to dummyBytes and discarded.
        i2c_read_reg(LSM6DSL_SLAVE_ADDR, LSM6DSL_REG_FIFO_DATA_OUT_L, (char*)&dummyBytes, 2);
        ui16Count -= 3;  //ui16Count is interms of word16

        //copy time stamp to gyro and accel buffer when decimation phase is 0.
        if (0 == gyroPhase)  //append time stamp after sensor data.
        {
            memcpy_xmem_to_xmem((void*)&tempSensorBufferGyro[3], (void*)&timeStamp[0], 4);
            numBytesGyro += 4;
        }
        if (0 == accelPhase)  //append time stamp after sensor data.
        {
            memcpy_xmem_to_xmem((void*)&tempSensorBufferAccel[3], (void*)&timeStamp[0], 4);
            numBytesAccel += 4;
        }
#endif
        if (0 == gyroPhase)
        {
            consumeGyroData(tempSensorBufferGyro, numBytesGyro);
        } //if (0 == gyroPhase)

        if (0 == accelPhase)
        {
            consumeAccelData(tempSensorBufferAccel, numBytesAccel);
        }  //if (0 == AccelPhase)

        //update phase for accel and gyro decimation
        gyroPhase++;
        if (gyroPhase >= lsm6Dsl_State.gyroDecimation)  //for decimation factor 1, gyro is present for every cycle.
            gyroPhase = 0;
        accelPhase++;
        if (accelPhase >= lsm6Dsl_State.accelDecimation)  //for decimation factor 1, accel is present for every cycle.
            accelPhase = 0;
    }//while (ui16Count > 0)
}


static bool checkIfSensorFifoEnabled(void)
{
    if ((0 == lsm6Dsl_State.sGyroBatchState.batchEnable) && (0 == lsm6Dsl_State.sXlBatchState.batchEnable)
        && (0 == lsm6Dsl_State.compositeSensorOnAccelEnable))
        return 0;
    else
        return 1;
}

//Poll FIFO counter to check if there is enough data available and fill batch buffer.
void pollForBatchSensorData(void)
{
    uint16_t ui16Count = 0;
    if ((0 == checkIfSensorFifoEnabled()) || (0 == lsm6Dsl_State.fifoOdr))
        return;

    if (lsm6dsl_fifo_data_level_get(&ui16Count))
        return;
    //To extact sensor data from fifo, make sure it has at least one frame worth of fifo data repeating pattern.
    if (ui16Count < lsm6Dsl_State.fifoPatternSizeInWord16)
        return;

    //Align ui16Count to pattern boundary.
    ui16Count = (ui16Count / lsm6Dsl_State.fifoPatternSizeInWord16) * (lsm6Dsl_State.fifoPatternSizeInWord16);
    switch (lsm6Dsl_State.fifoContentType)
    {
    case GYRO_ONLY:
        extractFifoDataGyroOnly(ui16Count);
        break;
    case ACCEL_ONLY:
        extractFifoDataAccelOnly(ui16Count);
        break;
    case GYRO_ACCEL:
        extractFifoDataGyroAccel(ui16Count);
        break;
    }
    return;
}

extern tMsgQueue     dspMsgQ;
extern uint8_t sensorTaskId;

void LsmSensorReadCb(void* lsmReadType)
{
    tdspLocalMsg msg;
    uint16_t sensorReadType;
    sensorReadType = (uint16_t)lsmReadType;
    //msg.msgId = SENSOR_OP_ACCEL_ASYNC_READ;
    TRACE("In LsmSensorReadCb()\n\r");
    switch (sensorReadType)
    {
    case SENSOR_OP_ACCEL_ASYNC_READ:
        if (i2c_read_reg(LSM6DSL_SLAVE_ADDR, LSM6DSL_REG_OUTX_L_XL, (char*)AccelSensorBuf, LSM6DSL_MAX_LIVE_SENSOR_BUFFER_SIZE))
        {
            TRACE("Issue in reading accel live data\r\n");
            return;
        }
        //TRACE("Accel Data Read Accel[2]=%d\n\r", AccelSensorBuf);

        msg.msgId = SENSOR_OP_ACCEL_ASYNC_READ;
        break;
    case SENSOR_OP_GYRO_ASYNC_READ:
        if (i2c_read_reg(LSM6DSL_SLAVE_ADDR, LSM6DSL_REG_OUTX_L_G, (char*)GyroSensorBuf, LSM6DSL_MAX_LIVE_SENSOR_BUFFER_SIZE))
        {
            TRACE("Issue in reading gyro live data\r\n");
            return;
        }
        msg.msgId = SENSOR_OP_GYRO_ASYNC_READ;

        break;

    case SENSOR_OP_BATCH_READ:
        msg.msgId = SENSOR_OP_BATCH_READ;
        msg.data[0] = LSM6DSL_SENSOR_ID;

        TRACE("Accel async batch Read:\n\r");
        break;

#if (CONFIG_LSM6DSL_FFT_DSP == 1)	
    case SENSOR_OP_FFT_BATCH_READ:
        msg.msgId = SENSOR_OP_FFT_BATCH_READ;
        msg.data[0] = LSM6DSL_FFT_SENSOR_ID;

        TRACE("Accel async FFT batch Read:\n\r");
        break;
#endif	

    default:
        TRACE("Issue in LsmSensorReadCb sensorType=%d\n\r", sensorReadType);
        break;
    }
    msg.taskId = sensorTaskId;
    MsgQueueAdd(&dspMsgQ, &msg);
    //TRACE("message from ISR added to local queue\n\r");
    //TRACE("local mesg added:::msg->taskId=%d, msg->msgId=%d\n\r", msg.taskId, msg.msgId);
}

void ProcessLsmSensorLocalMsg(tdspLocalMsg* msg)
{
    uint16_t accelBaseOffset;
    uint16_t gyroBaseOffset;
    uint16_t accelSampleSizeInBytes;
    uint16_t gyroSampleSizeInBytes;

    volatile uint16_t chess_storage(IOMEM)* tempIoMemBuffer;
    uint16_t offset;
    static uint16_t accelPP = 0;
    static uint16_t gyroPP = 0;

    char statusRegVal;


    TRACE("In ProcessLsmSensorLocalMsg, msgId=%d\n\r", msg->msgId);
    if ((SENSOR_OP_BATCH_READ == msg->msgId)  || (SENSOR_OP_FFT_BATCH_READ == msg->msgId))
    {
        TRACE("Polling batch sensor data\n\r");
        pollForBatchSensorData();
        return;
    }
    accelBaseOffset = lsm6Dsl_State.sXlLBinfo.ui16PPBufOffset;
    accelSampleSizeInBytes = lsm6Dsl_State.sXlLBinfo.sampleSizeInBytes;

    gyroBaseOffset = lsm6Dsl_State.sGyroLBinfo.ui16PPBufOffset;
    gyroSampleSizeInBytes = lsm6Dsl_State.sGyroLBinfo.sampleSizeInBytes;

#if 0
    if (i2c_read_reg(LSM6DSL_SLAVE_ADDR, LSM6DSL_REG_STATUS_REG, &statusRegVal, 1))
    {
        TRACE("Issue in reading status register\r\n");
        return;
    }

    if (0 == statusRegVal)
    {
        return;  //Sensor data is not ready.
    }
#endif

    if (SENSOR_OP_ACCEL_ASYNC_READ == msg->msgId)
    {
        if (accelSampleSizeInBytes)
        {
#if 0
            if (i2c_read_reg(LSM6DSL_SLAVE_ADDR, LSM6DSL_REG_OUTX_L_XL, (char*)tempSensorBuf, accelSampleSizeInBytes))
            {
                TRACE("Issue in reading accel live data\r\n");
                return;
            }
#endif
            offset = accelBaseOffset;
            if (accelPP)
            {
                offset += accelSampleSizeInBytes;
            }
            tempIoMemBuffer = (uint16_t chess_storage(IOMEM)*)getSharedMemoryAddress(offset);

            //copy sensor data from local x memory to iomem shared memory
            TRACE("adress=0x%x, Az=%d\n\r", tempIoMemBuffer, AccelSensorBuf[2]);
            memcpy_xmem_to_iomem((void chess_storage(IOMEM)*)tempIoMemBuffer, (void*)AccelSensorBuf, (uint16_t)accelSampleSizeInBytes);

            sendSensorEventData((uint8_t)LSM6DSL_SENSOR_ID, (uint8_t)SENSOR_TYPE_ACCELEROMETER, (uint16_t)SENSOR_EVENT_LIVE_DATA,
                (uint16_t)offset, (uint16_t)accelSampleSizeInBytes);
            //startTimer(lsm6Dsl_State.accelTimer, lsm6Dsl_State.ui16XlPeriodMs);  //TBD
            accelPP = (accelPP + 1) & 1;
        }
    }

    //Poll for gyro live data.
    if (SENSOR_OP_GYRO_ASYNC_READ == msg->msgId)
    {
        if (gyroSampleSizeInBytes)
        {
#if 0
            if (i2c_read_reg(LSM6DSL_SLAVE_ADDR, LSM6DSL_REG_OUTX_L_G, (char*)tempSensorBuf, gyroSampleSizeInBytes))
            {
                TRACE("Issue in reading gyro live data\r\n");
                return;
            }
#endif
            offset = gyroBaseOffset;
            if (gyroPP)
            {
                offset += gyroSampleSizeInBytes;
            }
            tempIoMemBuffer = (uint16_t chess_storage(IOMEM)*)getSharedMemoryAddress(offset);
            //copy sensor data from local x memory to iomem shared memory
            TRACE("adress=0x%x,                         Gz=%d\n\r", tempIoMemBuffer, GyroSensorBuf[2]);
            memcpy_xmem_to_iomem((void chess_storage(IOMEM)*)tempIoMemBuffer, (void*)GyroSensorBuf, (uint16_t)gyroSampleSizeInBytes);

            sendSensorEventData((uint8_t)LSM6DSL_SENSOR_ID, (uint8_t)SENSOR_TYPE_GYROSCOPE, (uint16_t)SENSOR_EVENT_LIVE_DATA,
                (uint16_t)(offset), (uint16_t)gyroSampleSizeInBytes);
            gyroPP = (gyroPP + 1) & 1;  //Alternate ping pong index between 0 and 1
        }
    }
}

void createBatchTimerForCompositeSensor(void)
{
    lsm6Dsl_State.batchTimer = createTimer(LsmSensorReadCb, (void*)SENSOR_OP_FFT_BATCH_READ);
}
void initLsm6dslSensor(void)
{
	registerRemoteCmdHandlerCb(doProcessingOnLsm6Dsl,LSM6DSL_SENSOR_ID);
	registerLocalMsgHandlerCb ( ProcessLsmSensorLocalMsg, LSM6DSL_SENSOR_ID);
	
}



