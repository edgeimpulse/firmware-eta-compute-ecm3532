/***************************************************************************//**
 *
 * @file eta_csp_rtc.h
 *
 * @brief This file contains eta_csp_rtc module definitions.
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
 * @addtogroup ecm3532rtc-m3 Real-time Clock (RTC)
 * @ingroup ecm3532csp-m3
 * @{
 ******************************************************************************/

#ifndef __ETA_CSP_RTC_H__
#define __ETA_CSP_RTC_H__

#include <stdint.h>
#include "eta_status.h"
#include "eta_chip.h"
#include "memio.h"

#ifdef __cplusplus
extern "C" {
#endif

//
//! Decimal to BCD.
//
#define DEC_TO_BCD(ui32Dec) ((((uint8_t)ui32Dec / 10) << 4) | \
                             ((uint8_t)ui32Dec % 10))

//
//! BCD to decimal.
//
#define BCD_TO_DEC(ui32BCD) ((((uint8_t)ui32BCD >> 4) * 10) + \
                             ((uint8_t)ui32BCD & 0xF))

//
//! The time of day structure typedef.
//
typedef struct
{
    //
    //! Second.
    //
    uint8_t ui8Second;

    //
    //! Minute.
    //
    uint8_t ui8Minute;

    //
    //! Hour.
    //
    uint8_t ui8Hour;

    //
    //! Day.
    //
    uint8_t ui8Day;

    //
    //! Day of the week.
    //
    uint8_t ui8DayOfWeek;

    //
    //! Month of the year.
    //
    uint8_t ui8Month;

    //
    //! Year.
    //
    uint16_t ui16Year;

    //
    //! Century 
    //
    uint8_t ui8Century;

    //
    //! Alarm matching DayOfWeek or Day.
    //
    bool bMatchDayOfWeek;

    //
    //! True = 12 hour mode, false = 24 hour mode.
    //
    bool bTwelve;

    //
    //! True = AM in 12 hour mode, false = PM in 12 hour mode.
    //
    bool bAm;
}
tTimeOfDay;

//
//! RTC Alarm A or B Selector typedef.
//
typedef enum
{
    eAlarmA = 0x00,
    eAlarmB = 0x01,
}
tRtcAlarmNum;

//
//! RTC Alarm Enables typedef.
//
typedef enum
{
    eAlarmSeconds = 0x01,
    eAlarmMinutes = 0x02,
    eAlarmHours   = 0x04,
    eAlarmMonths  = 0x08,
    eAlarmDayDate = 0x10,
}
tRtcAlarmEnable;

//
//! RTC Watchdog Timer Operation typedef.
//! these can be combined.
//
typedef enum
{
    eWdtPor       = 0x01,
    eWdtReset     = 0x02,
    eWdtInterrupt = 0x04,
}
tRtcWdtOp;

//
//! RTC restart mode typedef.
//
typedef enum
{
    eRestartCold                     = 0x00,
    eRestartWarmFlash2SramCold       = 0x01,
    eRestartWarmSramNoHfoTune        = 0x02,
    eRestartWarmSramHfoTune          = 0x40,
    eRestartWarmFlashNoTrimHfoTune   = 0x80,
    eRestartWarmFlashTrimHfoTune     = 0x88,
    eRestartWarmFlashNoTrimNoHfoTune = 0x90,
    eRestartWarmFlashTrimNoHfoTune   = 0x98,
}
tRtcRestartMode;

//
//! CLKOUT Frequency/Source Selects.
//
typedef enum
{
    eNone      = 0x00,
    eXtal32KHz = 0x01,
    eXtal16KHz = 0x02,
    eXtal8KHz  = 0x03,
    eXtal4KHz  = 0x04,
    eVHF2MHz   = 0x05,
    eVHF1MHz   = 0x06,
    eVHF512KHz = 0x07,
    eVHF256KHz = 0x08,
    eAdcToggle = 0x09,
}
tRtcClkOutFreqs;

//
// This fence will compile inline and do only one byte reads from the RTC block.
//
#define EtaCspRtcFenceFast() {REG_GPIO_DEBUG0_READ_ONLY.V = MEMIO8(REG_RTC_CSRA_ADDR);}

//
// Initialize the restart mode.
//
extern void EtaCspRtcRestartModeInit(bool bRunInFlash);

//
// Set the restart mode.
//
extern void EtaCspRtcRestartModeSet(tRtcRestartMode iMode);

//
// Get the restart mode.
//
extern tRtcRestartMode EtaCspRtcRestartModeGet(void);

//
// EtaCspRtcRestartModeClear - Clear the restart mode.
//
extern void EtaCspRtcRestartModeClear(void);

//
// EtaCspRtcIsColdStart - Determine if a cold start has occurred.
//
extern bool EtaCspRtcIsColdStart(void);

//
// EtaCspRtcSoftwarePor - Issue a POR to the entire chip (except analog).
//
extern void EtaCspRtcSoftwarePor(void);

//
// Clock out config.
//
extern tEtaStatus EtaCspRtcClockOutCfgSet(tRtcClkOutFreqs iFreq);

// EtaCspRtcRevIdGet - Get the Chip Mask Revision info.
extern uint32_t EtaCspRtcRevIdGet(void);

// EtaCspRtcUnlockDebug - Override the Debugger lockout in flash protection.
extern void EtaCspRtcUnlockDebug(uint32_t ui32TheValue);

//
// EtaCspRtcIsMasterTmrInt - Determine if a TMR has interrupted.
//
extern bool EtaCspRtcIsMasterTmrInt(void);

//
// EtaCspRtcTodSetDec - Set the time of day in decimal.
//
extern tEtaStatus EtaCspRtcTodSetDec(tTimeOfDay *psTimeOfDay);

//
// EtaCspRtcTodGetDec - Get the time of day in decimal.
//
extern void EtaCspRtcTodGetDec(tTimeOfDay *psTimeOfDay);

//
// EtaCspRtcAlarmSetDec - Set the time of day in decimal.
//
extern tEtaStatus EtaCspRtcAlarmSetDec(tRtcAlarmNum iAlarmNumber,
                                       tTimeOfDay *psTimeOfDay,
                                       tRtcAlarmEnable iEnables);

//
// EtaCspRtcAlarmGetDec - Get the alarm settings in decimal.
//
extern tEtaStatus EtaCspRtcAlarmGetDec(tRtcAlarmNum iAlarmNumber,
                                       tTimeOfDay *psTimeOfDay,
                                       tRtcAlarmEnable *piEnables);

//
// EtaCspRtcSecondTimerStart - Start the second timer.
//
extern tEtaStatus EtaCspRtcSecondTimerStart(tRtcAlarmNum iAlarmNumber);

//
// EtaCspRtcSecondTimerStop - Stop the second timer.
//
extern tEtaStatus EtaCspRtcSecondTimerStop(tRtcAlarmNum iAlarmNumber);

//
// EtaCspRtcMinuteTimerStart - Start the minute timer.
//
extern tEtaStatus EtaCspRtcMinuteTimerStart(tRtcAlarmNum iAlarmNumber);

//
// EtaCspRtcMinuteTimerStop - Stop the minute timer.
//
extern tEtaStatus EtaCspRtcMinuteTimerStop(tRtcAlarmNum iAlarmNumber);

//
// EtaCspRtcIntStatusGet - Get the Interrupt status
//
extern bool EtaCspRtcIntStatusGet(tRtcAlarmNum iAlarmNumber);

//
// EtaCspRtcIntClear - Clear the RTC interrupt.
//
extern tEtaStatus EtaCspRtcIntClear(tRtcAlarmNum iAlarmNumber);

//
// EtaCspRtcIntEnable - Enable the RTC interrupt.
//
extern tEtaStatus EtaCspRtcIntEnable(tRtcAlarmNum iAlarmNumber);

//
// EtaCspRtcIntDisable - Disable the RTC interrupt.
//
extern tEtaStatus EtaCspRtcIntDisable(tRtcAlarmNum iAlarmNumber);

//
// EtaCspRtcIntEnableGet - Get the Interrupt enable status
//
extern bool EtaCspRtcIntEnableGet(tRtcAlarmNum iAlarmNumber);

//
// EtaCspRtcSecondsGet - Get the seconds read.
//
extern uint32_t EtaCspRtcSecondsGet(void);

//
// Get the RTC Timer interrupt status Get.
//
extern bool EtaCspRtcTmrIntStatusGet(void);

//
// Get the RTC Timer interrupt Enable Get.
//
extern bool EtaCspRtcTmrIntEnableGet(void);

//
// Clear the RTC Timer interrupt.
//
extern void EtaCspRtcTmrIntClear(void);

//
// EtaCspRtcTmrClear - Clear the RTC TMR interrupt.
//
extern void EtaCspRtcTmrClear(void);

//
// Start the RTC TMR in periodic mode
//
extern void EtaCspRtcTmrPeriodic(uint32_t ui32ReloadMS);

//
// EtaCspRtcTmrModeGet - Get the RTC operating mode.
//
extern bool EtaCspRtcTmrPeriodicModeGet(void);

//
// EtaCspRtcTmrDisable - Disable the RTC Operation.
//
extern void EtaCspRtcTmrDisable(void);

//
// EtaCspRtcWdtInit - Initialize the Watchdog Timer
//
extern tEtaStatus EtaCspRtcWdtInit(tRtcWdtOp iWdtOperation,
                                   uint32_t ui32TriggerLevel,
                                   uint32_t ui32Reload);

//
// EtaCspRtcWdtStart - Kick off the watchdog timer.
//
extern void EtaCspRtcWdtStart(void);

//
// EtaCspRtcWdtRestart - pet the watchdog timer.
//
extern void EtaCspRtcWdtRestart(void);

//
// EtaCspRtcWdtStop - Stop the watchdog timer.
//
extern void EtaCspRtcWdtStop(void);

//
// EtaCspRtcWdtOperationModeGet - Get the current Watchdog Timer Count Value.
//
extern tRtcWdtOp EtaCspRtcWdtOperationModeGet(void);

//
// EtaCspRtcWdtIntGet - Get the Watchdog Timer Interrupt Status Bit.
//
extern bool EtaCspRtcWdtIntGet(void);

//
// EtaCspRtcWdtIntClear - Clear the Watchdog Timer Interrupt Status Bit.
//
extern void EtaCspRtcWdtIntClear(void);

//
// EtaCspRtcWdtIntEnable - Set the Watchdog Timer Interrupt Enable Bit.
//
extern void EtaCspRtcWdtIntEnable(void);

//
// EtaCspRtcWdtIntDisable - Clear the Watchdog Timer Interrupt Enable Bit.
//
extern void EtaCspRtcWdtIntDisable(void);

//
// EtaCspRtcWdtIntEnableGet - Get the Watchdog Timer Interrupt Enable Bit.
//
extern bool EtaCspRtcWdtIntEnableGet(void);

//
// EtaCspRtcWdtResetEnable - Set the Watchdog Timer Reset Enable Bit.
//
extern void EtaCspRtcWdtResetEnable(void);

//
// EtaCspRtcWdtResetDisable - Clear the Watchdog Timer Reset Enable Bit.
//
extern void EtaCspRtcWdtResetDisable(void);

//
// EtaCspRtcWdtResetEnableGet - Get the Watchdog Timer Reset Enable Bit.
//
extern bool EtaCspRtcWdtResetEnableGet(void);

//
// EtaCspRtcWdtPorSet - Set the Watchdog Timer Por Enable Bit.
//
extern void EtaCspRtcWdtPorSet(bool bNewPorState);

//
// EtaCspRtcWdtPorGet - Get the Watchdog Timer Por Enable Bit.
//
extern bool EtaCspRtcWdtPorGet(void);

//
// EtaCspRtcWdtCountGet - Get the current Watchdog Timer Count Value.
//
extern uint32_t EtaCspRtcWdtCountGet(void);

//
// Add a fence on the last bus operation to the RTC.
//
extern void EtaCspRtcFence(void);

//
// Enable the RTC Timer interrupt.
//
extern void EtaCspRtcTmrIntEnable(void);

//
// Disable the RTC Timer interrupt.
//
extern void EtaCspRtcTmrIntDisable(void);

//
// The Master Flag status.
//
extern bool EtaCspRtcMasterFlagGet(void);

//
// Run for a set amount of time.
//
extern void EtaCspRtcTmrTimedRun(uint32_t ui32TimeMs);

//
// Delay for a number of millisconds.
//
void EtaCspRtcTmrDelayMs(uint32_t ui32TimeMs);


#ifdef __cplusplus
}
#endif

#endif // __ETA_CSP_RTC_H__

