/*******************************************************************************
*
* @file i2c_hal.c
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
******************************************************************************/
#ifndef H_I2C_HAL_
#define H_I2C_HAL_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Supported I2C HAL clock
 */
typedef enum
{
    /** I2C Clk 100KHz */
    HalI2cClk100KHz = 100,
    /** I2C Clk 400KHz */
    HalI2cClk400KHz = 400,
    /** I2C Clk 1000KHZ (1MHz) */
    HalI2cClk1MHz = 1000,
}tHalI2cClk;

/**
 * Function prototype for transmission complete callback.
 * This will called when transmission is complete,
 * will be called from Interrupt context, it should do
 * do minimum things required and no blocking routinue
 * e.g. print should be called from it, if called
 * result in call back blocked for always.
 */
 typedef void (*tHalI2cCb)(void *);

/**
 * Initialize i2c bus with default frequency
 *
 * @param ui8I2cNum I2c bus number to be initialized
 *
 * @return 0 on success, and non-zero error code on failure
 */
int32_t HalI2cInit(uint8_t ui8I2cNum);

/**
 *
 * Change i2c bus frequency
 *
 * @param ui8I2cNum I2C bus number
 * @param iFreq I2C frequncy to be set
 *
 * @return 0 on success, and non-zero error code on failure
 */
int32_t HalI2cSetBusFreq(uint8_t ui8I2cNum, tHalI2cClk iFreq);

/**
 * Probes the i2c bus for a device with address passed as argument.
 * API can be used to detect slave device on bus. API will
 * issue issue address of slave on bus, if slave ACK the address
 * cycle API return 0.
 *
 * @param ui8I2cNum I2C bus number
 * @param ui8SlaveAddr slave address to be probe
 *
 * @return 0 on success, non-zero error code on failure
 */
int32_t HalI2cProbe(uint8_t ui8I2cNum, uint8_t ui8SlaveAddr);

/**
 * Read from slave device on i2c bus.
 *
 * @param ui8I2cNum I2C bus number
 * @param ui8SlaveAddr 7 bit slave address
 * @param ui16Offset Read offset in slave
 * @param ui8OffsetLen Length of offset, 1 byte or 2 byte
 * @param ui8RxBuf Buffer to store receive data from slave
 * @param ui32RxLen Receive data bytes length
 * @param fI2cCb read complete callback for non blocking mode,
 *  if NULL it will be blocking call
 * @param vPtr argument with which callback function will be called

 * @return 0 on success, non-zero error code on failure
 */
int32_t HalI2cRead(uint8_t ui8I2cNum, uint8_t ui8SlaveAddr, uint16_t ui16Offset,
                uint8_t ui8OffsetLen, uint8_t *ui8RxBuf, uint32_t ui32RxLen,
                tHalI2cCb fI2cCb, void *vPtr);

/**
 * Write i2c bus from salve device
 * API  to read to a slave device on bus
 *
 * @param ui8I2cNum I2C bus number
 * @param ui8SlaveAddr 7 bit slave address
 * @param ui16Offset Write offset in slave
 * @param ui8OffsetLen Length of offset, 1 byte or 2 byte
 * @param ui8TxBuf Write Buffer
 * @param ui32TxLen data bytes length
 * @param fI2cCb write complete callback for non blocking mode,
 * if NULL it will be blocking call. In case of non blocking mode,
 * last operation should be completed before next call other wise
 * BUSY error code will be returned.
 * @param vPtr argument with which callback function will be called

 * @return 0 on success, non-zero error code on failure
 */
int32_t HalI2cWrite(uint8_t ui8I2cNum, uint8_t ui8SlaveAddr, uint16_t ui16Offset,
                uint8_t ui8OffsetLen, uint8_t *ui8TxBuf, uint32_t ui32TxLen,
                tHalI2cCb fI2cCb, void *vPtr);

#ifdef __cplusplus
}
#endif

#endif /* H_I2C_HAL_ */
