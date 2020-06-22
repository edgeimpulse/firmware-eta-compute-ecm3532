/***************************************************************************//**
 *
 * @file eta_devices_icm20602.h
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

#ifndef __ETA_DEVICES_BMP388__
#define __ETA_DEVICES_BMP388__

#include <string.h>
#include "eta_csp_i2c.h"
#include "eta_csp_rtc.h"
#include "eta_csp_spi.h"

#include "bmp3.h"

#ifdef __cplusplus
extern "C" {
#endif

//
//! Bmp388 communication enum.
//
typedef enum
{
    //
    //! SPI
    //
    eBmp388ModeSpi = BMP3_SPI_INTF,

    //
    //! I2C
    //
    eBmp388ModeI2c = BMP3_I2C_INTF
}
tBmp388Mode;

//
//! Bmp388 Config structure.
//
typedef struct
{
    //
    //! Communication mode.
    //
    tBmp388Mode iMode;

    //
    //! I2c or SPI instance
    //
    union
    {
        tI2cNum iI2cInstance;
        tSpiNum iSpiInstance;
    };

    //
    //! Address or chip select.
    //
    union
    {
        tSpiChipSel iChipSel;
        uint8_t ui8I2cAddress;
    };
}
tBmp388Cfg;

//
//! Bmp388 sample.
//
typedef struct
{
    //
    // Un-compensated pressure.
    //
    uint32_t ui32Pressure;

    //
    // Un-compensated temperature.
    //
    uint32_t ui32Temperature;
}
tBmp388Sample;

/*******************************************************************************
 *
 * External function definitions.
 *
 ******************************************************************************/
extern int8_t EtaDevicesBmp388Init(tBmp388Cfg *psCfg);
extern uint8_t EtaDevicesBmp388IdGet(void);

#ifdef __cplusplus
}
#endif

#endif // __ETA_DEVICES_BMP388__

