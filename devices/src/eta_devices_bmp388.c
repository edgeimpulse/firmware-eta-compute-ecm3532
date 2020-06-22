/***************************************************************************//**
 *
 * @file eta_devices_icm20602.c
 *
 * @brief Bosch BMP388 presure sensor driver.
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
 * @addtogroup etadevices-bmp388 Bosch BMP388 Presure Sensor
 * @ingroup etadevices
 * @{
 ******************************************************************************/

#include "eta_devices_bmp388.h"

//
// Device configuration structure.
//
tBmp388Cfg g_sBmp388Cfg;

//
// The Chip ID.
//
uint8_t g_ui8Bmp388Id;

/***************************************************************************//**
 *
 * _Bmp388I2cRead - Read a register
 *
 * @param ui8DevId Device ID
 * @param ui8Reg Register to write.
 * @param pui8Data Data to write
 * @param ui16Len Number of bytes to write.
 *
 * @return Return the read value opf the requested register.
 *
 ******************************************************************************/
static uint8_t
_Bmp388I2cRead(uint8_t ui8DevId, uint8_t ui8Reg, uint8_t *pui8Data,
               uint16_t ui16Len)
{
    uint8_t pui8TxData[3];

    pui8TxData[0] = g_sBmp388Cfg.ui8I2cAddress; // write address
    pui8TxData[1] = ui8Reg; // 0x00;

    //
    // Setup the write.
    //
    EtaCspI2cTransferPoll(g_sBmp388Cfg.iI2cInstance, pui8TxData, 2, 0, 0,
                          false);

    //
    // Set the read bit.
    //
    pui8TxData[0] = g_sBmp388Cfg.ui8I2cAddress | 0x1;

    //
    // Setup the read.
    //
    EtaCspI2cTransferPoll(g_sBmp388Cfg.iI2cInstance, pui8TxData, 1,
                          pui8Data, 1, false);

    //
    // Return the result.
    //
    return(0);
}

/***************************************************************************//**
 *
 * _Bmp388I2cWrite - TODO
 *
 * @param ui8DevId Device ID
 * @param ui8Reg Register to write.
 * @param pui8Data Data to write
 * @param ui16Len Number of bytes to write.
 *
 * @return TODO
 *
 ******************************************************************************/
static uint8_t
_Bmp388I2cWrite(uint8_t ui8DevId, uint8_t ui8Reg, uint8_t *pui8Data,
                uint16_t ui16Len)
{
    uint8_t pui8TxData[16];
    // fixme remove uint32_t ui32I;

    //
    // Write the register.
    //
    pui8TxData[0] = g_sBmp388Cfg.ui8I2cAddress;
    pui8TxData[1] = ui8Reg;
    memcpy(&pui8TxData[2], pui8Data, ui16Len);
    EtaCspI2cTransferPoll(g_sBmp388Cfg.iI2cInstance, pui8TxData, ui16Len, 0, 0,
                          false);

    //
    // Return the result.
    //
    return(0);
}

/***************************************************************************//**
 *
 * EtaDevicesBmp388Init - Initialize the sensor.
 *
 * @param psCfg device configuration.
 *
 * @return zero -> Success / +ve value -> Warning / -ve value -> Error
 *
 ******************************************************************************/
int8_t
EtaDevicesBmp388Init(tBmp388Cfg *psCfg)
{
    struct bmp3_dev dev;
    int8_t i8Result = BMP3_OK;

    //
    // Save config.
    //
    g_sBmp388Cfg = *psCfg;

    //
    // Fill in the sensor structure.
    //
    dev.dev_id = 0;
    dev.intf = g_sBmp388Cfg.iMode;
    if(g_sBmp388Cfg.iMode == eBmp388ModeSpi)
    {
        dev.read =  (bmp3_com_fptr_t)0;
        dev.write =  (bmp3_com_fptr_t)0;
    }
    else
    {
        dev.read = (bmp3_com_fptr_t)_Bmp388I2cRead;
        dev.write = (bmp3_com_fptr_t)_Bmp388I2cWrite;
    }

    dev.delay_ms = EtaCspRtcTmrDelayMs;

    //
    // Initialize the sensor.
    //
    i8Result = bmp3_init(&dev);

    //
    // Was the operation successful?
    //
    if(i8Result == BMP3_OK)
    {
        g_ui8Bmp388Id = dev.chip_id;
    }
    else
    {
        g_ui8Bmp388Id = 0;
    }

    //
    // Return the result.
    //
    return(i8Result);
}

/***************************************************************************//**
 *
 * EtaDevicesBmp388IdGet - Get the saved after calling EtaDevicesBmp388Init.
 *
 * @return the saved ID.
 *
 ******************************************************************************/
uint8_t
EtaDevicesBmp388IdGet(void)
{
    //
    // Return the saved ID from the init.
    //
    return(g_ui8Bmp388Id);
}

