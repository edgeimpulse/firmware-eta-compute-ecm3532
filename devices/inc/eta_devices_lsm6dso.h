/***************************************************************************//**
 *
 * @file eta_devices_lsm6dso.h
 *
 * @brief ST Accel + Gyro sensor
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
 * This is part of revision 1.0.0a6 of the Tensai Software Development Kit.
 *
 ******************************************************************************/
#ifndef __ETA_DEVICES_LSM6DSO__
#define __ETA_DEVICES_LSM6DSO__

#include "eta_csp_i2c.h"
#include "eta_csp_rtc.h"

#ifdef __cplusplus
extern "C" {
#endif

//
//! Lsm6dso Config structure.
//
typedef struct
{
    //
    //! I2C instance
    //
    tI2cNum iI2cInstance;

    //
    //! Address.
    //
    uint8_t ui8I2cAddress;
}
tLsm6dsoCfg;

//
//! Axes struct
//
typedef struct
{
    //
    // X axis.
    //
    int16_t i16X;

    //
    // Y axis.
    //
    int16_t i16Y;

    //
    // Z axis.
    //
    int16_t i16Z;
}
tLsm6dsoAxes;

//
//! Sample struct.
//
typedef struct
{
    //
    // Accel.
    //
    union
    {
        tLsm6dsoAxes sAccel;
        int16_t pi16Accel[3];
    };

    //
    // Gyro.
    //
    union
    {
        tLsm6dsoAxes sGyro;
        int16_t pi16Gyro[3];
    };
}
tLsm6dsoSample;

/*******************************************************************************
 *
 * Device registers.
 *
 ******************************************************************************/
#define LSM6DSL_FIFO_CTRL1 0x06
#define LSM6DSL_FIFO_CTRL2 0x07
#define LSM6DSL_FIFO_CTRL3 0x08
#define LSM6DSL_FIFO_CTRL4 0x09
#define LSM6DSL_FIFO_CTRL5 0x0A
#define FIFO_CTRL5         0x0A
#define LSM6DSL_CTRL1_XL   0x10
#define LSM6DSL_CTRL3_C    0x12
#define LSM6DSL_CTRL5_C    0x14
#define LSM6DSL_CTRL2_G    0x11
#define LSM6DSL_INT1_CTRL  0x0D
#define LSM6DSL_INT2_CTRL  0x0E
#define LSM6DSL_STATUS_REG 0x1E

#define LSM6DSL_FIFO_STATUS2 0x3B

#define LSM6DSL_FIFO_DATA_OUT_L 0x3E
#define LSM6DSL_FIFO_DATA_OUT_H 0x3F

#define LSM6DSL_INT1_CTRL_FTH 0x08

// accelerometer data registers
#define LSM6DSL_OUTX_L_XL 0x28
#define LSM6DSL_OUTX_H_XL 0x29
#define LSM6DSL_OUTY_L_XL 0x2A
#define LSM6DSL_OUTY_H_XL 0x2B
#define LSM6DSL_OUTZ_L_XL 0x2C
#define LSM6DSL_OUTZ_H_XL 0x2D

// gyro data registers
#define LSM6DSL_OUTX_L_G 0x22
#define LSM6DSL_OUTX_H_G 0x23
#define LSM6DSL_OUTY_L_G 0x24
#define LSM6DSL_OUTY_H_G 0x25
#define LSM6DSL_OUTZ_L_G 0x26
#define LSM6DSL_OUTZ_H_G 0x27

// make 104 HZ the default sample rate
#define LSM6DSL_CTRL1_XL_VAL   0x40 // accel
#define LSM6DSL_CTRL2_G_VAL    0x40 // gyro
#define LSM6DSL_FIFO_CTRL5_VAL 0x20 // fifo

#ifdef SAMPLE208HZ

// make 208 HZ the default sample rate
#define LSM6DSL_CTRL1_XL_VAL   0x50 // accel
#define LSM6DSL_CTRL2_G_VAL    0x50 // gyro
#define LSM6DSL_FIFO_CTRL5_VAL 0x28 // fifo
#endif

#ifdef SAMPLE416HZ

#define LSM6DSL_CTRL1_XL_VAL   0x60
#define LSM6DSL_CTRL2_G_VAL    0x60
#define LSM6DSL_FIFO_CTRL5_VAL 0x30

#endif

#ifdef SAMPLE833HZ

#define LSM6DSL_CTRL1_XL_VAL   0x70
#define LSM6DSL_CTRL2_G_VAL    0x70
#define LSM6DSL_FIFO_CTRL5_VAL 0x38

#endif

#ifdef SAMPLE1660HZ

#define LSM6DSL_CTRL1_XL_VAL   0x80
#define LSM6DSL_CTRL2_G_VAL    0x80
#define LSM6DSL_FIFO_CTRL5_VAL 0x40

#endif

/*******************************************************************************
 *
 * External function definitions.
 *
 ******************************************************************************/
extern void EtaDevicesLsm6dsoInit(tLsm6dsoCfg *psCfg);
extern void EtaDevicesLsm6dsoSetAccSampleRate(uint32_t sample_rate);
extern uint8_t EtaDevicesLsm6dsoIDGet(void);
extern void EtaDevicesLsm6dsoSampleGet(tLsm6dsoSample *psSample);

#ifdef __cplusplus
}
#endif

#endif // __ETA_DEVICES_LSM6DSO__

