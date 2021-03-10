/* lsm6dsl.c - Driver for LSM6DSL accelerometer and gyroscope
 */

/*
 * Copyright (c) 2017 Linaro Limited
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Copyright (C) 2019 Eta Compute, Inc
 * Modified driver to fit in FreeRtos
 * Added FIFO mode
 */
#include "lsm6dsl.h"

#define LOG_DBG ecm35xx_printf
#define UNUSED     __attribute__ ((unused))

struct lsm6dsl_data *lsmdata;

static uint16_t odr_map[] = {0, 12, 26, 52, 104, 208, 416, 833,
                    1660, 3330, 6660};
/*
 * convert odr to register value
 */
static int32_t lsm6dsl_odr_to_val(uint16_t odr)
{
    size_t i;

    for (i = 0; i < ARRAY_SIZE(odr_map); i++) {
        if (odr == odr_map[i]) {
            return i;
        }
    }

    return -EINVAL;
}

/*
 * find closet from array
 */
static int32_t findClosestIndex(uint16_t arr[], uint32_t n, uint16_t target)
{
    int32_t index = 0;
    if (target <= arr[0])
        return 0;
    if (target >= arr[n - 1])
        return n - 1;

    // Doing binary search
    int32_t i = 0, j = n, mid = 0;
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
                if (target - arr[mid -1] >= arr[mid] - target)
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

/*
 * reboot sensor
 */
static inline int32_t lsm6dsl_reboot(struct lsm6dsl_data *data)
{

    if (data->hw_tf->update_reg(data, LSM6DSL_REG_CTRL3_C,
                    LSM6DSL_MASK_CTRL3_C_BOOT,
                    1 << LSM6DSL_SHIFT_CTRL3_C_BOOT) < 0) {
        return -EIO;
    }

    /* Wait sensor turn-on time as per datasheet */
    HalTmrDelay(0, 35);

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
static int32_t lsm6dsl_accel_fs_set(struct  lsm6dsl_data *data, lsm6dsl_fs_xl_t val)
{

  lsm6dsl_ctrl1_xl_t ctrl1_xl;

    if (data->hw_tf->read_data(data,
                    LSM6DSL_REG_CTRL1_XL,
                    (uint8_t *)&ctrl1_xl, 1) < 0) {
        return -EIO;
    }
     ctrl1_xl.fs_xl = (uint8_t) val;
    data->hw_tf->write_data(data,
                    LSM6DSL_REG_CTRL1_XL,
                    (uint8_t *)&ctrl1_xl, 1);
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
static int32_t lsm6dsl_gy_fs_set(struct lsm6dsl_data *data, lsm6dsl_fs_g_t val)
{
  lsm6dsl_ctrl2_g_t ctrl2_g;
  int32_t ret;

  ret = data->hw_tf->read_reg(data, LSM6DSL_REG_CTRL2_G, (uint8_t*)&ctrl2_g);
  if(ret == 0){
    ctrl2_g.fs_g = (uint8_t) val;
    ret = data->hw_tf->write_data(data, LSM6DSL_REG_CTRL2_G, (uint8_t*)&ctrl2_g, 1);
  }
  return ret;
}

/**
  * @brief  Accelerometer analog chain bandwidth selection (only for
  *         accelerometer ODR ≥ 1.67 kHz).[set]
  *
  * @param  data    Read / write interface definitions
  * @param  val    Change the values of bw0_xl in reg CTRL1_XL
  * @retval        Interface status (MANDATORY: return 0 -> no Error).
  *
  */
int32_t lsm6dsl_xl_filter_analog_set(struct lsm6dsl_data *data,
                                        lsm6dsl_bw0_xl_t val)
{
    lsm6dsl_ctrl1_xl_t ctrl1_xl;
    int32_t ret;

    ret = data->hw_tf->read_data(data,
            LSM6DSL_REG_CTRL1_XL, (uint8_t *)&ctrl1_xl, 1);
    if(ret == 0){
        ctrl1_xl.bw0_xl = (uint8_t) val;
        ret = data->hw_tf->write_data(data,
                LSM6DSL_REG_CTRL1_XL, (uint8_t*)&ctrl1_xl, 1);
    }
    return ret;

}

/**
  * @brief  FIFO mode selection.[set]
  *
  * @param  data    Read / write interface definitions
  * @param  val    Change the values of fifo_mode in reg FIFO_CTRL5
  * @retval        Interface status (MANDATORY: return 0 -> no Error).
  *
  */
int32_t lsm6dsl_fifo_mode_set(struct lsm6dsl_data *data, lsm6dsl_fifo_mode_t val)
{
  lsm6dsl_fifo_ctrl5_t fifo_ctrl5;
  int32_t ret;

  ret = data->hw_tf->read_reg(data,
                                LSM6DSL_REG_FIFO_CTRL5, (uint8_t*)&fifo_ctrl5);
  if(ret == 0){
    fifo_ctrl5.fifo_mode = (uint8_t)val;
    ret = data->hw_tf->write_data(data,
                                LSM6DSL_REG_FIFO_CTRL5, (uint8_t*)&fifo_ctrl5, 1);
  }
  return ret;
}

/**
  * @brief  FIFO pattern.[get]
  *
  * @param  data    Read / write interface definitions
  * @param  val    get the values of fifo_pattern in reg  FIFO_STATUS3 and
  *                FIFO_STATUS4, it is recommended to set the BDU bit
  * @retval        Interface status (MANDATORY: return 0 -> no Error).
  *
  */
UNUSED  static int32_t lsm6dsl_fifo_pattern_get(struct lsm6dsl_data *data, uint16_t *val)
{
  lsm6dsl_fifo_status3_t fifo_status3;
  lsm6dsl_fifo_status4_t fifo_status4;
  int32_t ret;

  ret = data->hw_tf->read_reg(data, LSM6DSL_REG_FIFO_STATUS3,
                         (uint8_t*)&fifo_status3);
  if(ret == 0){
    ret = data->hw_tf->read_reg(data, LSM6DSL_REG_FIFO_STATUS4,
                           (uint8_t*)&fifo_status4);
    *val = ( (uint16_t)fifo_status4.fifo_pattern << 8) +
             fifo_status3.fifo_pattern;
  }

  return ret;
}

/**
  * @brief  FIFO ODR selection, setting FIFO_MODE also.[set]
  *
  * @param  data    Read / write interface definitions
  * @param  val    Change the values of odr_fifo in reg FIFO_CTRL5
  * @retval        Interface status (MANDATORY: return 0 -> no Error).
  *
  */
static int32_t lsm6dsl_fifo_data_rate_set(struct lsm6dsl_data *data,
                                    lsm6dsl_odr_fifo_t val)
{
  lsm6dsl_fifo_ctrl5_t fifo_ctrl5;
  int32_t ret;

  ret =  data->hw_tf->read_reg(data,
            LSM6DSL_REG_FIFO_CTRL5, (uint8_t*)&fifo_ctrl5);
  if(ret == 0){
    fifo_ctrl5.odr_fifo = (uint8_t)val;
    ret =  data->hw_tf->write_data(data,
                LSM6DSL_REG_FIFO_CTRL5, (uint8_t*)&fifo_ctrl5, 1);
  }
  return ret;
}

/**
  * @brief  FIFO ODR selection, setting FIFO_MODE also.[get]
  *
  * @param  data    Read / write interface definitions
  * @param  val    Get the values of odr_fifo in reg FIFO_CTRL5
  * @retval        Interface status (MANDATORY: return 0 -> no Error).
  *
  */
UNUSED static int32_t
lsm6dsl_fifo_data_rate_get(struct lsm6dsl_data *data,
                            lsm6dsl_odr_fifo_t *val)
{
  lsm6dsl_fifo_ctrl5_t fifo_ctrl5;
  int32_t ret;

  ret = data->hw_tf->read_reg(data,
                LSM6DSL_REG_FIFO_CTRL5, (uint8_t*)&fifo_ctrl5);
  switch (fifo_ctrl5.odr_fifo)
  {
    case LSM6DSL_FIFO_DISABLE:
      *val = LSM6DSL_FIFO_DISABLE;
      break;
    case LSM6DSL_FIFO_12Hz5:
      *val = LSM6DSL_FIFO_12Hz5;
      break;
    case LSM6DSL_FIFO_26Hz:
      *val = LSM6DSL_FIFO_26Hz;
      break;
    case LSM6DSL_FIFO_52Hz:
      *val = LSM6DSL_FIFO_52Hz;
      break;
    case LSM6DSL_FIFO_104Hz:
      *val = LSM6DSL_FIFO_104Hz;
      break;
    case LSM6DSL_FIFO_208Hz:
      *val = LSM6DSL_FIFO_208Hz;
      break;
    case LSM6DSL_FIFO_416Hz:
      *val = LSM6DSL_FIFO_416Hz;
      break;
    case LSM6DSL_FIFO_833Hz:
      *val = LSM6DSL_FIFO_833Hz;
      break;
    case LSM6DSL_FIFO_1k66Hz:
      *val = LSM6DSL_FIFO_1k66Hz;
      break;
    case LSM6DSL_FIFO_3k33Hz:
      *val = LSM6DSL_FIFO_3k33Hz;
      break;
    case LSM6DSL_FIFO_6k66Hz:
      *val = LSM6DSL_FIFO_6k66Hz;
      break;
    default:
      *val = LSM6DSL_FIFO_RATE_ND;
      break;
  }
  return ret;
}

/**
  * @brief  FIFO data level.[get]
  *
  * @param  data    Read / write interface definitions
  * @param  val    get the values of diff_fifo in reg  FIFO_STATUS1 and
  *                FIFO_STATUS2(diff_fifo), it is recommended to set the
  *                BDU bit.
  * @retval        Interface status (MANDATORY: return 0 -> no Error).
  *
  */
static int32_t
lsm6dsl_fifo_data_level_get(struct lsm6dsl_data *data, uint16_t *val)
{
  lsm6dsl_fifo_status1_t fifo_status1;
  lsm6dsl_fifo_status2_t fifo_status2;
  int32_t ret;

  ret = data->hw_tf->read_reg(data, LSM6DSL_REG_FIFO_STATUS1,
                         (uint8_t*)&fifo_status1);
  if(ret == 0){
    ret = data->hw_tf->read_reg(data, LSM6DSL_REG_FIFO_STATUS2,
                           (uint8_t*)&fifo_status2);
    *val = ( (uint16_t) fifo_status2.diff_fifo << 8) +
             (uint16_t) fifo_status1.diff_fifo;
  }

  return ret;
}

/**
  * @brief  FIFO watermark level selection.[set]
  *
  * @param  data    Read / write interface definitions
  * @param  val    Change the values of fth in reg FIFO_CTRL1
  * @retval        Interface status (MANDATORY: return 0 -> no Error).
  *
  */
static int32_t
lsm6dsl_fifo_watermark_set(struct lsm6dsl_data *data, uint16_t val)
{
  lsm6dsl_fifo_ctrl1_t fifo_ctrl1;
  lsm6dsl_fifo_ctrl2_t fifo_ctrl2;
  int32_t ret;

  ret = data->hw_tf->read_reg(data,
                    LSM6DSL_REG_FIFO_CTRL2, (uint8_t*)&fifo_ctrl2);
  if(ret == 0){
    fifo_ctrl1.fth = (uint8_t) (0x00FFU & val);
    fifo_ctrl2.fth = (uint8_t) (( 0x0700U & val ) >> 8);
    ret = data->hw_tf->write_data(data,
                    LSM6DSL_REG_FIFO_CTRL1, (uint8_t*)&fifo_ctrl1, 1);
    if(ret == 0){
      ret = data->hw_tf->write_data(data, LSM6DSL_REG_FIFO_CTRL2,
                              (uint8_t*)&fifo_ctrl2, 1);
    }
  }
  return ret;
}

/**
  * @brief  Selects Batching Data Rate (writing frequency in FIFO) for
  *         accelerometer data.[set]
  *
  * @param  data    Read / write interface definitions
  * @param  val    Change the values of dec_fifo_xl in reg FIFO_CTRL3
  * @retval        Interface status (MANDATORY: return 0 -> no Error).
  *
  */
static int32_t
lsm6dsl_fifo_xl_dec_set(struct lsm6dsl_data *data,
                        lsm6dsl_dec_fifo_xl_t val)
{
  lsm6dsl_fifo_ctrl3_t fifo_ctrl3;
  int32_t ret;

  ret = data->hw_tf->read_reg(data,
                LSM6DSL_REG_FIFO_CTRL3, (uint8_t*)&fifo_ctrl3);
  if(ret == 0){
    fifo_ctrl3.dec_fifo_xl = (uint8_t)val;
    ret = data->hw_tf->write_data(data, LSM6DSL_REG_FIFO_CTRL3,
                            (uint8_t*)&fifo_ctrl3, 1);
  }
  return ret;
}

/**
  * @brief  Selects Batching Data Rate (writing frequency in FIFO) for
  *         accelerometer data.[get]
  *
  * @param  data    Read / write interface definitions
  * @param  val    Get the values of dec_fifo_xl in reg FIFO_CTRL3
  * @retval        Interface status (MANDATORY: return 0 -> no Error).
  *
  */
UNUSED static int32_t lsm6dsl_fifo_xl_dec_get(struct lsm6dsl_data *data,
                                  lsm6dsl_dec_fifo_xl_t *val)
{
  lsm6dsl_fifo_ctrl3_t fifo_ctrl3;
  int32_t ret;

  ret = data->hw_tf->read_reg(data,
            LSM6DSL_REG_FIFO_CTRL3, (uint8_t*)&fifo_ctrl3);
  switch (fifo_ctrl3.dec_fifo_xl) {
    case LSM6DSL_FIFO_XL_DISABLE:
      *val = LSM6DSL_FIFO_XL_DISABLE;
      break;
    case LSM6DSL_FIFO_XL_NO_DEC:
      *val = LSM6DSL_FIFO_XL_NO_DEC;
      break;
    case LSM6DSL_FIFO_XL_DEC_2:
      *val = LSM6DSL_FIFO_XL_DEC_2;
      break;
    case LSM6DSL_FIFO_XL_DEC_3:
      *val = LSM6DSL_FIFO_XL_DEC_3;
      break;
    case LSM6DSL_FIFO_XL_DEC_4:
      *val = LSM6DSL_FIFO_XL_DEC_4;
      break;
    case LSM6DSL_FIFO_XL_DEC_8:
      *val = LSM6DSL_FIFO_XL_DEC_8;
      break;
    case LSM6DSL_FIFO_XL_DEC_16:
      *val = LSM6DSL_FIFO_XL_DEC_16;
      break;
    case LSM6DSL_FIFO_XL_DEC_32:
      *val = LSM6DSL_FIFO_XL_DEC_32;
      break;
    default:
      *val = LSM6DSL_FIFO_XL_DEC_ND;
      break;
  }

  return ret;
}

/**
  * @brief  Selects Batching Data Rate (writing frequency in FIFO)
  *         for gyroscope data.[set]
  *
  * @param  data    Read / write interface definitions
  * @param  val    Change the values of dec_fifo_gyro in reg FIFO_CTRL3
  * @retval        Interface status (MANDATORY: return 0 -> no Error).
  *
  */
static int32_t lsm6dsl_fifo_gy_dec_set(struct lsm6dsl_data *data,
                                  lsm6dsl_dec_fifo_gyro_t val)
{
  lsm6dsl_fifo_ctrl3_t fifo_ctrl3;
  int32_t ret;

  ret = data->hw_tf->read_reg(data, LSM6DSL_REG_FIFO_CTRL3, (uint8_t*)&fifo_ctrl3);
  if(ret == 0){
    fifo_ctrl3.dec_fifo_gyro = (uint8_t)val;
    ret = data->hw_tf->write_data(data, LSM6DSL_REG_FIFO_CTRL3, (uint8_t*)&fifo_ctrl3, 1);
  }
  return ret;
}

/**
  * @brief  Selects Batching Data Rate (writing frequency in FIFO)
  *         for gyroscope data.[get]
  *
  * @param  data    Read / write interface definitions
  * @param  val    Get the values of dec_fifo_gyro in reg FIFO_CTRL3
  * @retval        Interface status (MANDATORY: return 0 -> no Error).
  *
  */
UNUSED static int32_t lsm6dsl_fifo_gy_dec_get(struct lsm6dsl_data *data,
                                  lsm6dsl_dec_fifo_gyro_t *val)
{
  lsm6dsl_fifo_ctrl3_t fifo_ctrl3;
  int32_t ret;

  ret = data->hw_tf->read_reg(data,
            LSM6DSL_REG_FIFO_CTRL3, (uint8_t*)&fifo_ctrl3);
  switch (fifo_ctrl3.dec_fifo_gyro)
  {
    case LSM6DSL_FIFO_GY_DISABLE:
      *val = LSM6DSL_FIFO_GY_DISABLE;
      break;
    case LSM6DSL_FIFO_GY_NO_DEC:
      *val = LSM6DSL_FIFO_GY_NO_DEC;
      break;
    case LSM6DSL_FIFO_GY_DEC_2:
      *val = LSM6DSL_FIFO_GY_DEC_2;
      break;
    case LSM6DSL_FIFO_GY_DEC_3:
      *val = LSM6DSL_FIFO_GY_DEC_3;
      break;
    case LSM6DSL_FIFO_GY_DEC_4:
      *val = LSM6DSL_FIFO_GY_DEC_4;
      break;
    case LSM6DSL_FIFO_GY_DEC_8:
      *val = LSM6DSL_FIFO_GY_DEC_8;
      break;
    case LSM6DSL_FIFO_GY_DEC_16:
      *val = LSM6DSL_FIFO_GY_DEC_16;
      break;
    case LSM6DSL_FIFO_GY_DEC_32:
      *val = LSM6DSL_FIFO_GY_DEC_32;
      break;
    default:
      *val = LSM6DSL_FIFO_GY_DEC_ND;
      break;
  }

  return ret;
}

/**
  * @brief  FIFO watermark level selection.[get]
  *
  * @param  data    Read / write interface definitions
  * @param  val    Change the values of fth in reg FIFO_CTRL1
  * @retval        Interface status (MANDATORY: return 0 -> no Error).
  *
  */
UNUSED static int32_t lsm6dsl_fifo_watermark_get(struct lsm6dsl_data *data, int16_t *val)
{
  lsm6dsl_fifo_ctrl1_t fifo_ctrl1;
  lsm6dsl_fifo_ctrl2_t fifo_ctrl2;
  int32_t ret;

  ret = data->hw_tf->read_reg(data,
                LSM6DSL_REG_FIFO_CTRL1, (uint8_t*)&fifo_ctrl1);
  if(ret == 0)
  {
    ret = data->hw_tf->read_reg(data,
                LSM6DSL_REG_FIFO_CTRL2, (uint8_t*)&fifo_ctrl2);
  }
  *val = ((uint16_t)fifo_ctrl2.fth << 8) + (uint16_t)fifo_ctrl1.fth;

  return ret;
}

/**
  * @brief  Read data in FIFO.[get]
  *
  * @param  data    Read / write interface definitions
  * @param  buffer Data buffer to store FIFO data.
  * @param  len    Number of data to read from FIFO.
  * @retval        Interface status (MANDATORY: return 0 -> no Error).
  *
  */
static int32_t lsm6dsl_fifo_raw_data_get(struct lsm6dsl_data *data, uint16_t *buffer)
{
  int32_t ret;
  ret = data->hw_tf->read_data(data,
                LSM6DSL_REG_FIFO_DATA_OUT_L, (uint8_t *)buffer, 2);
  return ret;
}

/*
 * get gcd of two number
 */
static uint16_t get_gcd(uint16_t n1, uint16_t n2)
{
    while(n1!=n2)
    {
        if(n1 > n2)
            n1 -= n2;
        else
            n2 -= n1;
    }
    return n1;
}

#ifdef CONFIG_SENSOR_TIMESTAMP
/*
 * set decimation for TimeStamp or Pedometer
 */
static int32_t lsm6dsl_fifo_dataset_4_batch_set(struct lsm6dsl_data *data,
                                         lsm6dsl_dec_ds4_fifo_t val)
{
  lsm6dsl_fifo_ctrl4_t fifo_ctrl4;
  int32_t ret;

  ret = data->hw_tf->read_reg(data,
                                LSM6DSL_REG_FIFO_CTRL4, (uint8_t*)&fifo_ctrl4);
  if(ret == 0){
    fifo_ctrl4.dec_ds4_fifo = (uint8_t)val;
    ret = data->hw_tf->write_data(data, LSM6DSL_REG_FIFO_CTRL4,
                            (uint8_t*)&fifo_ctrl4, 1);
  }
  return ret;
}
#endif

static int32_t lsm6dsl_batch_enable(tSensorType iSensor)
{
    uint16_t gcd, FifoTh = 0;
    /*
     * odr should be set before batch buffer
     */
    if (!(lsmdata->ui16XlOdr) && !(lsmdata->ui16GyroOdr))
            return -EINVAL;

    /* first put fifo in bypass mode */
    lsm6dsl_fifo_mode_set(lsmdata, LSM6DSL_BYPASS_MODE);
    /* Disable Fifo TH interrupt */
    lsmdata->hw_tf->update_reg(lsmdata, LSM6DSL_REG_INT1_CTRL,
                                LSM6DSL_MASK_INT1_FTH, 0);

    /*
     * Fifo th based on number of Xl and Gyro samples collected
     */
    if (lsmdata->sXlBinfo.ui8Buf && lsmdata->sGyBinfo.ui8Buf &&
            lsmdata->ui16XlOdr && lsmdata->ui16GyroOdr)
    {
        /* Xl odr == Gy Odr */
        if (lsmdata->ui16XlOdr == lsmdata->ui16GyroOdr)
        {
            lsmdata->GyCycleMask = 1;
            lsmdata->XlCycleMask = 1;
            lsmdata->fifo_sample_fact = 1;

            /*
             * get max common factor in case of differrent batch size
             */
#ifdef CONFIG_SENSOR_TIMESTAMP
            gcd = get_gcd((lsmdata->sGyBinfo.ui16BatchLen * XYG_LEN)/ XYG_TS_LEN ,
                    (lsmdata->sXlBinfo.ui16BatchLen * XYG_LEN) / XYG_TS_LEN);
            lsmdata->XlBatchLen = lsmdata->GyBatchLen = gcd;
            /* Fifo should be in 16bits */
            FifoTh =  (gcd +  gcd +  gcd) / 2;
#else
            gcd = get_gcd(lsmdata->sGyBinfo.ui16BatchLen,
                    lsmdata->sXlBinfo.ui16BatchLen);
            lsmdata->XlBatchLen = lsmdata->GyBatchLen = gcd;

            FifoTh = (gcd +  gcd ) / 2;
#endif
            lsmdata->FifoTh = FifoTh;

            lsm6dsl_fifo_xl_dec_set(lsmdata, LSM6DSL_FIFO_XL_NO_DEC);
            lsm6dsl_fifo_gy_dec_set(lsmdata, LSM6DSL_FIFO_GY_NO_DEC);
            lsm6dsl_fifo_data_rate_set(lsmdata,
                        lsm6dsl_odr_to_val(lsmdata->ui16GyroOdr));
        }
        else if(lsmdata->ui16XlOdr > lsmdata->ui16GyroOdr)
        {
            /* odr should be multiple of each other */
            lsmdata->fifo_sample_fact = (lsmdata->ui16XlOdr /
                    lsmdata->ui16GyroOdr);
            lsmdata->XlCycleMask = ((1 << lsmdata->fifo_sample_fact) - 1);
            lsmdata->GyCycleMask = 1;
#ifdef CONFIG_SENSOR_TIMESTAMP
            gcd = get_gcd(((lsmdata->sGyBinfo.ui16BatchLen * XYG_LEN) / XYG_TS_LEN),
                            ((lsmdata->sXlBinfo.ui16BatchLen * XYG_LEN) /
                                (lsmdata->fifo_sample_fact * XYG_TS_LEN)));
            ecm35xx_printf("--gcd-- %d \r\n", gcd);
            lsmdata->GyBatchLen = gcd;
            lsmdata->XlBatchLen = (gcd * lsmdata->fifo_sample_fact);
            FifoTh = (lsmdata->GyBatchLen + (lsmdata->XlBatchLen * 2)) / 2;
#else
            gcd = get_gcd(lsmdata->sGyBinfo.ui16BatchLen,
                            (lsmdata->sXlBinfo.ui16BatchLen /
                                lsmdata->fifo_sample_fact));
            lsmdata->GyBatchLen = gcd;
            lsmdata->XlBatchLen = (gcd * lsmdata->fifo_sample_fact);
            FifoTh = (lsmdata->XlBatchLen + lsmdata->GyBatchLen ) /2;
#endif
            lsmdata->FifoTh = FifoTh;
            lsm6dsl_fifo_xl_dec_set(lsmdata, LSM6DSL_FIFO_XL_NO_DEC);
            /* if odr is less then fifo
             * required decimation to match Fifo rate
             * */
            lsm6dsl_fifo_gy_dec_set(lsmdata, lsmdata->fifo_sample_fact);
            lsm6dsl_fifo_data_rate_set(lsmdata,
                        lsm6dsl_odr_to_val(lsmdata->ui16XlOdr));
        }
        else if(lsmdata->ui16GyroOdr > lsmdata->ui16XlOdr)
        {
            lsmdata->fifo_sample_fact = (lsmdata->ui16GyroOdr /
                    lsmdata->ui16XlOdr);

            lsmdata->GyCycleMask = ((1 << lsmdata->fifo_sample_fact) - 1);
            lsmdata->XlCycleMask = 1;
#ifdef CONFIG_SENSOR_TIMESTAMP
            gcd = get_gcd(((lsmdata->sGyBinfo.ui16BatchLen * XYG_LEN ) /
                                (lsmdata->fifo_sample_fact * XYG_TS_LEN)),
                            (lsmdata->sXlBinfo.ui16BatchLen * XYG_LEN) / XYG_TS_LEN);

            lsmdata->XlBatchLen = gcd;
            lsmdata->GyBatchLen = gcd * lsmdata->fifo_sample_fact;
            FifoTh = (lsmdata->XlBatchLen + (lsmdata->GyBatchLen * 2)) / 2;
#else
            gcd = get_gcd(lsmdata->sGyBinfo.ui16BatchLen /
                            lsmdata->fifo_sample_fact,
                            lsmdata->sXlBinfo.ui16BatchLen);

            lsmdata->XlBatchLen = gcd;
            lsmdata->GyBatchLen = gcd * lsmdata->fifo_sample_fact;

            FifoTh = (lsmdata->XlBatchLen + lsmdata->GyBatchLen) / 2 ;
#endif
            lsmdata->FifoTh = FifoTh;

            /* if odr is less then fifo
             * required decimation to match Fifo rate
             * */
            lsm6dsl_fifo_xl_dec_set(lsmdata, lsmdata->fifo_sample_fact);
            lsm6dsl_fifo_gy_dec_set(lsmdata, LSM6DSL_FIFO_GY_NO_DEC);
            lsm6dsl_fifo_data_rate_set(lsmdata,
                                lsm6dsl_odr_to_val(lsmdata->ui16GyroOdr));

        }

   }

    else if (lsmdata->sXlBinfo.ui8Buf && lsmdata->ui16XlOdr)
    {
            lsmdata->fifo_sample_fact = 1;
            lsm6dsl_fifo_xl_dec_set(lsmdata, LSM6DSL_FIFO_XL_NO_DEC);
            /*
             * Disable collection in FIFO
             * if batch not enable
             */
            lsm6dsl_fifo_gy_dec_set(lsmdata, LSM6DSL_FIFO_GY_DISABLE);
            lsm6dsl_fifo_data_rate_set(lsmdata,
                        lsm6dsl_odr_to_val(lsmdata->ui16XlOdr));
            lsmdata->XlBatchLen = lsmdata->sXlBinfo.ui16BatchLen;
#ifdef CONFIG_SENSOR_TIMESTAMP
            FifoTh = (lsmdata->sXlBinfo.ui16BatchLen * XYG_LEN) / XYG_TS_LEN;
#else
            FifoTh = (lsmdata->sXlBinfo.ui16BatchLen / 2);
#endif
            lsmdata->FifoTh = FifoTh;
            lsmdata->GyBatchLen = 0;
            lsmdata->XlCycleMask = 1;
            lsmdata->GyCycleMask = 0;

    }
    else if (lsmdata->sGyBinfo.ui8Buf && lsmdata->ui16GyroOdr)
    {
            lsmdata->fifo_sample_fact = 1;
            lsmdata->GyCycleMask = 1;
            lsmdata->XlCycleMask = 0;
            lsm6dsl_fifo_gy_dec_set(lsmdata, LSM6DSL_FIFO_GY_NO_DEC);
            /*
             * Disable collection in FIFO
             * if batch not enable
             */
            lsm6dsl_fifo_xl_dec_set(lsmdata, LSM6DSL_FIFO_XL_DISABLE);
            lsm6dsl_fifo_data_rate_set(lsmdata,
                        lsm6dsl_odr_to_val(lsmdata->ui16GyroOdr));
            lsmdata->GyBatchLen = lsmdata->sGyBinfo.ui16BatchLen;
#ifdef CONFIG_SENSOR_TIMESTAMP
            FifoTh = (lsmdata->sGyBinfo.ui16BatchLen * XYG_LEN)/ XYG_TS_LEN;
#else
            FifoTh = (lsmdata->sGyBinfo.ui16BatchLen / 2);
#endif
            lsmdata->XlBatchLen = 0;
            lsmdata->FifoTh = FifoTh;
    }

    /* Use FIFO in Ping Pong mode */
    if (FifoTh > FIFO_HALF_LEN)
    {
        FifoTh = FifoTh / (1 + (FifoTh / FIFO_HALF_LEN));
        lsmdata->FifoTh = FifoTh;
    }
    if (iSensor ==  SENSOR_TYPE_ACCELEROMETER)
        lsmdata->XlBatchSt = 1;
    else if (iSensor ==  SENSOR_TYPE_GYROSCOPE)
        lsmdata->GyBatchSt = 1;

    ecm35xx_printf("Batch fifo sample fact %d, fifoth %d \r\n",
                    lsmdata->fifo_sample_fact, FifoTh);
    lsm6dsl_fifo_watermark_set(lsmdata, FifoTh);


    ecm35xx_printf("Batch Size sGyro %d  Xl %d \r\n",
            lsmdata->sGyBinfo.ui16BatchLen, lsmdata->sXlBinfo.ui16BatchLen);

    ecm35xx_printf("sample fact %d\r\n",lsmdata->fifo_sample_fact);

#ifdef CONFIG_SENSOR_TIMESTAMP

    lsmdata->hw_tf->update_reg(lsmdata, LSM6DSL_REG_CTRL10_C,
                                LSM6DSL_MASK_CTRL10_C_TIMER_EN,
                                1 << LSM6DSL_SHIFT_CTRL10_C_TIMER_EN);

    lsm6dsl_fifo_dataset_4_batch_set(lsmdata, LSM6DSL_FIFO_DS4_NO_DEC);

    lsmdata->hw_tf->update_reg(lsmdata, LSM6DSL_REG_FIFO_CTRL2,
                                LSM6DSL_MASK_FIFO_CTRL2_TIMER_PEDO_FIFO_EN,
                                1 << LSM6DSL_SHIFT_FIFO_CTRL2_TIMER_PEDO_FIFO_EN);
#endif

    lsmdata->XlBatchCnt = 0;
    lsmdata->GyBatchCnt = 0;
    lsmdata->GyBatchCurrPtr =
        lsmdata->GyBatchPtr = lsmdata->GyBatchStartPtr;
    lsmdata->XlBatchCurrPtr =
        lsmdata->XlBatchPtr = lsmdata->XlBatchStartPtr;

    lsm6dsl_fifo_mode_set(lsmdata, LSM6DSL_FIFO_MODE);
    lsmdata->hw_tf->update_reg(lsmdata, LSM6DSL_REG_INT1_CTRL,
                                LSM6DSL_MASK_INT1_FTH,
                                (1 << LSM6DSL_SHIFT_INT1_FTH));
    return 0;
}

/**
  * @brief  Accelerometer data rate selection.[set]
  *
  * @param  data    Read / write interface definitions
  * @param  val    Change the values of odr_xl in reg CTRL1_XL
  * @retval        Interface status (MANDATORY: return 0 -> no Error).
  *
  */
static int32_t lsm6dsl_accel_data_rate_set(struct lsm6dsl_data *data,
                                        lsm6dsl_odr_xl_t val)
{
    lsm6dsl_ctrl1_xl_t ctrl1_xl;
    if (data->hw_tf->read_data(data,
                    LSM6DSL_REG_CTRL1_XL,
                    (uint8_t *)&ctrl1_xl, 1) < 0) {
        return -EIO;
    }
    ctrl1_xl.odr_xl = (uint8_t) val;
    data->hw_tf->write_data(data,
                    LSM6DSL_REG_CTRL1_XL,
                    (uint8_t *)&ctrl1_xl, 1);
    data->ui16XlOdr = odr_map[val];

    /* analog chain bandwidth */
    if (data->ui16XlOdr <= 416)
        lsm6dsl_xl_filter_analog_set(data, LSM6DSL_XL_ANA_BW_400Hz);
    else
        lsm6dsl_xl_filter_analog_set(data, LSM6DSL_XL_ANA_BW_1k5Hz);
    return 0;
}

/**
  * @brief  Accelerometer data rate selection.[set]
  *
  * @param  data    Read / write interface definitions
  * @param  val    Change the values of odr_xl in reg CTRL1_XL
  * @retval        Interface status (MANDATORY: return 0 -> no Error).
  *
  */
static int32_t lsm6dsl_accel_data_rate_get(struct lsm6dsl_data *data)
{
    uint16_t odr;
    lsm6dsl_ctrl1_xl_t ctrl1_xl;
    if (data->hw_tf->read_data(data,
                    LSM6DSL_REG_CTRL1_XL,
                    (uint8_t *)&ctrl1_xl, 1) < 0) {
        return -EIO;
    }
    odr = ctrl1_xl.odr_xl;

    /* for valid index, return value from map */
    if (odr < ARRAY_SIZE(odr_map)) {
        return odr_map[odr];
    }

    /* invalid index, return 1st entry, not enabled*/
    return odr_map[0];

}

static int32_t
lsm6dsl_gyro_data_rate_set(struct lsm6dsl_data *data, lsm6dsl_odr_g_t val)
{
    lsm6dsl_ctrl2_g_t ctrl2_g;
    if (data->hw_tf->read_data(data,
                    LSM6DSL_REG_CTRL2_G,
                    (uint8_t *)&ctrl2_g, 1) < 0) {
        return -EIO;
    }
    ctrl2_g.odr_g = (uint8_t) val;
    data->hw_tf->write_data(data,
                    LSM6DSL_REG_CTRL2_G,
                    (uint8_t *)&ctrl2_g, 1);

    data->ui16GyroOdr = odr_map[val];

    return 0;
}

static int32_t
lsm6dsl_gyro_data_rate_get(struct lsm6dsl_data *data)
{
    uint16_t odr;
    lsm6dsl_ctrl2_g_t ctrl2_g;
    if (data->hw_tf->read_data(data,
                    LSM6DSL_REG_CTRL2_G,
                    (uint8_t *)&ctrl2_g, 1) < 0) {
        return -EIO;
    }
    odr = ctrl2_g.odr_g;

    /* for valid index, return value from map */
    if (odr < ARRAY_SIZE(odr_map)) {
        return odr_map[odr];
    }

    /* invalid index, return 1st entry, not enabled*/
    return odr_map[0];

}

static int32_t
lsm6dsl_sample_fetch_accel(struct lsm6dsl_data *data,
                            uint8_t *buf, uint8_t len)
{
    if (len < 6)
        return -ENOMEM;
    if (data->hw_tf->read_data(data, LSM6DSL_REG_OUTX_L_XL,
                                buf, 6) < 0) {
        LOG_DBG("failed to read sample");
        return -EIO;
    }
    return 0;
}

static int32_t
lsm6dsl_sample_fetch_gyro(struct lsm6dsl_data *data,
                            uint8_t *buf, uint8_t len)
{
    if (len < 6)
        return -ENOMEM;

    if (data->hw_tf->read_data(data, LSM6DSL_REG_OUTX_L_G,
                buf, 6) < 0) {
        LOG_DBG("failed to read sample");
        return -EIO;
    }
    return 0;
}

static int32_t
lsm6dsl_sample_fetch(struct lsm6dsl_data *data,
                        tSensorType chan, uint8_t *buf, uint8_t len)
{
    switch (chan)
    {
    case SENSOR_TYPE_ACCELEROMETER:
        lsm6dsl_sample_fetch_accel(data, buf, len);
        break;
    case SENSOR_TYPE_GYROSCOPE:
        lsm6dsl_sample_fetch_gyro(data, buf, len);
        break;
    default:
        return -EOPNOTSUPP;
    }

    return 0;
}

static int32_t lsm6dsl_init_chip(struct lsm6dsl_data *data)
{
    uint8_t chip_id;
    uint8_t val;

    /*
       if (lsm6dsl_reboot(data) < 0) {
       LOG_DBG("failed to reboot device");
       return -EIO;
       }
       */
    /* Soft reset */
    if (data->hw_tf->update_reg(data,
                LSM6DSL_REG_CTRL3_C,
                LSM6DSL_MASK_CTRL3_C_SW_RESET,
                1 << LSM6DSL_SHIFT_CTRL3_C_SW_RESET) < 0) {
        LOG_DBG("failed to Reset");
        return -EIO;
    }

    HalTmrDelay(0, 50);
    if (data->hw_tf->read_reg(data,
                LSM6DSL_REG_WHO_AM_I, (uint8_t *)&chip_id) < 0) {
        LOG_DBG("failed reading chip id");
        return -EIO;
    }
    if (chip_id != LSM6DSL_VAL_WHO_AM_I) {
        LOG_DBG("invalid chip id 0x%x", chip_id);
        return -EIO;
    }
    LOG_DBG("chip id 0x%x\r\n", chip_id);

    if (data->hw_tf->update_reg(data,
                LSM6DSL_REG_CTRL3_C,
                LSM6DSL_MASK_CTRL3_C_BDU |
                LSM6DSL_MASK_CTRL3_C_IF_INC,
                1 << LSM6DSL_SHIFT_CTRL3_C_BDU |
                1 << LSM6DSL_SHIFT_CTRL3_C_IF_INC) < 0) {
        LOG_DBG("failed to set FIFO mode");
        return -EIO;
    }

    val = LSM6DSL_MASK_CTRL4_C_INT2_ON_INT1;
    if (data->hw_tf->write_data(data,
                    LSM6DSL_REG_CTRL4_C, &val, 1) < 0) {
        LOG_DBG("failed reading chip id");
        return -EIO;
    }

    return 0;
}

/*
 * deferred ISR
 */
static void Irq_postprocess( void *data, uint32_t param)
{
    uint16_t FifoLevel = 0 , RdCycle, i, j;
    volatile lsm6dsl_fifo_status2_t fifo_status2;
    uint16_t tbuf[3];
    uint8_t reg = 0;
    tSensorEventInfo EvInfo;
    uint8_t cycle_mask;
    struct lsm6dsl_data *lsmdata = (struct lsm6dsl_data *)data;

    xSemaphoreTake(lsmdata->DevLock, portMAX_DELAY);
    do
    {
        /* live data, data ready ISR */
        if (lsmdata->XlEvMask & SENSOR_EVENT_LIVE_DATA ||
                lsmdata->GyEvMask & SENSOR_EVENT_LIVE_DATA)
        {
            lsmdata->hw_tf->read_reg(lsmdata,
                    LSM6DSL_REG_STATUS_REG, &reg);
            /*
             * if XL Live Data Event is enable and Data is available
             */
            if((lsmdata->XlEvMask & SENSOR_EVENT_LIVE_DATA) &&
                    (reg & LSM6DSL_MASK_STATUS_REG_XLDA))
            {
                lsm6dsl_sample_fetch_accel(lsmdata, (uint8_t *)tbuf, 6);
                EvInfo.iEvent = SENSOR_EVENT_LIVE_DATA;
                EvInfo.iSensor = SENSOR_TYPE_ACCELEROMETER;
                EvInfo.sEvData.ui8Buf = (uint8_t *)tbuf;
                EvInfo.sEvData.ui16BufLen = 6;
                lsmdata->fXlCb(&EvInfo, lsmdata->vXlCbptr);
            }
            /*
             * if Gy Live Data Event is enable and Data is available
             */
            if((lsmdata->GyEvMask & SENSOR_EVENT_LIVE_DATA) &&
                    (reg & LSM6DSL_MASK_STATUS_REG_GDA))
            {
                lsm6dsl_sample_fetch_gyro(lsmdata, (uint8_t *)tbuf, 6);
                EvInfo.iEvent = SENSOR_EVENT_LIVE_DATA;
                EvInfo.iSensor = SENSOR_TYPE_GYROSCOPE;
                EvInfo.sEvData.ui8Buf = (uint8_t *)tbuf;
                EvInfo.sEvData.ui16BufLen = 6;
                lsmdata->fGyCb(&EvInfo, lsmdata->vGyCbptr);
            }

            lsmdata->hw_tf->read_reg(lsmdata,
                    LSM6DSL_REG_STATUS_REG, &reg);
        }
    } while(reg & (LSM6DSL_MASK_STATUS_REG_XLDA |
                    LSM6DSL_MASK_STATUS_REG_GDA));

    /* Batching Fifo watermark ISR */
    lsmdata->hw_tf->read_reg(lsmdata,
            LSM6DSL_REG_FIFO_STATUS2, (uint8_t*)&fifo_status2);
    lsm6dsl_fifo_data_level_get(lsmdata, &FifoLevel);
    if (fifo_status2.waterm && (FifoLevel >= lsmdata->FifoTh))
    {
        ecm35xx_printf("FCNT %d\r\n", FifoLevel);
        /* Xl and Gy both in batching mode*/
        if( lsmdata->XlBatchLen && lsmdata->GyBatchLen)
        {
            /* Xl Odr == Gy Odr */
            if (lsmdata->fifo_sample_fact == 1)
            {
                /*
                 * fifo sample cycle will have
                 * Gy sample followed by Xl sample
                 */
                /* RdCycle is word read*/
#ifdef CONFIG_SENSOR_TIMESTAMP
                RdCycle = (lsmdata->FifoTh / 3);
#else
                RdCycle = (lsmdata->FifoTh / 2);
#endif
                for (i = 0; i < RdCycle; i= i + 3)
                {
                    /*
                     * pattern in fifo will be
                     * Gy X:Y:Z followed by Xl X:Y:Z
                     * */
                    lsm6dsl_fifo_raw_data_get(lsmdata, lsmdata->GyBatchCurrPtr++);
                    if (lsmdata->GyBatchCurrPtr == lsmdata->GyBatchEndPtr)
                        lsmdata->GyBatchCurrPtr = lsmdata->GyBatchStartPtr;

                    lsm6dsl_fifo_raw_data_get(lsmdata, lsmdata->GyBatchCurrPtr++);
                    if (lsmdata->GyBatchCurrPtr == lsmdata->GyBatchEndPtr)
                        lsmdata->GyBatchCurrPtr = lsmdata->GyBatchStartPtr;

                    lsm6dsl_fifo_raw_data_get(lsmdata, lsmdata->GyBatchCurrPtr++);
                    if (lsmdata->GyBatchCurrPtr == lsmdata->GyBatchEndPtr)
                        lsmdata->GyBatchCurrPtr = lsmdata->GyBatchStartPtr;

                    lsmdata->GyBatchCnt += 3;

                    lsm6dsl_fifo_raw_data_get(lsmdata, lsmdata->XlBatchCurrPtr++);
                    if (lsmdata->XlBatchCurrPtr == lsmdata->XlBatchEndPtr)
                        lsmdata->XlBatchCurrPtr = lsmdata->XlBatchStartPtr;

                    lsm6dsl_fifo_raw_data_get(lsmdata, lsmdata->XlBatchCurrPtr++);
                    if (lsmdata->XlBatchCurrPtr == lsmdata->XlBatchEndPtr)
                        lsmdata->XlBatchCurrPtr = lsmdata->XlBatchStartPtr;

                    lsm6dsl_fifo_raw_data_get(lsmdata, lsmdata->XlBatchCurrPtr++);
                    if (lsmdata->XlBatchCurrPtr == lsmdata->XlBatchEndPtr)
                        lsmdata->XlBatchCurrPtr = lsmdata->XlBatchStartPtr;

                    lsmdata->XlBatchCnt += 3;

#ifdef CONFIG_SENSOR_TIMESTAMP
                    /* convert to LE format*/
                    lsm6dsl_fifo_raw_data_get(lsmdata, &tbuf[0]);
                    lsm6dsl_fifo_raw_data_get(lsmdata, &tbuf[1]);
                    lsm6dsl_fifo_raw_data_get(lsmdata, &tbuf[2]);

                    *lsmdata->XlBatchCurrPtr++ =
                        *lsmdata->GyBatchCurrPtr++ = (tbuf[1] >> 8);
                    if (lsmdata->XlBatchCurrPtr == lsmdata->XlBatchEndPtr)
                        lsmdata->XlBatchCurrPtr = lsmdata->XlBatchStartPtr;
                    if (lsmdata->GyBatchCurrPtr == lsmdata->GyBatchEndPtr)
                        lsmdata->GyBatchCurrPtr = lsmdata->GyBatchStartPtr;

                    *lsmdata->XlBatchCurrPtr++ =
                        *lsmdata->GyBatchCurrPtr++ = tbuf[0];
                    if (lsmdata->XlBatchCurrPtr == lsmdata->XlBatchEndPtr)
                        lsmdata->XlBatchCurrPtr = lsmdata->XlBatchStartPtr;
                    if (lsmdata->GyBatchCurrPtr == lsmdata->GyBatchEndPtr)
                        lsmdata->GyBatchCurrPtr = lsmdata->GyBatchStartPtr;

                    lsmdata->GyBatchCnt += 2;
                    lsmdata->XlBatchCnt += 2;
#endif
                }
            }
            else
            {
                /*
                 * fifo sample cycle will have
                 * Gy sample followed by Xl sample
                 * Xl will be in every cycle as fifo sampling
                 * rate in this case will be same as Xl.
                 */
#ifdef CONFIG_SENSOR_TIMESTAMP
                if (lsmdata->ui16XlOdr > lsmdata->ui16GyroOdr)
                    RdCycle = (lsmdata->XlBatchLen / 2);
                else
                    RdCycle = (lsmdata->GyBatchLen / 2);
#else
                RdCycle = (lsmdata->FifoTh / 2);
#endif

                for (i = 0; i < (RdCycle /
                            lsmdata->fifo_sample_fact); i = i + 3)
                {
                    for (j = 0; j < lsmdata->fifo_sample_fact; j++)
                    {
                        cycle_mask = (1 << j);
                        if (cycle_mask & lsmdata->GyCycleMask)
                        {
                            lsm6dsl_fifo_raw_data_get(lsmdata, lsmdata->GyBatchCurrPtr++);
                            if (lsmdata->GyBatchCurrPtr == lsmdata->GyBatchEndPtr)
                                lsmdata->GyBatchCurrPtr = lsmdata->GyBatchStartPtr;

                            lsm6dsl_fifo_raw_data_get(lsmdata, lsmdata->GyBatchCurrPtr++);
                            if (lsmdata->GyBatchCurrPtr == lsmdata->GyBatchEndPtr)
                                lsmdata->GyBatchCurrPtr = lsmdata->GyBatchStartPtr;

                            lsm6dsl_fifo_raw_data_get(lsmdata, lsmdata->GyBatchCurrPtr++);
                            if (lsmdata->GyBatchCurrPtr == lsmdata->GyBatchEndPtr)
                                lsmdata->GyBatchCurrPtr = lsmdata->GyBatchStartPtr;

                            lsmdata->GyBatchCnt += 3;
                        }

                        if (cycle_mask & lsmdata->XlCycleMask)
                        {
                            lsm6dsl_fifo_raw_data_get(lsmdata, lsmdata->XlBatchCurrPtr++);
                            if (lsmdata->XlBatchCurrPtr == lsmdata->XlBatchEndPtr)
                                lsmdata->XlBatchCurrPtr = lsmdata->XlBatchStartPtr;

                            lsm6dsl_fifo_raw_data_get(lsmdata, lsmdata->XlBatchCurrPtr++);
                            if (lsmdata->XlBatchCurrPtr == lsmdata->XlBatchEndPtr)
                                lsmdata->XlBatchCurrPtr = lsmdata->XlBatchStartPtr;

                            lsm6dsl_fifo_raw_data_get(lsmdata, lsmdata->XlBatchCurrPtr++);
                            if (lsmdata->XlBatchCurrPtr == lsmdata->XlBatchEndPtr)
                                lsmdata->XlBatchCurrPtr = lsmdata->XlBatchStartPtr;

                            lsmdata->XlBatchCnt += 3;

                        }

#ifdef CONFIG_SENSOR_TIMESTAMP
                        /* convert to LE format*/
                        lsm6dsl_fifo_raw_data_get(lsmdata, &tbuf[0]);
                        lsm6dsl_fifo_raw_data_get(lsmdata, &tbuf[1]);
                        lsm6dsl_fifo_raw_data_get(lsmdata, &tbuf[2]);
                        tbuf[1] = tbuf[1] >> 8;

                        if (cycle_mask & lsmdata->XlCycleMask)
                        {
                            *lsmdata->XlBatchCurrPtr++ = tbuf[1];
                            if (lsmdata->XlBatchCurrPtr == lsmdata->XlBatchEndPtr)
                                lsmdata->XlBatchCurrPtr = lsmdata->XlBatchStartPtr;
                            *lsmdata->XlBatchCurrPtr++ = tbuf[0];
                            if (lsmdata->XlBatchCurrPtr == lsmdata->XlBatchEndPtr)
                                lsmdata->XlBatchCurrPtr = lsmdata->XlBatchStartPtr;
                            lsmdata->XlBatchCnt += 2;
                        }

                        if (cycle_mask & lsmdata->GyCycleMask)
                        {
                            *lsmdata->GyBatchCurrPtr++ = tbuf[1];
                            if (lsmdata->GyBatchCurrPtr == lsmdata->GyBatchEndPtr)
                                lsmdata->GyBatchCurrPtr = lsmdata->GyBatchStartPtr;
                            *lsmdata->GyBatchCurrPtr++ = tbuf[0];
                            if (lsmdata->GyBatchCurrPtr == lsmdata->GyBatchEndPtr)
                                lsmdata->GyBatchCurrPtr = lsmdata->GyBatchStartPtr;
                            lsmdata->GyBatchCnt += 2;

                        }
#endif

                    }
                }
            }

        }
        /* Only Xl Batch */
        else if( lsmdata->XlBatchLen)
        {
#ifdef CONFIG_SENSOR_TIMESTAMP
            RdCycle = (lsmdata->FifoTh / 2);
#else
            RdCycle = (lsmdata->FifoTh);
#endif
            for (i = 0; i < RdCycle / 3; i++)
            {
                lsm6dsl_fifo_raw_data_get(lsmdata, lsmdata->XlBatchCurrPtr++);
                if (lsmdata->XlBatchCurrPtr == lsmdata->XlBatchEndPtr)
                    lsmdata->XlBatchCurrPtr = lsmdata->XlBatchStartPtr;

                lsm6dsl_fifo_raw_data_get(lsmdata, lsmdata->XlBatchCurrPtr++);
                if (lsmdata->XlBatchCurrPtr == lsmdata->XlBatchEndPtr)
                    lsmdata->XlBatchCurrPtr = lsmdata->XlBatchStartPtr;

                lsm6dsl_fifo_raw_data_get(lsmdata, lsmdata->XlBatchCurrPtr++);
                if (lsmdata->XlBatchCurrPtr == lsmdata->XlBatchEndPtr)
                    lsmdata->XlBatchCurrPtr = lsmdata->XlBatchStartPtr;

                lsmdata->XlBatchCnt += 3;
#ifdef CONFIG_SENSOR_TIMESTAMP
                /* convert to LE format*/
                lsm6dsl_fifo_raw_data_get(lsmdata, &tbuf[0]);
                lsm6dsl_fifo_raw_data_get(lsmdata, &tbuf[1]);
                lsm6dsl_fifo_raw_data_get(lsmdata, &tbuf[2]);
                *lsmdata->XlBatchCurrPtr++ = (tbuf[1] >> 8);
                if (lsmdata->XlBatchCurrPtr == lsmdata->XlBatchEndPtr)
                    lsmdata->XlBatchCurrPtr = lsmdata->XlBatchStartPtr;
                *lsmdata->XlBatchCurrPtr++ = tbuf[0];
                if (lsmdata->XlBatchCurrPtr == lsmdata->XlBatchEndPtr)
                    lsmdata->XlBatchCurrPtr = lsmdata->XlBatchStartPtr;
                lsmdata->XlBatchCnt += 2;

#endif
            }
        }
        /* only Gyro batch */
        else if( lsmdata->GyBatchLen)
        {
            /* convert byte to 3 words(16 bit) read cycle */
#ifdef CONFIG_SENSOR_TIMESTAMP
            RdCycle = (lsmdata->FifoTh / 2);
#else
            RdCycle = (lsmdata->FifoTh);
#endif
            for (i = 0; i < RdCycle / 3; i++)
            {
                lsm6dsl_fifo_raw_data_get(lsmdata, lsmdata->GyBatchCurrPtr++);
                if (lsmdata->GyBatchCurrPtr == lsmdata->GyBatchEndPtr)
                    lsmdata->GyBatchCurrPtr = lsmdata->GyBatchStartPtr;

                lsm6dsl_fifo_raw_data_get(lsmdata, lsmdata->GyBatchCurrPtr++);
                if (lsmdata->GyBatchCurrPtr == lsmdata->GyBatchEndPtr)
                    lsmdata->GyBatchCurrPtr = lsmdata->GyBatchStartPtr;

                lsm6dsl_fifo_raw_data_get(lsmdata, lsmdata->GyBatchCurrPtr++);
                if (lsmdata->GyBatchCurrPtr == lsmdata->GyBatchEndPtr)
                    lsmdata->GyBatchCurrPtr = lsmdata->GyBatchStartPtr;
                lsmdata->GyBatchCnt += 3;
#ifdef CONFIG_SENSOR_TIMESTAMP
                /* convert to LE format*/
                lsm6dsl_fifo_raw_data_get(lsmdata, &tbuf[0]);
                lsm6dsl_fifo_raw_data_get(lsmdata, &tbuf[1]);
                lsm6dsl_fifo_raw_data_get(lsmdata, &tbuf[2]);
                *lsmdata->GyBatchCurrPtr++ = (tbuf[1] >> 8);
                if (lsmdata->GyBatchCurrPtr == lsmdata->GyBatchEndPtr)
                    lsmdata->GyBatchCurrPtr = lsmdata->GyBatchStartPtr;
                *lsmdata->GyBatchCurrPtr++ = tbuf[0];
                if (lsmdata->GyBatchCurrPtr == lsmdata->GyBatchEndPtr)
                    lsmdata->GyBatchCurrPtr = lsmdata->GyBatchStartPtr;
                lsmdata->GyBatchCnt += 2;
#endif
            }
        }
        /* check for batch count to send event*/
        if(lsmdata->XlBatchLen &&
                ((lsmdata->XlBatchCnt * 2) >= lsmdata->sXlBinfo.ui16BatchLen))
        {
            if (lsmdata->XlEvMask & SENSOR_EVENT_BATCH)
            {
                EvInfo.iEvent = SENSOR_EVENT_BATCH;
                EvInfo.iSensor = SENSOR_TYPE_ACCELEROMETER;
                EvInfo.sEvData.ui8Buf = (uint8_t *)lsmdata->XlBatchPtr;
                EvInfo.sEvData.ui16BufLen = (lsmdata->XlBatchCnt * 2);
                if (lsmdata->XlBatchSt && lsmdata->fXlCb)
                {
                     ecm35xx_printf("Xl BPtr %x Cnt %d\r\n",
                                    lsmdata->XlBatchCurrPtr,
                                    lsmdata->XlBatchCnt);

                    lsmdata->fXlCb(&EvInfo, lsmdata->vXlCbptr);
                }
                lsmdata->XlBatchPtr = lsmdata->XlBatchCurrPtr;
            }
            lsmdata->XlBatchCnt = 0;
        }

        if(lsmdata->GyBatchLen && ((lsmdata->GyBatchCnt * 2) >=
                    lsmdata->sGyBinfo.ui16BatchLen))
        {
            if (lsmdata->GyEvMask & SENSOR_EVENT_BATCH)
            {
                EvInfo.iEvent = SENSOR_EVENT_BATCH;
                EvInfo.iSensor = SENSOR_TYPE_GYROSCOPE;
                EvInfo.sEvData.ui8Buf = (uint8_t *)lsmdata->GyBatchPtr;
                EvInfo.sEvData.ui16BufLen = lsmdata->GyBatchCnt * 2;

                if (lsmdata->GyBatchSt && lsmdata->fGyCb)
                {
                     ecm35xx_printf("Gy Bptr %x Cnt %d\r\n",
                                    lsmdata->GyBatchCurrPtr,
                                    lsmdata->GyBatchCnt);
                    lsmdata->fGyCb(&EvInfo, lsmdata->vGyCbptr);
                }
                lsmdata->GyBatchPtr = lsmdata->GyBatchCurrPtr;
            }
            lsmdata->GyBatchCnt = 0;
        }

    }

    xSemaphoreGive(lsmdata->DevLock);

}

static void lsm6dsl_irq(void *data)
{
    BaseType_t lHigherPriorityTaskWoken;

    xTimerPendFunctionCallFromISR (Irq_postprocess,
                        data, 0, &lHigherPriorityTaskWoken);

    if(lHigherPriorityTaskWoken == pdTRUE)
        portYIELD_FROM_ISR(lHigherPriorityTaskWoken);
}

static int32_t lsm6dsl_init(struct lsm6dsl_data *data)
{
    lsm6dsl_i2c_init(data);
    if (lsm6dsl_init_chip(data) < 0) {
        LOG_DBG("failed to initialize chip");
        return -EIO;
    }
    HalGpioIntInit(CONFIG_LSM6DSL_GPIO_IRQ, 0, lsm6dsl_irq, lsmdata,
                      HalGpioTrigHigh, HalGpioPullNone);
    return 0;
}

static tDrvHandle Lsm6dslDrvOpen(char *string)
{
    int ret = -ENODEV;
    xSemaphoreTake(lsmdata->DevLock, portMAX_DELAY);
    if (lsmdata->ui8OpenCnt)
    {
        /* Does not support multiple Instances*/
        ret = -EBUSY;
    }
    else if(!strcmp(string, CONFIG_LSM6DSL_NAME))
    {
        lsmdata->ui8OpenCnt++;
        ret = (LSM6DSL_VAL_WHO_AM_I);
    }
    xSemaphoreGive(lsmdata->DevLock);
    return (tDrvHandle)ret;
}

static int32_t Lsm6dslDrvClose(tDrvHandle sDrvHandle)
{
    int32_t ret = 0;
    xSemaphoreTake(lsmdata->DevLock, portMAX_DELAY);
    if(sDrvHandle != LSM6DSL_VAL_WHO_AM_I)
        ret = -EPIPE;
    else if (lsmdata->ui8OpenCnt)
        lsmdata->ui8OpenCnt--;
    xSemaphoreGive(lsmdata->DevLock);
    return ret;
}

static int32_t
Lsm6dslDrvRead(tDrvHandle sDrvHandle, tSensorType iSensor,
                        uint8_t *ui8Buf, uint8_t ui8Len)
{
    int32_t ret = 0;
    xSemaphoreTake(lsmdata->DevLock, portMAX_DELAY);
    if(!lsmdata->ui8OpenCnt ||
            (sDrvHandle != (LSM6DSL_VAL_WHO_AM_I)))
        ret = -EPIPE;
    else
        lsm6dsl_sample_fetch(lsmdata, iSensor, ui8Buf, ui8Len);
    xSemaphoreGive(lsmdata->DevLock);
    return ret;
}

static int32_t
Lsm6dslDrvIoctl(tSDrvHandle sDrvHandle, tSensorType iSensor,
                    tSensorIOCTL iCmd, void *vPtr)
{
    int32_t ret = 0;
    int32_t val;
    int32_t odr;

    if(!lsmdata->ui8OpenCnt ||
            (sDrvHandle != (LSM6DSL_VAL_WHO_AM_I)))
        return -EPIPE;

    xSemaphoreTake(lsmdata->DevLock, portMAX_DELAY);
    switch (iCmd)
    {
        case IOCTL_SDEV_SET_ODR:
        {
            val = *(int32_t *)vPtr;
            odr = findClosestIndex(odr_map,
                                    ARRAY_SIZE(odr_map), (uint16_t)val);
            if (iSensor == SENSOR_TYPE_ACCELEROMETER)
            {
                lsm6dsl_accel_data_rate_set(lsmdata, odr);
            }
            else if (iSensor ==  SENSOR_TYPE_GYROSCOPE)
            {
                lsm6dsl_gyro_data_rate_set(lsmdata, odr);
            }
            else
            {
                ret = -EOPNOTSUPP;
            }

            break;
        }

        case IOCTL_SDEV_GET_ODR:
        {
            if (iSensor == SENSOR_TYPE_ACCELEROMETER)
            {
                *((int32_t *)vPtr) = lsm6dsl_accel_data_rate_get(lsmdata);
            }
            else if (iSensor ==  SENSOR_TYPE_GYROSCOPE)
            {
                *((int32_t *)vPtr) = lsm6dsl_gyro_data_rate_get(lsmdata);
            }
            else
                ret = -EOPNOTSUPP;

            break;
        }

        case IOCTL_SDEV_BATCH_BUFFER_INFO:
        {
            tSensorBufInfo *batch_info;

            if (iSensor == SENSOR_TYPE_ACCELEROMETER)
            {
                batch_info = (tSensorBufInfo *)vPtr;
                if ((!batch_info->ui8Buf) ||
                        (!batch_info->ui16BufLen))
                {
                    ret = -EINVAL;
                }
                else
                {
                    lsmdata->sXlBinfo.ui8Buf =
                            batch_info->ui8Buf;
                    lsmdata->XlBatchPtr = lsmdata->XlBatchStartPtr =
                        lsmdata->XlBatchCurrPtr =
                            (uint16_t *)batch_info->ui8Buf;
                    lsmdata->sXlBinfo.ui16BufLen = batch_info->ui16BufLen;
                    lsmdata->sXlBinfo.ui16BatchLen = batch_info->ui16BatchLen;
                    lsmdata->XlBatchEndPtr = (uint16_t *)(batch_info->ui8Buf +
                                                batch_info->ui16BufLen);
                    ecm35xx_printf("-- Batch Info %x Batch L %d Buff L %d\r\n",
                                batch_info->ui8Buf, batch_info->ui16BatchLen,
                                batch_info->ui16BufLen);
                    lsmdata->XlBatchCnt = 0;
                }
            }
            if (iSensor == SENSOR_TYPE_GYROSCOPE)
            {
                batch_info = (tSensorBufInfo *)vPtr;
                if ((!batch_info->ui8Buf) ||
                        (!batch_info->ui16BufLen))
                {
                    ret = -EINVAL;
                }
                else
                {
                    lsmdata->sGyBinfo.ui8Buf =
                        batch_info->ui8Buf;
                    lsmdata->GyBatchPtr = lsmdata->GyBatchStartPtr =
                        lsmdata->GyBatchCurrPtr =
                        (uint16_t *)batch_info->ui8Buf;
                    lsmdata->sGyBinfo.ui16BufLen = batch_info->ui16BufLen;
                    lsmdata->sGyBinfo.ui16BatchLen = batch_info->ui16BatchLen;
                    lsmdata->GyBatchEndPtr = (uint16_t *)(batch_info->ui8Buf +
                            batch_info->ui16BufLen);
                    lsmdata->GyBatchCnt = 0;
                }
            }
            break;
        }
        case IOCTL_SDEV_SET_FULL_SCALE:
        {
            val = *(int32_t *)vPtr;
            if (iSensor == SENSOR_TYPE_ACCELEROMETER)
            {
                lsm6dsl_accel_fs_set(lsmdata, val);
            }
            else if (iSensor ==  SENSOR_TYPE_GYROSCOPE)
            {
                lsm6dsl_gy_fs_set(lsmdata, val);
            }
            else
                ret = -EOPNOTSUPP;

            break;
        }

        case IOCTL_SDEV_BATCH_ENABLE:
        {
            if (iSensor == SENSOR_TYPE_ACCELEROMETER)
            {
                if (!lsmdata->XlBatchSt)
                    lsm6dsl_batch_enable(iSensor);
            }
            else if (iSensor == SENSOR_TYPE_GYROSCOPE)
            {
                if (!lsmdata->GyBatchSt)
                    lsm6dsl_batch_enable(iSensor);
            }

            break;
        }
        case IOCTL_SDEV_BATCH_DISABLE:
        {
            if (iSensor ==  SENSOR_TYPE_ACCELEROMETER)
                lsmdata->XlBatchSt = 0;
            else if (iSensor ==  SENSOR_TYPE_GYROSCOPE)
                lsmdata->GyBatchSt = 0;
            if ((!lsmdata->XlBatchSt) && (!lsmdata->GyBatchSt))
            {
                lsmdata->hw_tf->update_reg(lsmdata, LSM6DSL_REG_INT1_CTRL,
                                LSM6DSL_MASK_INT1_FTH, 0);
                lsm6dsl_fifo_mode_set(lsmdata, LSM6DSL_BYPASS_MODE);
            }
            break;

        }

        case IOCTL_SDEV_BATCH_FLUSH:
        {
            /*
            if (iSensor ==  SENSOR_TYPE_ACCELEROMETER)
            {

                lsmdata->XlBatchCnt = 0;
                lsmdata->XlBatchCurrPtr =
                    lsmdata->XlBatchPtr = lsmdata->XlBatchStartPtr;
            }
            if (iSensor ==  SENSOR_TYPE_GYROSCOPE)
            {
                lsmdata->GyBatchCnt = 0;
                lsmdata->GyBatchCurrPtr =
                    lsmdata->GyBatchPtr = lsmdata->GyBatchStartPtr;

            }
            */
            break;
        }

        case IOCTL_SDEV_LIVE_DATA_ENABLE:
        {
            /*
             * Enable Data ready INT
             * if Live Data is requested
             */

            if (iSensor == SENSOR_TYPE_ACCELEROMETER)
            {
                if (lsmdata->XlEvMask & SENSOR_EVENT_LIVE_DATA)
                {
                    lsmdata->hw_tf->update_reg(lsmdata, LSM6DSL_REG_INT1_CTRL,
                            LSM6DSL_MASK_INT1_CTRL_DRDY_XL,
                            (1 << LSM6DSL_SHIFT_INT1_CTRL_DRDY_XL));
                }
            }
            else if (iSensor == SENSOR_TYPE_GYROSCOPE)
            {
                if (lsmdata->GyEvMask & SENSOR_EVENT_LIVE_DATA)
                {
                    lsmdata->hw_tf->update_reg(lsmdata, LSM6DSL_REG_INT1_CTRL,
                            LSM6DSL_MASK_INT1_CTRL_DRDY_G,
                            (1 << LSM6DSL_SHIFT_INT1_CTRL_DRDY_G));
                }
            }

            break;
        }

        case IOCTL_SDEV_LIVE_DATA_DISABLE:
        {

            if (iSensor == SENSOR_TYPE_ACCELEROMETER)
            {
                lsmdata->hw_tf->update_reg(lsmdata, LSM6DSL_REG_INT1_CTRL,
                        LSM6DSL_MASK_INT1_CTRL_DRDY_XL, 0);
            }
            else if (iSensor == SENSOR_TYPE_GYROSCOPE)
            {
                lsmdata->hw_tf->update_reg(lsmdata, LSM6DSL_REG_INT1_CTRL,
                                LSM6DSL_MASK_INT1_CTRL_DRDY_G, 0);
            }

            break;
        }

        case IOCTL_SDEV_GET_SUPPORTED_SENSOR_TYPES:
        {
            tSensorTypeInfo *sInfo = (tSensorTypeInfo *)vPtr;
            sInfo->ui8Cnt = 2;
            sInfo->iType[0]= SENSOR_TYPE_GYROSCOPE;
            sInfo->iType[1]= SENSOR_TYPE_ACCELEROMETER;
            break;
        }

        default:
            break;
    }
    xSemaphoreGive(lsmdata->DevLock);
    return ret;
}

/*
 * Set Event callback
 */
int32_t Lsm6dslDrvSetEvCb(tDrvHandle sDrvHandle, tSensorType iSensor,
                        tSensorEventCb fEvCb, tSensorEvent iEvMask,
                        void *vCbData)
{
    int32_t ret = 0;
    xSemaphoreTake(lsmdata->DevLock, portMAX_DELAY);
    if(!lsmdata->ui8OpenCnt ||
            (sDrvHandle != (LSM6DSL_VAL_WHO_AM_I)))
        ret = -EPIPE;
    else
    {
        if (iSensor == SENSOR_TYPE_ACCELEROMETER)
        {
            lsmdata->fXlCb = fEvCb;
            lsmdata->vXlCbptr = vCbData;
            lsmdata->XlEvMask = iEvMask;
        }
        else if (iSensor == SENSOR_TYPE_GYROSCOPE)
        {
            lsmdata->fGyCb = fEvCb;
            lsmdata->vGyCbptr = vCbData;
            lsmdata->GyEvMask = iEvMask;
        }
    }
    xSemaphoreGive(lsmdata->DevLock);
    return ret;
}

/*
 * Will be called from SM Init
 * will call driver Init
 */
int32_t Lsm6dslDrvProbe()
{
    int32_t ret;
    xSemaphoreTake(lsmdata->DevLock, portMAX_DELAY);
    ret = lsm6dsl_init(lsmdata);
    xSemaphoreGive(lsmdata->DevLock);
    return ret;
}

/*
 * Driver Fops linked with SM
 */
tSensorDrvOps sLsm6dslDevOps = {
    .DrvOpen = Lsm6dslDrvOpen,
    .DrvProbe = Lsm6dslDrvProbe,
    .DrvClose = Lsm6dslDrvClose,
    .DrvRead = Lsm6dslDrvRead,
    .DrvIoctl = Lsm6dslDrvIoctl,
    .DrvSetCb = Lsm6dslDrvSetEvCb,
};

/*
 * This will register Driver with SM
 */
void Lsm6dslDrvInit(void)
{
#ifndef CONFIG_EVB_BOARD
    HalGpioOutInit(CONFIG_LSM6DSL_GPIO_USERIRQ, 1);
#endif
    lsmdata = pvPortMalloc(sizeof(struct lsm6dsl_data));
    memset(lsmdata,0,sizeof(struct lsm6dsl_data));
    SensorDrvRegister(CONFIG_LSM6DSL_NAME, &sLsm6dslDevOps);
    lsmdata->DevLock = xSemaphoreCreateMutex();
}

void Lsm6dslDrvDeInit(void)
{
    HalGpioIntDisable(0, CONFIG_LSM6DSL_GPIO_IRQ);
    SensorDrvDeRegister(CONFIG_LSM6DSL_NAME);
    vSemaphoreDelete(lsmdata->DevLock);
    vPortFree(lsmdata);
}
