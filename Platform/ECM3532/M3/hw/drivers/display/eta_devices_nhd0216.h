/***************************************************************************//**
 *
 * @file eta_devices_nhd0216.h
 *
 * @brief NHD_OLED (2 rows)
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
#ifndef __ETA_DEVICES_NHD0216__
#define __ETA_DEVICES_NHD0216__

#include "eta_csp_i2c.h"
#include "eta_csp_rtc.h"
#include "eta_csp_spi.h"

//
//! Nhd0216 communication enum.
//
typedef enum
{
    //
    //! SPI
    //
    eNhd0216ModeSpi = 1,

    //
    //! I2C
    //
    eNhd0216ModeI2c = 2
}
tNhd0216Mode;

//
//! Nhd0216 Config structure.
//
typedef struct
{
    //
    //! Communication mode.
    //
    tNhd0216Mode iMode;

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
tNhd0216Cfg;

/*******************************************************************************
 *
 * External function definitions.
 *
 ******************************************************************************/
extern void EtaDevicesNhd0216Init(tNhd0216Cfg *psCfg);
extern void EtaDevicesNhd0216CommandSend(char cCmd);
extern void EtaDevicesNhd0216DataSend(char cData);
extern uint8_t EtaDevicesNhd0216DataRead(uint8_t ui8Data);
extern void EtaDevicesNhd0216StringPrint(char *pcStr, uint8_t ui8X,
                                           uint8_t ui8Y);
extern void EtaDevicesNhd0216Clear(void);

#endif // __ETA_DEVICES_NHD0216__
