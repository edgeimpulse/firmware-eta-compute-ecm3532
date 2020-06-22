/***************************************************************************//**
 *
 * @file eta_devices_icm20602.c
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
 * This is part of revision 1.0.0a6 of the Tensai Software Development Kit.
 *
 ******************************************************************************/

/***************************************************************************//**
 * @addtogroup etadevices-icm20602 InvenSense Accel + Gyro Sensor
 * @ingroup etadevices
 * @{
 ******************************************************************************/

#include "eta_devices_icm20602.h"

//
// Device configuration structure.
//
tIcm20602Cfg g_sIcm20602Cfg;

/***************************************************************************//**
 *
 * _Icm20602Read - Read from the Icm20602
 *
 * @param ui8Reg Register to read.
 * @param ui32NumBytes Number of bytes to read.
 *
 * @return Return the read value opf the requested register.
 *
 ******************************************************************************/
static void
_Icm20602Read(uint8_t ui8Reg, uint8_t *pui8RxData, uint32_t ui32NumBytes)
{
    uint8_t pui8TxData[1];

    //
    // Setup the read.
    //
    pui8TxData[0] = ui8Reg | 0x80;

    EtaCspSpiTransferPoll(g_sIcm20602Cfg.iSpiInstance, pui8TxData, 1,
                          pui8RxData, ui32NumBytes, g_sIcm20602Cfg.iSpiChipSel,
                          eSpiSequenceFirstLast);
}

/***************************************************************************//**
 *
 * _Icm20602Write - Write to the Icm20602
 *
 * @param ui8Reg Register to write.
 * @param ui8Value TODO
 *
 ******************************************************************************/
static void
_Icm20602Write(uint8_t ui8Reg, uint8_t ui8Value)
{
    uint8_t pui8TxData[2];

    //
    // Write the register.
    //
    pui8TxData[0] = ui8Reg & 0x7F;
    pui8TxData[1] = ui8Value;
    EtaCspSpiTransferPoll(g_sIcm20602Cfg.iSpiInstance, pui8TxData, 2,
                          0, 0, g_sIcm20602Cfg.iSpiChipSel,
                          eSpiSequenceFirstLast);
    EtaCspRtcTmrDelayMs(10);
}

/***************************************************************************//**
 *
 * EtaDevicesIcm20602Init - Initialize the display.
 *
 * @param psCfg device configuration.
 *
 ******************************************************************************/
void
EtaDevicesIcm20602Init(tIcm20602Cfg *psCfg)
{
    // fixme remove uint8_t pui8Rx[1];

    //
    // Save config.
    //
    g_sIcm20602Cfg = *psCfg;

    //
    // Full reset.
    //
    _Icm20602Write(ICM20602_PWR_MGMT_1, 0x80);
    EtaCspRtcTmrDelayMs(1000);

    //
    // Check the ID.
    //
    if(EtaDevicesIcm20602IDGet() == ICM20602_WHO_AM_I_VAL)
    {
        //
        // Set to PLL.
        //
        _Icm20602Write(ICM20602_PWR_MGMT_1, 0x01);

        //
        // Put accel and gyro in standby.
        //
        _Icm20602Write(ICM20602_PWR_MGMT_2, 0x3f);

        //
        // Disable FIFO
        //
        _Icm20602Write(ICM20602_USER_CTRL, 0x00);

        //
        // Gyro config.
        //
        _Icm20602Write(ICM20602_GYRO_CONFIG,
                       (ICM20602_GYRO_250DPS | ICM20602_GYRO_DLPF_ENABLE));
        _Icm20602Write(ICM20602_CONFIG, ICM20602_GYRO_DLPF6_NBW8HZ);

        //
        // Accel config.
        //
        _Icm20602Write(ICM20602_ACCEL_CONFIG, ICM20602_ACCEL_2G);
        _Icm20602Write(ICM20602_ACCEL_CONFIG2, ICM20602_ACCEL_DLPF5_NBW16HZ);

        //
        // Set sample rate 1000Hz
        //
        _Icm20602Write(ICM20602_SMPLRT_DIV, ICM20602_SAMPLE_RATE_100HZ);

        //
        // Enable accel and gyro.
        //
        _Icm20602Write(ICM20602_PWR_MGMT_2,
                       ICM20602_PWN2_ENABLE_ACCE | ICM20602_PWN2_ENABLE_GYRO);
    }
}

/***************************************************************************//**
 *
 * EtaDevicesIcm20602IDGet - Get the device ID.
 *
 * @return Return the device ID.
 *
 ******************************************************************************/
uint8_t
EtaDevicesIcm20602IDGet(void)
{
    uint8_t pui8ID[1];

    _Icm20602Read(ICM20602_WHO_AM_I, pui8ID, 1);
    EtaCspRtcTmrDelayMs(10);
    return(*pui8ID);
}

/***************************************************************************//**
 *
 * EtaDevicesIcm20602SampleGet - Read the accel and gyro sample values.
 *
 * @param psSample pointer to a sample structure to store the values.
 *
 ******************************************************************************/
void
EtaDevicesIcm20602SampleGet(tIcm20602Sample *psSample)
{
    uint8_t pui8Bytes[14];

    do
    {
        //
        // Read int status reg (3A) to see if new data is available.
        //
        _Icm20602Read(ICM20602_INT_STATUS, pui8Bytes, 1);
    }
    while((pui8Bytes[0] & 0x1) == 0x1);
    

    //
    // Read a sample.
    //
    _Icm20602Read(ICM20602_ACCEL_XOUT_H, pui8Bytes, 14);

    psSample->pi16Accel[0] = (pui8Bytes[0] << 8) | pui8Bytes[1];
    psSample->pi16Accel[1] = (pui8Bytes[2] << 8) | pui8Bytes[3];
    psSample->pi16Accel[2] = (pui8Bytes[4] << 8) | pui8Bytes[5];
    psSample->pi16Gyro[0] = (pui8Bytes[8] << 8) | pui8Bytes[9];
    psSample->pi16Gyro[1] = (pui8Bytes[10] << 8) | pui8Bytes[11];
    psSample->pi16Gyro[2] = (pui8Bytes[12] << 8) | pui8Bytes[13];
}

int EtaDevicesIcm20602Status(void)
{
    uint8_t stat8[1];

    _Icm20602Read(ICM20602_INT_STATUS, stat8, 1);

    return stat8[0];
}

