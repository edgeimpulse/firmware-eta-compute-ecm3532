/***************************************************************************//**
 *
 * @file eta_csp.h
 *
 * @brief CSP for DSP.
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
 * @addtogroup ecm3532
 * @addtogroup ecm3532csp-dsp
 *
 * @defgroup ecm3532 ECM3532 Documentation
 * @defgroup ecm3532csp-dsp DSP CSP (Chip Support Package)
 *
 * @ingroup ecm3532
 * @ingroup ecm3532csp-dsp
 * @{
 ******************************************************************************/

#ifndef ETA_CSP_H
#define ETA_CSP_H

//
//! Clock control typedef.
//
typedef enum
{
    //
    //! Clock is always on.
    //
    eClkCtrlAlwaysOn = 1,

    //
    //! Clock gating enabled.
    //
    eClkCtrlAuto = 0,
}
tClkCtrl;

/*******************************************************************************
 *
 * External function definitions.
 *
 ******************************************************************************/

//
// EtaCspFinish - Notify M3 that we are finished for either DSP core or DMA.
//
extern void EtaCspFinish(int);

//
// EtaCspAhbWindowDontWaitGet - Get DONTWAIT bit value
//
extern uint_fast8_t EtaCspAhbWindowDontWaitGet();

//
// EtaCspAhbWindowDontWaitSet - Set DONTWAIT bit
//
extern void EtaCspAhbWindowDontWaitSet(uint_fast8_t ui8Enable);

//
// EtaCspAhbWindowCacheLineDisable - Disable 32-bit cache line in AHB Window.
//
extern void EtaCspAhbWindowCacheLineDisable();

//
// EtaCspAhbWindowCacheLineEnable - Enable 32-bit cache line in AHB Window (Default).
//
extern void EtaCspAhbWindowCacheLineEnable();

//
// EtaCspCtrlSpiClkSet - Set clock gating mode for SPI master
//
extern void EtaCspCtrlSpiClkSet(tClkCtrl clkCtrl);

//
// EtaCspCtrlI2cClkSet  - Set clock gating mode for I2C master
//
extern void EtaCspCtrlI2cClkSet(tClkCtrl clkCtrl);

#endif // ETA_CSP_H

