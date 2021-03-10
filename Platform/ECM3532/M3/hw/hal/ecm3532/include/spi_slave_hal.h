/*******************************************************************************
*
* @file spi_slave_hal.h
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
******************************************************************************/
#ifndef H_SPI_SLAVE_HAL_
#define H_SPI_SLAVE_HAL_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/**
 *  Prototype for xfer complete callback for async mode API,
 *  callback will be called from interrupt context,
 *  it should do minimum and should not
 *  call any blocking API such as print.
 */
typedef void (*tHalSpiXferDoneCb) (void *);

/**
 * Initialize the SPI slave with default configuration
 *
 * @param ui8SpiNum SPI slave number to be initialize
 *
 * @return 0 on success, non-zero error code on failure.
 */
int32_t HalSpiSlaveInit(uint8_t ui8SpiNum);

/**
 * SPI Slave send and receive routine, works in half-duplex mode.
 *
 * SPI Slave Xfer, send and receive on SPI bus
 *
 * Slave will receive data when Master transmits
 *
 * For Slave to Send data, GPIO trigger need to be send
 * to master to start SPI clock for slave to transmit
 *
 * If called with valid callback function, it will be a non blocking
 * call and callback function will be called after completion from interrupt
 * context, callback function must only be used for signal of completion as it
 * is called from interrupt context.In case of non blockng mode, last xfer should
 * be completed before next call other wise BUSY error code will be returned.
 * If callback function is NULL, API will be blocking API and will retun after
 * completion of SPI transfer
 *
 * @param ui8SpiNum     SPI slave
 * @param ui8TxBuf      Pointer transmit buffer.
 * @param ui32TxCnt     Transmit byte count.
 * @param ui8RxBuf      Received buffer to store data from slave.
 * @param ui32RxCnt     Receive byte count.
 * @param fSpiCb        Callback function pointer, for async call.
 * @param vCbArg        Argument with which callback func will be called after
 *                      completion.
 *
 * @return int 0 on success, non-zero error code on failure.
 */
int32_t HalSpiSlaveXfer(uint8_t ui8SpiNum, uint8_t *ui8TxBuf,
                    uint32_t ui32TxCnt, uint8_t *ui8RxBuf, uint32_t ui32RxCnt,
                    tHalSpiXferDoneCb fSpiCb, void *vCbArg);

#ifdef __cplusplus
}
#endif

#endif /* H_SPI_SLAVE_HAL_ */
