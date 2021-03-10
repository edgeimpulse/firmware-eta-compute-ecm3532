/*******************************************************************************
*
* @file rtc_hal.h
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
#ifndef H_RTC_HAL_H_
#define H_RTC_HAL_H_
#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
/** Rtc Alarm Callback function proto */
typedef void (*tRtcAlarmCb)(void);

/** RTC time date struct*/
struct RtcTm
{
    /** time hour in 24 hr format */
    uint8_t hour;
    /** rtc minutes*/
    uint8_t min;
    /** rtc seconds*/
    uint8_t sec;
    /** rtc year*/
    uint16_t year;
    /** rtc month*/
    uint8_t  month;
    /** rtc date*/
    uint8_t  date;
    /** rtc week day*/
    uint8_t day;
};

#define CURR_CENTURY_START 2000
#define CURR_CENTURY_END 2099
#define JULIAN_DATE_BASE     2440588   // Unix epoch time in Julian calendar (UnixTime = 00:00:00 01.01.1970 => JDN = 2440588)

/**
 * Get Time in Epoch format
 *
 * @param ui32Timeptr if called with valid pointer variable, Epoch time will be filled
 *
 * @return Epoch time will be returned
 */
uint32_t GetEpochTime(uint32_t *ui32Timeptr);

/**
 * RTC Time to Epoch format
 *
 * @param sRtcTm contain valid rtc time
 *
 * @return Epoch time will be returned
 */
uint32_t RtcToEpoch(struct RtcTm *sRtcTm);

/**
 * Epoch to RTC format
 *
 * @param ui32EpochT time in Epoch format
 *
 * @param sRtcTm pointer in which RTC time will be filled
 *
 */
void RtcFromEpoch(uint32_t ui32EpochT, struct RtcTm *sRtcTm);

/**
 * Set RTC Time
 *
 * @param sRtcTm pointer RTC time to be set
 *
 * @return 0 on success, -ve value on error
 *
 */
int32_t RtcSetTime(struct RtcTm *sRtcTm);

/**
 * Get RTC Time
 *
 * @param sRtcTm pointer in which RTC time to be filled
 *
 */
void RtcGetTime(struct RtcTm *sRtcTm);

/**
 * Set RTC Alarm
 *
 * @param sRtcTm pointer in which RTC time to be filled
 *
 * @return 0 on success, -ve value on error
 *
 */
int RtcSetAlarm(struct RtcTm *sRtcTm);

/**
 * Disable RTC Alarm
 *
 */
void RtcDisableAlarm(void);

/*
 * RTC Register Alarm callback
 *
 * @param fAlarmCb function pointer to be called on Alarm expiry
 */
void RtcRegisterAlarmCb(tRtcAlarmCb fAlarmCb);

/*
 * Initialize RTC driver
 *
 * Will be called during system initialization
 *
 */
void RtcInit(void);
#endif /* H_RTC_HAL_H_ */
