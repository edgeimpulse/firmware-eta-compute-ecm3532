/***************************************************************************//**
 *
 * @file eta_devices_opt3001.c
 *
 * @brief Driver for OPT3001 Ambient light sensor
 *
 * Copyright (C) 2019 Eta Compute, Inc
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
#include "config.h"
#include <stdint.h>
#include <stdlib.h>
#include "i2c_hal.h"
#include "eta_devices_opt3001.h"

#ifdef CONFIG_ALS_OPT3001
void Opt3001RegWrite(uint8_t reg, uint16_t val)
{
    volatile uint8_t txData[2];

    txData[0] = (val >> 8) & 0xFF;
    txData[1] = (val) && 0xFF;

    HalI2cWrite(CONFIG_OPT3001_I2C_INSTANCE, CONFIG_OPT3001_I2C_ADDRESS,
            reg, 1, (uint8_t*)&txData, 2, NULL, NULL);
}

uint16_t Opt3001RegRead(uint8_t reg)
{
    uint8_t data[2];
    uint16_t val = 0;

    HalI2cRead(CONFIG_OPT3001_I2C_INSTANCE, CONFIG_OPT3001_I2C_ADDRESS, reg, 1, (uint8_t*)&data, 2, NULL, NULL);

    val = data[0] << 8;
    val |= data[1];

    return val;
}

volatile double val = 0;

double Opt3001RegtoLux(uint16_t data)
{
    
    OPT3001_RESULT_t* als_val = (OPT3001_RESULT_t*)&data;

    val = 0.01 * (double)((1 << als_val->E) * (als_val->R));
    return val;
}
#endif