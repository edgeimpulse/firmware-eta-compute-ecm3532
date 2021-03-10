/***************************************************************************//**
 *
 * @file eta_csp_status.h
 *
 * @brief This file contains eta_csp_status module definitions.
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
 * This is part of revision 1.0.0b1 of the Tensai Chip Support Package.
 *
 ******************************************************************************/

/***************************************************************************//**
 * @addtogroup ecm3532status-dsp Status
 * @ingroup ecm3532csp-dsp
 * @{
 ******************************************************************************/

#ifndef __ETA_STATUS_H__
#define __ETA_STATUS_H__

#ifdef __cplusplus
extern "C" {
#endif

#define ETA_STATUS_COMMON_GROUP (-99)
#define ETA_STATUS_I2C_GROUP    (-599)
#define ETA_STATUS_SPI_GROUP    (-699)

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
    eEtaBusy = -2,

    //
    //! General programatic errors.
    //
    eEtaNullPointer = ETA_STATUS_COMMON_GROUP,
    eEtaParameterOutOfRange,

    //
    //! I2C errors.
    //
    eEtaI2cInvalidConfig = ETA_STATUS_I2C_GROUP,
    eEtaI2cDoneTimeout,
    eEtaI2cRecTimeout,
    eEtaI2cSendTimeout,
    eEtaI2cUnexpectedNack,
    eEtaI2cCmdSequenceErr,
    eEtaI2cBusy,

    //
    //! SPI errors.
    //
    eEtaSpiInvalidConfig = ETA_STATUS_SPI_GROUP,
    eEtaSpiDoneTimeout,
    eEtaSpiRecTimeout,
    eEtaSpiSendTimeout,
    eEtaSpiCmdSequenceErr,
    eEtaSpiBusy
} tEtaStatus;

#ifdef __cplusplus
}
#endif

#endif // __ETA_STATUS_H__

