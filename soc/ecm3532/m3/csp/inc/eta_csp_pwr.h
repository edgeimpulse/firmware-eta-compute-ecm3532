/***************************************************************************//**
 *
 * @file eta_csp_pwr.h
 *
 * @brief This file contains eta_csp_pwr module implementations.
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
 * This is part of revision 1.0.0a6 of the Tensai Software Development Kit.
 *
 ******************************************************************************/

/***************************************************************************//**
 * @addtogroup ecm3532pwr-m3 Power
 * @ingroup ecm3532csp-m3
 * @{
 ******************************************************************************/

#ifndef __ETA_CSP_PWR_H__
#define __ETA_CSP_PWR_H__

#ifdef __cplusplus
extern "C" {
#endif

//
//! The power domain typedef.
//
typedef enum
{
    //
    //! Peripheral power domain.
    //
    ePwrDomainPeriph = 1,

    //
    //! SRAM bank 0 power domain.
    //
    ePwrDomainSram0 = 2,

    //
    //! SRAM bank 1
    //
    ePwrDomainSram1 = 4,

    //
    //! SRAM banks 2, and 3 power domain.
    //
    ePwrDomainSram23 = 8,

    //
    //! SRAM banks 4, 5,  6, and 7 power domain.
    //
    ePwrDomainSram4567 = 16,

    //
    //! Flash power domain.
    //
    ePwrDomainFlash = 32,

    //
    //! Memory buck power domain.
    //
    ePwrDomainBuckMem = 64,

    //
    //! M3 buck power domain.
    //
    ePwrDomainBuckM3 = 128,

    //
    //! DSP buck power domain.
    //
    ePwrDomainBuckDsp = 256,

    //
    //! DSP power domain.
    //
    ePwrDomainDsp = 512,

    //
    //! ADC power domain.
    //
    ePwrDomainAdc = 1024,
}
tPwrDomain;

//
// Force the power to keep PMICS, SRAMs, etc on during power down.
// NOTE more than one domain can be ORed in.
//
extern void EtaCspPwrForce(tPwrDomain iDomain);

//
// Clear force bit allowing PMICS, SRAMs, etc to power down.
// NOTE more than one domain can be ORed in.
//
extern void EtaCspPwrRelease(tPwrDomain iDomain);

//
// Turn power on to a domain.
//
extern void EtaCspPwrOn(tPwrDomain iDomain);

//
// Turn power off to a domain.
//
extern void EtaCspPwrOff(tPwrDomain iDomain);

//
// Restore the default power configuration.
//
extern void EtaCspPwrDefaultsRestore(void);

//
// Shutdown power to the SoC.
//
extern void EtaCspPwrSocShutdown(void);

//
// EtaCspPwrWakePinGet - return the state of the wake pin.
//
extern bool EtaCspPwrWakePinGet(void);

//
//EtaCspPwrFlashPwrUpCompleteGet- return flash power up complete state.
//
extern bool EtaCspPwrFlashPwrUpCompleteGet(void);

//
// EtaCspPwrFlashV25Set - Flash 2.5V on or off.
//
extern void EtaCspPwrFlashV25Set(bool);

//
// EtaCspPwrFlashV25Get - return state of flash 2.5V switch.
//
extern bool EtaCspPwrFlashV25Get(void);

//
// EtaCspPwrFlashPDMSet - Flash PDM on or off.
//
extern void EtaCspPwrFlashPDMSet(bool);

//
// EtaCspPwrFlashPDMGet - return state of flash PDM switch.
//
extern bool EtaCspPwrFlashPDMGet(void);

//
// EtaCspPwrFlashVREFSet - turn Flash VREF on or off.
//
extern void EtaCspPwrFlashVREFSet(bool);

//
// EtaCspPwrFlashVREFGet - return state of Flash VREF.
//
extern bool EtaCspPwrFlashVREFGet(void);

//
// EtaCspPwrDspPwrStateGet - return power state of DSP.
//
extern bool EtaCspPwrDspPwrStateGet(void);

//
// EtaCspPwrAdcPwrStateGet - return power state of the ADC.
//
extern bool EtaCspPwrAdcPwrStateGet(void);

//
// EtaCspPwrStateAllGet - return power state of everything.
//
extern uint32_t EtaCspPwrAllPwrStateGet(void); 


#ifdef __cplusplus
}
#endif

#endif // __ETA_CSP_PWR_H__

