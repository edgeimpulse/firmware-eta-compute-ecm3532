/***************************************************************************//**
 *
 * @file icm20602.c
 *
 * @brief InvenSense Accel + Gyro sensor
 *
 * Copyright (C) 2020 Eta Compute, Inc
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
 * This is part of revision ${version} of the Tensai Software Development Kit.
 *
 ******************************************************************************/
#include "icm20602.h"
#include "spi_hal.h"
#define LOG_DBG ecm35xx_printf
#define UNUSED     __attribute__ ((unused))

struct icm20602_data *icmdata;


/**
  * @brief  Accel full-scale selection.[set]
  *
  * @param  data    Read / write interface definitions
  * @param  val    configure full scale/range through reg ACCEL_CONFIG
  * @retval        Interface status (MANDATORY: return 0 -> no Error).
  *
  */
static int32_t icm20602_accel_fs_set(struct  icm20602_data *data, icm20602_fs_xl_t val)
{
    data->hw_tf->update_reg(data, ICM20602_ACCEL_CONFIG, BITS_ACCEL_FS_SEL , val << BIT_POS_ACCEL_FS_SEL);
    return 0;
}

/**
  * @brief  Gyroscope full-scale selection.[set]
  *
  * @param  data    Read / write interface definitions
  * @param  val    configure full scale/range through reg GYRO_CONFIG
  * @retval        Interface status (MANDATORY: return 0 -> no Error).
  *
  */
static int32_t icm20602_gy_fs_set(struct icm20602_data *data, icm20602_fs_g_t val)
{
    data->hw_tf->update_reg(data, ICM20602_GYRO_CONFIG, BITS_GYRO_FS_SEL, val << BIT_POS_GYRO_FS_SEL);
    return 0;
}

/**
  * @brief  Accelerometer high data rate selection.[set]
  *
  * @param  data    Read / write interface definitions
  * @param  val    high ODR setting for accel through reg ACCEL_CONFIG2
  * @retval        Interface status (MANDATORY: return 0 -> no Error).
  *
  */
static int32_t icm20602_accel_data_rate_set_higher_odr(struct icm20602_data* data,
    uint16_t odr)
{
    uint16_t sr_divider;
    uint8_t temp;
    data->icm20602Odr = odr;
    data->ui16XlOdr = odr;
    if (data->ui16GyroOdr)
    {
        data->ui16GyroOdr = odr;  //Configuring sample rate divider will also change gyro odr.
    }

    sr_divider = 0;
    data->hw_tf->write_data(data, ICM20602_SMPLRT_DIV, sr_divider);

    //Enable accelerometer
    data->hw_tf->read_reg(data, ICM20602_PWR_MGMT_2, &temp);
    temp &= ~BIT_PWR_ACCEL_STBY;
    data->hw_tf->write_data(data, ICM20602_PWR_MGMT_2, temp);

    //set higher odr (4KHz) for accel
    data->hw_tf->write_data(data, ICM20602_ACCEL_CONFIG2, ICM20602_HIGH_SAMPLE_RATE_4000HZ);

    return 0;
}

/**
  * @brief  Accelerometer data rate selection.[set]
  *
  * @param  data    Read / write interface definitions
  * @param  val    ODR setting for accel through reg ICM20602_SMPLRT_DIV
  * @retval        Interface status (MANDATORY: return 0 -> no Error).
  *
  */
static int32_t icm20602_accel_data_rate_set(struct icm20602_data *data,
                                        uint16_t odr)
{
    uint16_t sr_divider;
    uint8_t temp;
    data->icm20602Odr = odr;
    data->ui16XlOdr = odr;
    if (data->ui16GyroOdr)
    {
        data->ui16GyroOdr = odr;  //Configuring sample rate divider will also change gyro odr.
    }

    sr_divider = (1000 / odr) - 1;
    //Configure sample rate divider. Resultant ODR = 1000/(sr_divider+1)
    data->hw_tf->write_data(data, ICM20602_SMPLRT_DIV, sr_divider);
    //Enable accelerometer
    data->hw_tf->read_reg(data, ICM20602_PWR_MGMT_2, &temp);
    temp &= ~BIT_PWR_ACCEL_STBY;
    data->hw_tf->write_data(data, ICM20602_PWR_MGMT_2, temp);

    return 0;
}

/**
  * @brief  Accelerometer data rate selection.[get]
  *
  * @param  data    Read / write interface definitions
  * @param  val    return current configured ODR from reg ICM20602_SMPLRT_DIV
  * @retval        Interface status (MANDATORY: return 0 -> no Error).
  *
  */
static int32_t icm20602_accel_data_rate_get(struct icm20602_data *data)
{
    uint16_t odr;
    uint8_t sr_divider;

///////////higher odr case
    if (data->icm20602Odr > 1000)
    {
        uint8_t temp;
        data->hw_tf->read_reg(data, ICM20602_ACCEL_CONFIG2, &temp);

        temp = temp & BIT_ACCEL_FCHOICE_B;
        if (temp == 0)
        {
            LOG_DBG("Warning issue in get accel odr expected = higher ODR but ACCEL_FCHOICE_B is not set \r\n");
        }
        return data->icm20602Odr;
    }
/////////////////


    data->hw_tf->read_reg(data, ICM20602_SMPLRT_DIV, &sr_divider);
    odr = 1000 / (sr_divider + 1);

    if (data->icm20602Odr != odr)
    {
        LOG_DBG("Warning issue in get accel odr expected = %d, actual = %d \r\n", data->icm20602Odr, odr);
    }

    return odr;
}

/**
  * @brief  Gyro data rate selection.[set]
  *
  * @param  data    Read / write interface definitions
  * @param  val    ODR setting for accel through reg ICM20602_SMPLRT_DIV
  * @retval        Interface status (MANDATORY: return 0 -> no Error).
  *
  */
static int32_t
icm20602_gyro_data_rate_set(struct icm20602_data *data, uint16_t odr)
{
    uint16_t sr_divider;
    uint8_t temp;
    data->icm20602Odr = odr;
    data->ui16GyroOdr = odr;
    if (data->ui16XlOdr)
    {
        data->ui16XlOdr = odr;  //Configuring sample rate divider will also change accel odr.
    }

    sr_divider = (1000 / odr) - 1;
    //Configure sample rate divider. Resultant ODR = 1000/(sr_divider+1)
    data->hw_tf->write_data(data, ICM20602_SMPLRT_DIV, sr_divider);
    //Enable accelerometer
    data->hw_tf->read_reg(data, ICM20602_PWR_MGMT_2, &temp);
    temp &= ~BIT_PWR_GYRO_STBY;
    data->hw_tf->write_data(data, ICM20602_PWR_MGMT_2, temp);
    return 0;
}

/**
  * @brief  Gyro data rate selection.[get]
  *
  * @param  data    Read / write interface definitions
  * @param  val    return current configured ODR from reg ICM20602_SMPLRT_DIV
  * @retval        Interface status (MANDATORY: return 0 -> no Error).
  *
  */

static int32_t
icm20602_gyro_data_rate_get(struct icm20602_data *data)
{
    uint16_t odr;
    uint8_t sr_divider;

    data->hw_tf->read_reg(data, ICM20602_SMPLRT_DIV, &sr_divider);
    odr = 1000 / (sr_divider + 1);

    if (data->icm20602Odr != odr)
    {
        LOG_DBG("Warning issue in get gyro odr expected = %d, actual = %d \r\n", data->icm20602Odr, odr);
    }

    return odr;
}

static int32_t
icm20602_sample_fetch_accel(struct icm20602_data *data,
                            uint8_t *buf, uint8_t len)
{
    uint8_t pui8Bytes[6];
    if (len < 6)
        return -ENOMEM;

    data->hw_tf->read_data(data, ICM20602_ACCEL_XOUT_H, pui8Bytes, 6);

    //swap big endian to little endian
    buf[0] = pui8Bytes[1];  buf[1] = pui8Bytes[0];
    buf[2] = pui8Bytes[3];  buf[3] = pui8Bytes[2];
    buf[4] = pui8Bytes[5];  buf[5] = pui8Bytes[4];

    return 0;
}

static int32_t
icm20602_sample_fetch_gyro(struct icm20602_data *data,
                            uint8_t *buf, uint8_t len)
{
    uint8_t pui8Bytes[6];
    if (len < 6)
        return -ENOMEM;

    data->hw_tf->read_data(data, ICM20602_GYRO_XOUT_H, pui8Bytes, 6);

    //swap big endian to little endian
    buf[0] = pui8Bytes[1];  buf[1] = pui8Bytes[0];
    buf[2] = pui8Bytes[3];  buf[3] = pui8Bytes[2];
    buf[4] = pui8Bytes[5];  buf[5] = pui8Bytes[4];

    return 0;
}

static int32_t
icm20602_sample_fetch(struct icm20602_data *data,
                        tSensorType chan, uint8_t *buf, uint8_t len)
{
    switch (chan)
    {
    case SENSOR_TYPE_ACCELEROMETER:
        icm20602_sample_fetch_accel(data, buf, len);
        break;
    case SENSOR_TYPE_GYROSCOPE:
        icm20602_sample_fetch_gyro(data, buf, len);
        break;
    default:
        return -EOPNOTSUPP;
    }

    return 0;
}

static int32_t icm20602_init_chip(struct icm20602_data *data)
{
    uint8_t chip_id;
    uint8_t val;

    data->hw_tf->write_data(data, ICM20602_PWR_MGMT_1, 0x80);
    //vTaskDelay(1000);
    HalTmrDelay(0, 50);

    if (data->hw_tf->read_reg(data,
        ICM20602_WHO_AM_I, (uint8_t*)&chip_id) < 0) {
        LOG_DBG("failed reading chip id");
        return -EIO;
    }
    if (chip_id != ICM20602_WHO_AM_I_VAL) {
        LOG_DBG("invalid chip id 0x%x", chip_id);
        return -EIO;
    }
    LOG_DBG("chip id 0x%x\r\n", chip_id);

    //
    // Set to PLL.
    //
    data->hw_tf->write_data(data, ICM20602_PWR_MGMT_1, 0x01);

    //
    // Put accel and gyro in standby.
    //
    data->hw_tf->write_data(data, ICM20602_PWR_MGMT_2, 0x3f);

    //
    // Disable FIFO
    //
    data->hw_tf->write_data(data, ICM20602_USER_CTRL, 0x00);

    //
    // Disable I2C intergace
    //
    data->hw_tf->write_data(data, ICM20602_I2C_IF, I2C_IF_DIS);

    //
    // Gyro config.
    //
    data->hw_tf->write_data(data, ICM20602_GYRO_CONFIG,
        (ICM20602_GYRO_250DPS | ICM20602_GYRO_DLPF_ENABLE));
    data->hw_tf->write_data(data, ICM20602_CONFIG, ICM20602_GYRO_DLPF6_NBW8HZ);

    //
    // Accel config.
    //
    data->hw_tf->write_data(data, ICM20602_ACCEL_CONFIG, ICM20602_ACCEL_8G);
    data->hw_tf->write_data(data, ICM20602_ACCEL_CONFIG2, ICM20602_ACCEL_DLPF5_NBW16HZ);

#if 0
    //
    // Enable accel and gyro.
    //
    data->hw_tf->write_data(data, ICM20602_PWR_MGMT_2,
        ICM20602_PWN2_ENABLE_ACCE | ICM20602_PWN2_ENABLE_GYRO);

    //
    // Set sample rate 100Hz
    //
    data->hw_tf->write_data(data, ICM20602_SMPLRT_DIV, ICM20602_SAMPLE_RATE_100HZ);
    data->icm20602Odr = 100;
    data->sampleRateDivider = ICM20602_SAMPLE_RATE_100HZ;
#else
    //
    // Disable accel and gyro.
    //
    data->hw_tf->write_data(data, ICM20602_PWR_MGMT_2, BIT_PWR_ALL_OFF);
    data->icm20602Odr = 0;
    data->icmXlLiveDataEnable = 0;
    data->icmGyroLiveDataEnable = 0;

#endif
    return 0;
}

/*
 * deferred ISR
 */
static void Irq_postprocess( void *data, uint32_t param)
{
    uint16_t tbuf[3];
    uint8_t reg = 0;
    tSensorEventInfo EvInfo;
    uint8_t cycle_mask;
    struct icm20602_data *icmdata = (struct icm20602_data *)data;

    xSemaphoreTake(icmdata->DevLock, portMAX_DELAY);

    do
    {
        /* live data, data ready ISR */
        if (icmdata->XlEvMask & SENSOR_EVENT_LIVE_DATA ||
                icmdata->GyEvMask & SENSOR_EVENT_LIVE_DATA)
        {
            icmdata->hw_tf->read_reg(icmdata,
                ICM20602_INT_STATUS, &reg);
            /*
             * if XL Live Data Event is enable and Data is available
             */
            if((icmdata->XlEvMask & SENSOR_EVENT_LIVE_DATA) &&
                    (reg & BIT_DATA_RDY_INT))
            {
                if (icmdata->icmXlLiveDataEnable)
                {
                    icm20602_sample_fetch_accel(icmdata, (uint8_t*)tbuf, 6);
                    EvInfo.iEvent = SENSOR_EVENT_LIVE_DATA;
                    EvInfo.iSensor = SENSOR_TYPE_ACCELEROMETER;
                    EvInfo.sEvData.ui8Buf = (uint8_t*)tbuf;
                    EvInfo.sEvData.ui16BufLen = 6;
                    icmdata->fXlCb(&EvInfo, icmdata->vXlCbptr);
                }
            }
            /*
             * if Gy Live Data Event is enable and Data is available
             */
            if((icmdata->GyEvMask & SENSOR_EVENT_LIVE_DATA) &&
                    (reg & BIT_DATA_RDY_INT))
            {
                if (icmdata->icmGyroLiveDataEnable)
                {
                    icm20602_sample_fetch_gyro(icmdata, (uint8_t*)tbuf, 6);
                    EvInfo.iEvent = SENSOR_EVENT_LIVE_DATA;
                    EvInfo.iSensor = SENSOR_TYPE_GYROSCOPE;
                    EvInfo.sEvData.ui8Buf = (uint8_t*)tbuf;
                    EvInfo.sEvData.ui16BufLen = 6;
                    icmdata->fGyCb(&EvInfo, icmdata->vGyCbptr);
                }
            }
            icmdata->hw_tf->read_reg(icmdata,
                ICM20602_INT_STATUS, &reg);
        }
    } while(reg & BIT_DATA_RDY_INT);

    /* Batching Fifo watermark ISR */
    //Not yet supported.

    xSemaphoreGive(icmdata->DevLock);

}

static void icm20602_irq(void *data)
{
    BaseType_t lHigherPriorityTaskWoken;

    xTimerPendFunctionCallFromISR (Irq_postprocess,
                        data, 0, &lHigherPriorityTaskWoken);

    if(lHigherPriorityTaskWoken == pdTRUE)
        portYIELD_FROM_ISR(lHigherPriorityTaskWoken);
}

static int32_t icm20602_init(struct icm20602_data *data)
{
    icm20602_spi_init(data);
    if (icm20602_init_chip(data) < 0) {
        LOG_DBG("failed to initialize chip");
        return -EIO;
    }

    HalGpioIntInit(CONFIG_ICM20602_GPIO_IRQ, 0, icm20602_irq, icmdata,
        HalGpioTrigHigh, HalGpioPullNone);
    data->hw_tf->write_data(data, ICM20602_INT_PIN_CFG, INT_PIN_CFG_VAL);

    return 0;
}

static tDrvHandle icm20602DrvOpen(char *string)
{
    int ret = -ENODEV;
    xSemaphoreTake(icmdata->DevLock, portMAX_DELAY);
    if (icmdata->ui8OpenCnt)
    {
        /* Does not support multiple Instances*/
        ret = -EBUSY;
    }
    else if(!strcmp(string, CONFIG_ICM20602_NAME))
    {
        icmdata->ui8OpenCnt++;
        ret = (ICM20602_WHO_AM_I_VAL);
    }
    xSemaphoreGive(icmdata->DevLock);
    return (tDrvHandle)ret;
}

static int32_t icm20602DrvClose(tDrvHandle sDrvHandle)
{
    int32_t ret = 0;
    xSemaphoreTake(icmdata->DevLock, portMAX_DELAY);
    if(sDrvHandle != ICM20602_WHO_AM_I_VAL)
        ret = -EPIPE;
    else if (icmdata->ui8OpenCnt)
        icmdata->ui8OpenCnt--;
    xSemaphoreGive(icmdata->DevLock);
    return ret;
}

static int32_t
icm20602DrvRead(tDrvHandle sDrvHandle, tSensorType iSensor,
                        uint8_t *ui8Buf, uint8_t ui8Len)
{
    int32_t ret = 0;
    xSemaphoreTake(icmdata->DevLock, portMAX_DELAY);
    if(!icmdata->ui8OpenCnt ||
            (sDrvHandle != (ICM20602_WHO_AM_I_VAL)))
        ret = -EPIPE;
    else
        icm20602_sample_fetch(icmdata, iSensor, ui8Buf, ui8Len);
    xSemaphoreGive(icmdata->DevLock);
    return ret;
}

static int32_t
icm20602DrvIoctl(tSDrvHandle sDrvHandle, tSensorType iSensor,
                    tSensorIOCTL iCmd, void *vPtr)
{
    int32_t ret = 0;
    int32_t val;
    int32_t odr;

    if(!icmdata->ui8OpenCnt ||
            (sDrvHandle != (ICM20602_WHO_AM_I_VAL)))
        return -EPIPE;

    xSemaphoreTake(icmdata->DevLock, portMAX_DELAY);
    switch (iCmd)
    {
        case IOCTL_SDEV_SET_ODR:
        {
            uint16_t sr_divider_plus1;
            val = *(int32_t *)vPtr;
            odr = (uint16_t)val;
            if (odr > 4000)
            {
                //Issue in odr settings.
                ecm35xx_printf("\n\r ODR settings > 4000 not support \n\r");
                return -EINVAL;
            }
            if (odr > 1000)
            {
                if (iSensor == SENSOR_TYPE_GYROSCOPE)
                {
                    //Issue in odr settings.
                    ecm35xx_printf("\n\r Gyro for high ODR settings not supported\n\r");
                    return -EINVAL;
                }
                if (iSensor == SENSOR_TYPE_ACCELEROMETER)
                {
                    odr = 4000;  //only 4kHz high rate accel odr is valid configuration.
                    ecm35xx_printf("\n\r setting accel odr = %d\n\r", odr);
                    icm20602_accel_data_rate_set_higher_odr(icmdata, odr);
                }
            }
            else if (odr)
            {
                //if we already set higher accel odr, then we cannot handle lower gyro odr. report error.
                if (icmdata->icm20602Odr > 1000)
                {
                    //Issue in odr settings.
                    ecm35xx_printf("\n\r High rate accel ODR and gyro together not supported \n\r");
                    return -EINVAL;
                }

                if (odr < 4)
                    odr = 4;  //minimum supported odr is 1000/255 == 4
                if (odr < icmdata->icm20602Odr)
                    odr = icmdata->icm20602Odr;

                sr_divider_plus1 = 1000 / odr;
                odr = 1000 / sr_divider_plus1;  //Actual ODR set
                icmdata->icm20602Odr = odr;  //save current icm sensor odr setting.
                if (iSensor == SENSOR_TYPE_ACCELEROMETER)
                {
                    ecm35xx_printf("\n\r setting accel odr = %d\n\r", odr);
                    icm20602_accel_data_rate_set(icmdata, odr);
                }
                else if (iSensor == SENSOR_TYPE_GYROSCOPE)
                {
                    ecm35xx_printf("\n\r setting gyro odr = %d\n\r", odr);
                    icm20602_gyro_data_rate_set(icmdata, odr);
                }
                else
                {
                    ret = -EOPNOTSUPP;
                }
            }
            break;
        }

        case IOCTL_SDEV_GET_ODR:
        {
            if (iSensor == SENSOR_TYPE_ACCELEROMETER)
            {
                if (icmdata->ui16XlOdr)
                    *((int32_t*)vPtr) = icm20602_accel_data_rate_get(icmdata);
                else
                    *((int32_t*)vPtr) = 0;
            }
            else if (iSensor ==  SENSOR_TYPE_GYROSCOPE)
            {
                if (icmdata->ui16GyroOdr)
                    *((int32_t*)vPtr) = icm20602_gyro_data_rate_get(icmdata);
                else
                    *((int32_t*)vPtr) = 0;
            }
            else
                ret = -EOPNOTSUPP;
            break;
        }
        case IOCTL_SDEV_BATCH_BUFFER_INFO:
        {
            LOG_DBG("Error: Batch Buffer Info not implemented\n\r");
            break;
        }
        case IOCTL_SDEV_SET_FULL_SCALE:
        {
            val = *(int32_t *)vPtr;
            if (iSensor == SENSOR_TYPE_ACCELEROMETER)
            {
                icm20602_accel_fs_set(icmdata, val);
            }
            else if (iSensor ==  SENSOR_TYPE_GYROSCOPE)
            {
                icm20602_gy_fs_set(icmdata, val);
            }
            else
                ret = -EOPNOTSUPP;
            break;
        }

        case IOCTL_SDEV_BATCH_ENABLE:
        {
            LOG_DBG("Error: Batch Enable not implemented\n\r");
            break;
        }
        case IOCTL_SDEV_BATCH_DISABLE:
        {
            LOG_DBG("Error: Batch Disable not implemented\n\r");
            break;
        }

        case IOCTL_SDEV_BATCH_FLUSH:
        {
            LOG_DBG("Error: Batch Flush not implemented\n\r");
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
                if (icmdata->XlEvMask & SENSOR_EVENT_LIVE_DATA)
                {
                    icmdata->hw_tf->update_reg(icmdata, ICM20602_INT_ENABLE, BITS_DATA_RDY_INT_SEL, BIT_DATA_RDY_INT_EN);
                    icmdata->icmXlLiveDataEnable = 1;
                }
            }
            else if (iSensor == SENSOR_TYPE_GYROSCOPE)
            {
                if (icmdata->GyEvMask & SENSOR_EVENT_LIVE_DATA)
                {
                    icmdata->hw_tf->update_reg(icmdata, ICM20602_INT_ENABLE, BITS_DATA_RDY_INT_SEL, BIT_DATA_RDY_INT_EN);
                    icmdata->icmGyroLiveDataEnable = 1;
                }
            }

            break;
        }

        case IOCTL_SDEV_LIVE_DATA_DISABLE:
        {
            if (iSensor == SENSOR_TYPE_ACCELEROMETER)
            {
                icmdata->icmXlLiveDataEnable = 0;
            }
            else if (iSensor == SENSOR_TYPE_GYROSCOPE)
            {
                icmdata->icmGyroLiveDataEnable = 0;
            }
            if ((0 == icmdata->icmGyroLiveDataEnable) && (0 == icmdata->icmGyroLiveDataEnable))
            {
                //When both accel and gyro are disabled then disable interrupt for data ready.
                icmdata->hw_tf->update_reg(icmdata, ICM20602_INT_ENABLE, BITS_DATA_RDY_INT_SEL, ~BIT_DATA_RDY_INT_EN);
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
    xSemaphoreGive(icmdata->DevLock);
    return ret;
}

/*
 * Set Event callback
 */
int32_t icm20602DrvSetEvCb(tDrvHandle sDrvHandle, tSensorType iSensor,
                        tSensorEventCb fEvCb, tSensorEvent iEvMask,
                        void *vCbData)
{
    int32_t ret = 0;
    xSemaphoreTake(icmdata->DevLock, portMAX_DELAY);
    if(!icmdata->ui8OpenCnt ||
            (sDrvHandle != (ICM20602_WHO_AM_I_VAL)))
        ret = -EPIPE;
    else
    {
        if (iSensor == SENSOR_TYPE_ACCELEROMETER)
        {
            icmdata->fXlCb = fEvCb;
            icmdata->vXlCbptr = vCbData;
            icmdata->XlEvMask = iEvMask;
        }
        else if (iSensor == SENSOR_TYPE_GYROSCOPE)
        {
            icmdata->fGyCb = fEvCb;
            icmdata->vGyCbptr = vCbData;
            icmdata->GyEvMask = iEvMask;
        }
    }
    xSemaphoreGive(icmdata->DevLock);
    return ret;
}

/*
 * Will be called from SM Init
 * will call driver Init
 */
int32_t icm20602DrvProbe()
{
    int32_t ret;
    xSemaphoreTake(icmdata->DevLock, portMAX_DELAY);
    ret = icm20602_init(icmdata);
    xSemaphoreGive(icmdata->DevLock);
    return ret;
}

/*
 * Driver Fops linked with SM
 */
tSensorDrvOps sicm20602DevOps = {
    .DrvOpen = icm20602DrvOpen,
    .DrvProbe = icm20602DrvProbe,
    .DrvClose = icm20602DrvClose,
    .DrvRead = icm20602DrvRead,
    .DrvIoctl = icm20602DrvIoctl,
    .DrvSetCb = icm20602DrvSetEvCb,
};

/*
 * This will register Driver with SM
 */
void icm20602DrvInit(void)
{
    icmdata = pvPortMalloc(sizeof(struct icm20602_data));
    memset(icmdata,0,sizeof(struct icm20602_data));
    SensorDrvRegister(CONFIG_ICM20602_NAME, &sicm20602DevOps);
    icmdata->DevLock = xSemaphoreCreateMutex();
}

void icm20602DrvDeInit(void)
{
    //HalGpioIntDisable(0, CONFIG_ICM20602_GPIO_IRQ);
    SensorDrvDeRegister(CONFIG_ICM20602_NAME);
    vSemaphoreDelete(icmdata->DevLock);
    vPortFree(icmdata);
}
