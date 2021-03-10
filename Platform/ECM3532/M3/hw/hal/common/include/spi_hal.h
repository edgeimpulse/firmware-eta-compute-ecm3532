/*******************************************************************************
*
* @file spi_hal.h
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
#ifndef H_SPI_HAL_
#define H_SPI_HAL_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "gpio_hal.h"

/**
 *  SPI CS enum
 */
typedef enum
{
  /** Active Low Chip Select */
  CS_ACTIVE_LOW = 0,
  /** Active High Chip Select */
  CS_ACTVIE_HIGH = 1,
}tSpiCSActiveLevel;

/**
 *  Prototype for xfer complete callback,
 *  callback will be called from interrupt context,
 *  it should do minimum and should not
 *  call any blocking API such as print.
 */
typedef void (*tHalSpiXferDoneCb) (void *);

/**
 * SPI Bus config structure
 * required to get and set SPI bus properties
 */
typedef struct {
    /** Baudrate in KHz */
    uint16_t    ui16BRateKHz;
    /** SPI Clock Polarity */
    uint8_t     ui8Cpol:1;
    /** SPI Clock Phase */
    uint8_t     ui8Cpha:1;
    /**
     * Meta Flops Dis, if 1 meta flops
     * are disabled else meta flops will be enabled
     */
    uint8_t     ui8MetaFlopsDis:1;
    uint8_t     reserve;
}tHalSpiCfg;


/** Assert CS Gpio pin of SPI Slave,
 * GPIO pin should have configured to OUTPUT pin
 * This need to be called before HalSpiXfer, this will also ensure exclusive
 * access for SPI slave
 *
 * @param ui8Spi SPI bus number to be initialize
 * @param ui32CSGpio Gpio pin connected to slave CS pin
 * @param CSLevel CS active level
 *
 */
void HalSpiAssertCS(uint8_t ui8Spi, uint32_t ui32CSGpio, tSpiCSActiveLevel CSLevel);

/** De-Assert CS Gpio pin of SPI Slave,
 * GPIO pin should have configured to OUTPUT pin
 * This need to be called after HalSpiXfer, this will release lock
 *
 * @param ui8Spi SPI bus number to be initialize
 * @param ui32CSGpio Gpio pin connected to slave CS pin
 *
 */
void HalSpiDeAssertCS(uint8_t ui8Spi, uint32_t ui32CSGpio);
/**
 * Initialize the SPI bus with default configuration
 *
 * @param ui8SpiNum SPI bus number to be initialize
 *
 * @return 0 on success, non-zero error code on failure.
 */
int32_t HalSpiInit(uint8_t ui8SpiNum);

/**
 * Set SPI configuration,
 * to change SPI default configuration set in SpiInit
 *
 * @param ui8SpiNum SPI bus number to be configure
 * @param sCfgSpi   configuration to be set
 *
 * @return 0 on success, non-zero error code on failure.
 */
int32_t HalSpiSetCfg(uint8_t ui8SpiNum, tHalSpiCfg *sCfgSpi);

/**
 * Get SPI Configuration
 *
 * @param ui8SpiNum SPI bus number
 * @param sCfgSpi   pointer where current settings will be filled
 *
 * @return 0 on success, non-zero error code on failure.
 */
int32_t HalSpiGetCfg(uint8_t ui8SpiNum, tHalSpiCfg *sCfgSpi);

/**
 * SPI Xfer, send and receive on SPI bus
 *
 * Slave Chip select and de-select must be handle out side of this function
 *
 * If called with valid callback function, it will be a non blocking
 * call and callback function will be called after completion from interrupt
 * context, callback function must only be used for signal of completion as it
 * is called from interrupt context.In case of non blockng mode, last xfer should
 * be completed before next call other wise BUSY error code will be returned.
 * If callback function is NULL, API will be blocking API and will retun after
 * completion of SPI transfer
 * CS should be assert before calling this API by using HalSpiAssertCS
 * CS should be de-asserted after calling this API by using HalSpiDeAssertCS
 *
 * @param ui8SpiNum     SPI bus number
 * @param ui8TxBuf      Pointer transmit buffer.
 * @param ui16TxCnt     Transmit byte count.
 * @param ui8RxBuf      Received buffer to store data from slave.
 * @param ui16RxCnt     Receive byte count.
 * @param fSpiCb        Callback function pointer, for async call.
 * @param vCbArg        Argument with which callback func will be called after
 *                      completion.
 *
 * @return int 0 on success, non-zero error code on failure.
 */
int32_t HalSpiXfer(uint8_t ui8SpiNum, void *ui8TxBuf,
                    uint16_t ui16TxCnt, void *ui8RxBuf, uint16_t ui16RxCnt,
                    tHalSpiXferDoneCb fSpiCb, void *vCbArg);

/**
 * SPI Xfer, send and receive on SPI bus in Full Duplex mode i.e. both TX and RX
 * simultaneously
 *
 * Slave Chip select and de-select must be handle out side of this function
 *
 * If called with valid callback function, it will be a non blocking
 * call and callback function will be called after completion from interrupt
 * context, callback function must only be used for signal of completion as it
 * is called from interrupt context.In case of non blockng mode, last xfer should
 * be completed before next call other wise BUSY error code will be returned.
 * If callback function is NULL, API will be blocking API and will retun after
 * completion of SPI transfer
 *
 * CS should be assert before calling this API by using HalSpiAssertCS
 * CS should be de-asserted after calling this API by using HalSpiDeAssertCS
 *
 * @param ui8SpiNum     SPI bus number
 * @param ui8TxBuf      Pointer transmit buffer.
 * @param ui16TxCnt     Transmit byte count.
 * @param ui8RxBuf      Received buffer to store data from slave.
 * @param ui16RxCnt     Receive byte count.
 * @param fSpiCb        Callback function pointer, for async call.
 * @param vCbArg        Argument with which callback func will be called after
 * completion.
 *
 * @return int 0 on success, non-zero error code on failure.
 */

int32_t HalSpiXferFD(uint8_t ui8SpiNum, void *ui8TxBuf,
                    uint16_t ui16TxCnt, void *ui8RxBuf, uint16_t ui16RxCnt,
                    tHalSpiXferDoneCb fSpiCb, void *vCbArg);


#ifdef __cplusplus
}
#endif

#endif /* H_SPI_HAL_ */
