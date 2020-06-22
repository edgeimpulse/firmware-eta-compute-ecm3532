/***************************************************************************//**
 *
 * @file eta_csp_socctrl.h
 *
 * @brief This file contains eta_csp_socctrl module definitions.
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
 * @addtogroup ecm3532socctrl-m3 SoC Control (SOCCTRL)
 * @ingroup ecm3532csp-m3
 * @{
 ******************************************************************************/

#ifndef __ETA_CSP_SOCCTRL_H__
#define __ETA_CSP_SOCCTRL_H__

#include <stdint.h>
#include "eta_csp.h"
#include "eta_csp_gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __LANGUAGE_ASM__

typedef enum
{
    eStallGpio0   = 0x1,
    eStallGpio1   = 0x2,
    eStallTimer0  = 0x4,
    eStallTimer1  = 0x8,
    eStallUart0   = 0x10,
    eStallUart1   = 0x20,
    eStallSpi0    = 0x40,
    eStallSpi1    = 0x80,
    eStallI2c0    = 0x100,
    eStallI2c1    = 0x200,
    eStallRtc0    = 0x400,
    eStallRtc1    = 0x800,
    eStallDsp     = 0x1000,
    eStallAdc     = 0x2000,
    eStallPwm     = 0x4000,
    eStallWdt     = 0x8000,
    eStallRtcTmr  = 0x10000,
    eStallAoBod   = 0x20000,

    eStallAnyInt  = 0x3ffff,
    eStallNone    = 0x0,
}
tStallIntBit;

//
//! Pad mux enum typedef.
//
typedef enum
{
    eSocCtrlPadMux0 = 0,
    eSocCtrlPadMux1 = 1,
    eSocCtrlPadMux2 = 2,
    eSocCtrlPadMux3 = 3,
    eSocCtrlPadMux4 = 4,
}
tSocCtrlPadMux;

//
//! Reset Status Bit enum typedef.
//
typedef enum
{
    eSocCtrlResetStatusPor      = 0x01,
    eSocCtrlResetStatusVector   = 0x02,
    eSocCtrlResetStatusAirCr    = 0x04,
    eSocCtrlResetStatusDap      = 0x08,
    eSocCtrlResetStatusWatchdog = 0x10,
    eSocCtrlResetStatusPin      = 0x20,
}
tSocCtrlResetStatusBit;

//
//! UART/SPI/TIMER CLOCK SOURCE SELECTOR
//
typedef enum
{
    eSocCtrlUSTSelError = 0x00,
    eSocCtrlUSTSel2MHz  = 0x01,
    eSocCtrlUSTSel4MHz  = 0x02,
    eSocCtrlUSTSel8MHz  = 0x03,
}
tSocCtrlUSTSelect;

//
//! DSP CLOCK SOURCE SELECTOR
//
typedef enum
{
    eSocCtrlDspSelError = 0x00,
    eSocCtrlDspSel2MHz  = 0x01,
    eSocCtrlDspSel4MHz  = 0x02,
    eSocCtrlDspSel8MHz  = 0x03,
}
tSocCtrlDspSelect;

//
//! ADC CLOCK SOURCE SELECTOR
//
typedef enum
{
    eSocCtrlAdcSelError = 0x00,
    eSocCtrlAdcSelHfo1MHz  = 0x01,
    eSocCtrlAdcSelHfo2MHz  = 0x02,
    eSocCtrlAdcSelXtal2MHz = 0x03,
    eSocCtrlAdcSelPad2MHz  = 0x04,
}
tSocCtrlAdcSelect;

//
//! I2C CLOCK SOURCE SELECTOR
//
typedef enum
{
    eSocCtrlI2CSelError = 0x00,
    eSocCtrlI2CSel2MHz  = 0x01,
    eSocCtrlI2CSel4MHz  = 0x02,
}
tSocCtrlI2CSelect;

#endif // __LANGUAGE_ASM__

//
// This fence will compile inline and do only one byte reads from the RTC block.
//
#define EtaCspSocCtrlFenceFast() \
                             {REG_GPIO_DEBUG0_READ_ONLY.V = MEMIO8(REG_SOCCTRL_CFG_STATUS_ADDR);}

//
// Add a fence on the last bus operation to SOC Ctrl registers.
//
extern void EtaCspSocCtrlFence(void);

//
// Return the M3 operating frequency.
//
extern uint32_t EtaCspSocCtrlM3FrequencyGet(void);

//
// Return the DSP operating frequency.
//
extern uint32_t EtaCspSocCtrlDspFrequencyGet(void);

//
// GPIO pad mux select.
//
extern void EtaCspSocCtrlPadMuxSet(tGpioBit iBit, tSocCtrlPadMux iPadMux);

//
// Prime the interrupt control bit for Stall wake up
//
extern void EtaCspSocCtrlStallAllowIrq(tStallIntBit iStallBit);

//
// Stall the SOC to wake up on certain interrupt.
//
extern void EtaCspSocCtrlStall(tStallIntBit iStallBit);

//
// Stall the SOC prepare it to wake up on any interrupt.
//
extern void EtaCspSocCtrlStallAnyIrq(void);

//
// Enable Stall on the SOC.
//
extern void EtaCspSocCtrlStallEnable(void);

//
// Disable Stall on the SOC.
//
extern void EtaCspSocCtrlStallDisable(void);

//
// WFI Stall enable and disable
//
extern void EtaCspSocCtrlStallWfiEnable(void);
extern void EtaCspSocCtrlStallWfiDisable(void);

//
// Get the M3 reset status value.
//
extern uint32_t EtaCspSocCtrlResetStatusGet(void);

//
// Clear the M3 reset status value.
//
extern void EtaCspSocCtrlResetStatusClear(void);

//
// EtaCspSocCtrlUstFreqGet - Return current Clock UST Select
//
extern tSocCtrlUSTSelect EtaCspSocCtrlUstFreqGet(void);

//
// EtaCspSocCtrlUstFreqSet - Sets Clock UST Select
//
extern void EtaCspSocCtrlUstFreqSet(tSocCtrlUSTSelect sSelect);

//
// EtaCspSocCtrlDspFreqGet - Return current Clock DSP Select
//
extern tSocCtrlDspSelect EtaCspSocCtrlDspFreqGet(void);

//
// EtaCspSocCtrlDspFreqSet - Sets Clock DSP Select
//
extern void EtaCspSocCtrlDspFreqSet(tSocCtrlDspSelect sSelect);

//
// EtaCspSocCtrlAdcFreqGet - Return current Clock ADC Select
//
extern tSocCtrlAdcSelect EtaCspSocCtrlAdcFreqGet(void);

//
// EtaCspSocCtrlAdcFreqSet - Sets Clock ADC Select
//
extern void EtaCspSocCtrlAdcFreqSet(tSocCtrlAdcSelect sSelect);

//
// EtaCspSocCtrlI2CFreqGet - Return current Clock I2C Select
//
extern tSocCtrlI2CSelect EtaCspSocCtrlI2CFreqGet(void);

//
// EtaCspSocCtrlI2CFreqSet - Sets Clock I2C Select
//
extern void EtaCspSocCtrlI2CFreqSet(tSocCtrlI2CSelect sSelect);


#ifdef __cplusplus
}
#endif

#endif // __ETA_CSP_SOCCTRL_H__

