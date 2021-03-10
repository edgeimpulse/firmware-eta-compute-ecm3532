/***************************************************************************//**
 *
 * @file icm20602.h
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

#ifndef __ICM20602__
#define __ICM20602__

#include "config.h"
#include "sal.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "timers.h"
#include "queue.h"
#include "timer_hal.h"
#include "gpio_hal.h"
#include "print_util.h"
#include "sensor_dev.h"
#include "helper_func.h"
#include "errno.h"
#include <stdint.h>

#define XYG_LEN 6
#define XYG_TS_LEN  10
#define FIFO_HALF_LEN (1023)

 /*******************************************************************************
  *
  * Device registers.
  *
  ******************************************************************************/
//#define ICM20602_WHO_AM_I     0x75
#define ICM20602_WHO_AM_I_VAL 0x12

#define ICM20602_XG_OFFS_TC_H         0x04
#define ICM20602_XG_OFFS_TC_L         0x05
#define ICM20602_YG_OFFS_TC_H         0x07
#define ICM20602_YG_OFFS_TC_L         0x08
#define ICM20602_ZG_OFFS_TC_H         0x0A
#define ICM20602_ZG_OFFS_TC_L         0x0B
#define ICM20602_SELF_TEST_X_ACCEL    0x0D
#define ICM20602_SELF_TEST_Y_ACCEL    0x0E
#define ICM20602_SELF_TEST_Z_ACCEL    0x0F
#define ICM20602_XG_OFFS_USRH         0x13
#define ICM20602_XG_OFFS_USRL         0x14
#define ICM20602_YG_OFFS_USRH         0x15
#define ICM20602_YG_OFFS_USRL         0x16
#define ICM20602_ZG_OFFS_USRH         0x17
#define ICM20602_ZG_OFFS_USRL         0x18
#define ICM20602_SMPLRT_DIV           0x19
#define ICM20602_CONFIG               0x1A
#define ICM20602_GYRO_DLPF0_NBW307HZ  0x00
#define ICM20602_GYRO_DLPF1_NBW177HZ  0x01
#define ICM20602_GYRO_DLPF2_NBW109HZ  0x02
#define ICM20602_GYRO_DLPF3_NBW59HZ   0x03
#define ICM20602_GYRO_DLPF4_NBW31HZ   0x04
#define ICM20602_GYRO_DLPF5_NBW16HZ   0x05
#define ICM20602_GYRO_DLPF6_NBW8HZ    0x06
#define ICM20602_GYRO_DLPF7_NBW3451HZ 0x07
#define ICM20602_GYRO_CONFIG          0x1B
#define ICM20602_GYRO_250DPS          0x00
#define ICM20602_GYRO_500DPS          0x08
#define ICM20602_GYRO_1000DPS         0x10
#define ICM20602_GYRO_2000DPS         0x18
#define ICM20602_GYRO_DLPF_ENABLE     0x00
#define ICM20602_ACCEL_CONFIG         0x1C
#define ICM20602_ACCEL_2G             0x00
#define ICM20602_ACCEL_4G             0x08
#define ICM20602_ACCEL_8G             0x10
#define ICM20602_ACCEL_16G            0x18
#define ICM20602_ACCEL_CONFIG2        0x1D
#define ICM20602_ACCEL_DLPF_DISABLE   0x08
#define ICM20602_ACCEL_DLPF0_NBW235HZ 0x00
#define ICM20602_ACCEL_DLPF1_NBW235HZ 0x01
#define ICM20602_ACCEL_DLPF2_NBW121HZ 0x02
#define ICM20602_ACCEL_DLPF3_NBW62HZ  0x03
#define ICM20602_ACCEL_DLPF4_NBW31HZ  0x04
#define ICM20602_ACCEL_DLPF5_NBW16HZ  0x05
#define ICM20602_ACCEL_DLPF6_NBW8HZ   0x06
#define ICM20602_ACCEL_DLPF7_NBW442HZ 0x07
#define ICM20602_LP_MODE_CFG          0x1E
#define ICM20602_ACCEL_WOM_X_THR      0x20
#define ICM20602_ACCEL_WOM_Y_THR      0x21
#define ICM20602_ACCEL_WOM_Z_THR      0x22
#define ICM20602_FIFO_EN              0x23
#define ICM20602_FSYNC_INT            0x36
#define ICM20602_INT_PIN_CFG          0x37
#define ICM20602_INT_ENABLE           0x38
#define ICM20602_FIFO_WM_INT_STATUS   0x39
#define ICM20602_INT_STATUS           0x3A
#define ICM20602_ACCEL_XOUT_H         0x3B
#define ICM20602_ACCEL_XOUT_L         0x3C
#define ICM20602_ACCEL_YOUT_H         0x3D
#define ICM20602_ACCEL_YOUT_L         0x3E
#define ICM20602_ACCEL_ZOUT_H         0x3F
#define ICM20602_ACCEL_ZOUT_L         0x40
#define ICM20602_TEMP_OUT_H           0x41
#define ICM20602_TEMP_OUT_L           0x42
#define ICM20602_GYRO_XOUT_H          0x43
#define ICM20602_GYRO_XOUT_L          0x44
#define ICM20602_GYRO_YOUT_H          0x45
#define ICM20602_GYRO_YOUT_L          0x46
#define ICM20602_GYRO_ZOUT_H          0x47
#define ICM20602_GYRO_ZOUT_L          0x48
#define ICM20602_SELF_TEST_X_GYRO     0x50
#define ICM20602_SELF_TEST_Y_GYRO     0x51
#define ICM20602_SELF_TEST_Z_GYRO     0x52
#define ICM20602_FIFO_WM_TH1          0x60
#define ICM20602_FIFO_WM_TH2          0x61
#define ICM20602_SIGNAL_PATH_RESET    0x68
#define ICM20602_ACCEL_INTEL_CTRL     0x69
#define ICM20602_USER_CTRL            0x6A
#define ICM20602_PWR_MGMT_1           0x6B
#define ICM20602_PWR_MGMT_2           0x6C
#define ICM20602_PWR1_RUN_MODE        0x01
#define ICM20602_PWN2_DISABLE_ACCE    0x38
#define ICM20602_PWN2_DISABLE_GYRO    0x07
#define ICM20602_PWN2_ENABLE_ACCE     0x00
#define ICM20602_PWN2_ENABLE_GYRO     0x00
#define ICM20602_I2C_IF               0x70
#define ICM20602_FIFO_COUNTH          0x72
#define ICM20602_FIFO_COUNTL          0x73
#define ICM20602_FIFO_R_W             0x74
#define ICM20602_WHO_AM_I             0x75
#define ICM20602_XA_OFFSET_H          0x77
#define ICM20602_XA_OFFSET_L          0x78
#define ICM20602_YA_OFFSET_H          0x7A
#define ICM20602_YA_OFFSET_L          0x7B
#define ICM20602_ZA_OFFSET_H          0x7D
#define ICM20602_ZA_OFFSET_L          0x7E

//Typical register values
#define ICM20602_SMPLRT_DIV         0x19
#define ICM20602_SAMPLE_RATE_1000HZ 0x00
#define ICM20602_SAMPLE_RATE_500HZ  0x01
#define ICM20602_SAMPLE_RATE_200HZ  0x04
#define ICM20602_SAMPLE_RATE_125HZ  0x07
#define ICM20602_SAMPLE_RATE_100HZ  0x09
#define ICM20602_SAMPLE_RATE_50HZ   0x13
#define ICM20602_SAMPLE_RATE_20HZ   0x31

#define ICM20602_HIGH_SAMPLE_RATE_4000HZ   0x08    //Set ACCEL_FCHOICE_B = 1 rest all bit don't care.
#define BIT_ACCEL_FCHOICE_B 0x08

#define BIT_DATA_RDY_INT_EN             0x01 //enable interrupting source bit within INT_ENABLE register
#define BITS_DATA_RDY_INT_SEL           0x01
#define BIT_DATA_RDY_INT                0x01  //source of interrupt (within INT_STATUS register)

//bit 4 = 0; (interrutp is cleared on int status read.); 
//bit 5 = 1; level trig.
//bit 7 = 0; active high
#define INT_PIN_CFG_VAL                 0x20

//0100 0000 = 0x40 bit[6] = I2c interface disable
#define I2C_IF_DIS                      0x40

#define BIT_PWR_ACCEL_STBY              0x38
#define BIT_PWR_GYRO_STBY               0x07
#define BIT_PWR_ALL_OFF                 0x3f


#define BIT_POS_ACCEL_FS_SEL               3
#define BITS_ACCEL_FS_SEL               0x18  //FS_SEL MASK

#define BITS_GYRO_FS_SEL              0x18
#define BIT_POS_GYRO_FS_SEL           3


/*******************************************************************************/

typedef enum {
  ICM20602_2g       = 0,
  ICM20602_4g      = 1,
  ICM20602_8g       = 2,
  ICM20602_16g       = 3,
  ICM20602_XL_FS_ND = 4,  /* ERROR CODE */
} icm20602_fs_xl_t;

typedef enum {
  ICM20602_250dps     = 0,
  ICM20602_500dps     = 1,
  ICM20602_1000dps     = 2,
  ICM20602_2000dps    = 3,
  ICM20602_GY_FS_ND   = 4,    /* ERROR CODE */
} icm20602_fs_g_t;

#if (CONFIG_ICM20602_GYRO_ODR == 0)
#define ICM20602_GYRO_ODR_RUNTIME 1
#endif

#if 0
struct icm20602_config {
	char *comm_master_dev_name;
};
#endif

struct icm20602_data;


struct icm20602_transfer_function {
	int (*read_data)(struct icm20602_data *data, uint8_t reg_addr,
			 uint8_t *value, uint8_t len);
	int (*write_data)(struct icm20602_data *data, uint8_t reg_addr,
			  uint8_t value);
	int (*read_reg)(struct icm20602_data *data, uint8_t reg_addr,
			uint8_t *value);
	int (*update_reg)(struct icm20602_data *data, uint8_t reg_addr,
			  uint8_t mask, uint8_t value);
};

struct icm20602_data {
	const struct icm20602_transfer_function *hw_tf;
    uint8_t ui8OpenCnt;
    uint16_t fifo_sample_fact;
    tSensorBufInfo sXlBinfo;
    tSensorEventCb fXlCb;
    tSensorEvent XlEvMask;
    void *vXlCbptr;
	uint16_t ui16XlOdr;
    uint8_t XlCycleMask;

    tSensorBufInfo sGyBinfo;
    tSensorEventCb fGyCb;
    tSensorEvent GyEvMask;
    void *vGyCbptr;
	uint16_t ui16GyroOdr;
    uint8_t GyCycleMask;

    uint16_t FifoTh;

    uint16_t XlBatchSt;
    uint16_t XlBatchLen;
    uint16_t XlBatchCnt;
    uint16_t *XlBatchPtr;
    uint16_t *XlBatchStartPtr;
    uint16_t *XlBatchEndPtr;
    uint16_t *XlBatchCurrPtr;

    uint16_t GyBatchSt;
    uint16_t GyBatchLen;
    uint16_t GyBatchCnt;
    uint16_t *GyBatchPtr;
    uint16_t *GyBatchStartPtr;
    uint16_t *GyBatchEndPtr;
    uint16_t *GyBatchCurrPtr;

    uint16_t icm20602Odr;  //MAX odr settings between accel and gyro
    uint8_t icmXlLiveDataEnable;
    uint8_t icmGyroLiveDataEnable;
    SemaphoreHandle_t DevLock;
};

int icm20602_spi_init(struct icm20602_data *data);

void icm20602DrvInit(void);


#endif /* __ICM20602__ */
