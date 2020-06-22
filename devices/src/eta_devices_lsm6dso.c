/***************************************************************************//**
 *
 * @file eta_devices_lsm6sdo.c
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

#include "eta_devices_lsm6dso.h"

//
// Device configuration structure.
//
tLsm6dsoCfg g_sLsm6dsoCfg;

/***************************************************************************//**
 *
 * _Lsm6dsoRead - Read a register
 *
 * @param ui8Reg Register to read.
 *
 * @return Return the read value opf the requested register.
 *
 ******************************************************************************/
static uint8_t
_Lsm6dsoRead(uint8_t ui8Reg)
{
    uint8_t pui8TxData[3], pui8RxData[1];

    pui8TxData[0] = g_sLsm6dsoCfg.ui8I2cAddress; // write address
    pui8TxData[1] = ui8Reg; // 0x00;

    //
    // Setup the write.
    //
    EtaCspI2cTransferPoll(g_sLsm6dsoCfg.iI2cInstance, pui8TxData, 2, 0, 0,
                          false);

    //
    // Set the read bit.
    //
    pui8TxData[0] = g_sLsm6dsoCfg.ui8I2cAddress | 0x1;

    //
    // Setup the read.
    //
    EtaCspI2cTransferPoll(g_sLsm6dsoCfg.iI2cInstance, pui8TxData, 1,
                          pui8RxData, 1, false);

    //
    // Return the result.
    //
    return(*pui8RxData);
}

/***************************************************************************//**
 *
 * _Lsm6dsoWrite - Write a register.
 *
 * @param ui8Reg Register to write.
 * @param ui8Value TODO
 *
 ******************************************************************************/
static void
_Lsm6dsoWrite(uint8_t ui8Reg, uint8_t ui8Value)
{
    uint8_t pui8TxData[3];

    //
    // Write the register.
    //
    pui8TxData[0] = g_sLsm6dsoCfg.ui8I2cAddress;
    pui8TxData[1] = ui8Reg; // 0x00;
    pui8TxData[2] = ui8Value;
    EtaCspI2cTransferPoll(g_sLsm6dsoCfg.iI2cInstance, pui8TxData, 3, 0, 0,
                          false);
}

/***************************************************************************//**
 *
 * EtaDevicesLsm6dsoInit - Initialize the display.
 *
 * @param psCfg device configuration.
 *
 ******************************************************************************/
void
EtaDevicesLsm6dsoInit(tLsm6dsoCfg *psCfg)
{
    //
    // Save config.
    //
    g_sLsm6dsoCfg = *psCfg;

    //
    // initAccelerometer() to 104 Hz.
    //
    _Lsm6dsoWrite(LSM6DSL_CTRL1_XL, 0x40);

    //
    // initGyro(); to 104 Hz.
    //
    _Lsm6dsoWrite(LSM6DSL_CTRL2_G, 0x40);

    //
    // Set interrupt line to FIFO full
    //
    _Lsm6dsoWrite(LSM6DSL_INT1_CTRL, LSM6DSL_INT1_CTRL_FTH);

    //
    // Enable FIFO. Write bypass mode, sampling frequency.
    //
    _Lsm6dsoWrite(LSM6DSL_FIFO_CTRL5, LSM6DSL_FIFO_CTRL5_VAL);
}

void
EtaDevicesLsm6dsoSetAccSampleRate(uint32_t sample_rate)
{
    uint8_t odr;

    switch(sample_rate)
    {
        case 52:    odr = 0x30; break;
        case 208:   odr = 0x50; break;
        /* Default 104 Hz */
        default:    odr = 0x40; break;
    }

    _Lsm6dsoWrite(LSM6DSL_CTRL1_XL, odr);
}

/***************************************************************************//**
 *
 * EtaDevicesLsm6dsoIDGet - Get the device ID.
 *
 * @return Return the device ID.
 *
 ******************************************************************************/
uint8_t
EtaDevicesLsm6dsoIDGet(void)
{
    return(_Lsm6dsoRead(0x0F));
}

/***************************************************************************//**
 *
 * EtaDevicesLsm6dsoSampleGet - Read the accel and gyro sample values.
 *
 * @param psSample pointer to a sample structure to store the values.
 *
 ******************************************************************************/
void
EtaDevicesLsm6dsoSampleGet(tLsm6dsoSample *psSample)
{
    uint8_t tmpx, tmpl, tmph;

    do
    {
        //
        // Read status reg (1E) to see if new data is available.
        //
        tmpx = _Lsm6dsoRead(0x1E);
    }
    while((tmpx & 0x3) == 0x0);

    //
    // Read data.
    //
    for(int i = 0; i < 6; i += 2)
    {
        //
        // Read accel.
        //
        tmpl = _Lsm6dsoRead(0x28 + i);
        tmph = _Lsm6dsoRead(0x28 + i + 1);
        psSample->pi16Accel[i >> 1] = tmpl | (tmph << 8);

        //
        // Read gyro.
        //
        tmpl = _Lsm6dsoRead(0x22 + i);
        tmph = _Lsm6dsoRead(0x22 + i + 1);
        psSample->pi16Gyro[i >> 1] = tmpl | (tmph << 8);
    }
}

