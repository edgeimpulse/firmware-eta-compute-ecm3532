/***************************************************************************//**
 *
 * @file eta_status.h
 *
 * @brief This file contains eta_status module definitions.
 *
 * Copyright (C) 2018 Eta Compute, Inc
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
 * This is part of revision 1.0.0b1 of the Tensai Chip Support Package.
 *
 ******************************************************************************/

/***************************************************************************//**
 * @addtogroup ecm3532status-m3 Status
 * @ingroup ecm3532csp-m3
 * @{
 ******************************************************************************/

#ifndef __ETA_STATUS_H__
#define __ETA_STATUS_H__

#ifdef __cplusplus
extern "C" {
#endif

#define ETA_STATUS_COMMON_GROUP     (-99)
#define ETA_STATUS_BUCK_GROUP       (-199)
#define ETA_STATUS_UART_GROUP       (-299)
#define ETA_STATUS_ADC_GROUP        (-399)
#define ETA_STATUS_PWM_GROUP        (-499)
#define ETA_STATUS_FLASH_GROUP      (-549)
#define ETA_STATUS_FLASH_GROUP_ADDR (-599)
#define ETA_STATUS_I2C_GROUP        (-699)
#define ETA_STATUS_SPI_GROUP        (-799)

//
//! Status typedef used by the CSP.
//
typedef enum
{
    //
    //! Success/Failure.
    //
    eEtaSuccess = 0,
    eEtaFailure = -1,

    //
    //! General programatic errors.
    //
    eEtaNullPointer = ETA_STATUS_COMMON_GROUP,
    eEtaParameterOutOfRange,

    //
    //! Buck Errors and messages
    //
    eEtaBuckCallBackLater = ETA_STATUS_BUCK_GROUP,  // -199
    eEtaBuckInvalidAoVoltage,                       // -198
    eEtaBuckInvalidM3Voltage,                       // -197
    eEtaBuckInvalidM3Frequency,                     // -196
    eEtaBuckInvalidDspVoltage,                      // -195
    eEtaBuckInvalidDspFrequency,                    // -194
    eEtaBuckInvalidMemVoltage,                      // -193
    eEtaBuckDspPoweredOff,                          // -192
    eEtaBuckInvalidFreqOffsets,                     // -191

    //
    //! UART errors.
    //
    eEtaUartSendTimeout = ETA_STATUS_UART_GROUP,
    eEtaUartRecTimeout,
    eEtaUartInvalidBaud,
    eEtaUartInvalidPin,

    //
    //! ADC errors.
    //
    eEtaAdcSampleOverflow = ETA_STATUS_ADC_GROUP,
    eEtaAdcSampleInvalid,
    eEtaAdcFifoOverflow,

    //
    //! Flash errors.
    //
    eEtaFlashBranchTable = ETA_STATUS_FLASH_GROUP,
    eEtaFlashPageSizeError,
    eEtaFlashAddressError = ETA_STATUS_FLASH_GROUP_ADDR,

    //
    //! VHFO flash INFO trim valuer errors.
    //
    eEtaVhfoTrimKeyError,

    //
    //! I2C errors.
    //
    eEtaI2cInvalidConfig = ETA_STATUS_I2C_GROUP,
    eEtaI2cDoneTimeout,
    eEtaI2cRecTimeout,
    eEtaI2cSendTimeout,
    eEtaI2cUnexpectedNack,
    eEtaI2cCmdSequenceErr,

    //
    //! SPI errors.
    //
    eEtaSpiInvalidConfig = ETA_STATUS_SPI_GROUP,
    eEtaSpiDoneTimeout,
    eEtaSpiRecTimeout,
    eEtaSpiSendTimeout,
    eEtaSpiCmdSequenceErr,
    eEtaSpiTxFifoUflow,
    eEtaSpiRxFifoOflow,

} tEtaStatus;

#ifdef __cplusplus
}
#endif

#endif // __ETA_STATUS_H__

