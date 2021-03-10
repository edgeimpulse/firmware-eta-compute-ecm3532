/*******************************************************************************
*
* @file rtc_hal.c
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
#include "config.h"
#include <stddef.h>
#include "eta_csp_rtc.h"
#include "rtc_hal.h"
#include "errno.h"
#ifdef CONFIG_ECM3531
#include "ecm3531.h"
#elif CONFIG_ECM3532
#include "ecm3532.h"
#endif
#include "cm3.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "timers.h"

SemaphoreHandle_t xRtcSem;
tRtcAlarmCb fRtcHalAlarmCb = NULL;

#define MAX_HR (23)
#define MAX_MINU (60)
#define MAX_SEC (60)
#define MAX_MON (12)
#define MAX_DATE (31)
#define MAX_DAY (7)



/*
 * Set Rtc Time
 */
int32_t RtcSetTime(struct RtcTm *sTm)
{
    if ((sTm->year < CURR_CENTURY_START) ||
            (sTm->year > CURR_CENTURY_END))
        return -1;
    else
        sTm->year =  sTm->year - CURR_CENTURY_START;
  if ((sTm->hour > MAX_HR) || (sTm->min > MAX_MINU) ||
            (sTm->sec >  MAX_SEC)  ||
            (sTm->month > MAX_MON) || (sTm->date > MAX_DATE) ||
            (sTm->day > MAX_DAY))
    {
        return -1;

    }


    xSemaphoreTake(xRtcSem, portMAX_DELAY);

    reg_rtc_toda_t uTimeOfDayA =
    {
        .BF.SECONDS = DEC_TO_BCD(sTm->sec),
        .BF.MINUTES = DEC_TO_BCD(sTm->min),
        .BF.HOURS = DEC_TO_BCD(sTm->hour),
        .BF.DATE = DEC_TO_BCD(sTm->date),
        .BF.MONTH = DEC_TO_BCD(sTm->month),
    };

    reg_rtc_todb_t uTimeOfDayB =
    {
        .BF.DAY = DEC_TO_BCD(sTm->day),
        .BF.YEAR = DEC_TO_BCD((uint8_t)(sTm->year)),
    };

    REG_RTC_TODA = uTimeOfDayA;
    REG_RTC_TODB = uTimeOfDayB;

    xSemaphoreGive(xRtcSem);
    return 0;
}

/*
 * Get Rtc Time
 */
void RtcGetTime(struct RtcTm *sTm)
{
    xSemaphoreTake(xRtcSem, portMAX_DELAY);

    reg_rtc_toda_t uTimeOfDayA = REG_RTC_TODA;
    reg_rtc_todb_t uTimeOfDayB = REG_RTC_TODB;

    sTm->sec = BCD_TO_DEC(uTimeOfDayA.BF.SECONDS);
    sTm->min = BCD_TO_DEC(uTimeOfDayA.BF.MINUTES);
    sTm->hour = BCD_TO_DEC(uTimeOfDayA.BF.HOURS);
    sTm->date = BCD_TO_DEC(uTimeOfDayA.BF.DATE);
    sTm->day = BCD_TO_DEC(uTimeOfDayB.BF.DAY);
    sTm->month = BCD_TO_DEC(uTimeOfDayA.BF.MONTH);
    sTm->year = BCD_TO_DEC(uTimeOfDayB.BF.YEAR) + 2000;

    xSemaphoreGive(xRtcSem);
}

/*
 * Set Rtc Alarm
 */
int RtcSetAlarm(struct RtcTm *sTm)
{
    if ((sTm->hour > 23) || (sTm->min > 60) ||
            (sTm->sec > 60) || (sTm->month >  12) ||
            (sTm->date > 31))
        return -1;
    reg_rtc_alarma_t uAlarmTime =
    {
        .BF.HOURS = DEC_TO_BCD(sTm->hour),
        .BF.MINUTES = DEC_TO_BCD(sTm->min),
        .BF.SECONDS = DEC_TO_BCD(sTm->sec),
        .BF.DATE = DEC_TO_BCD(sTm->date),
        .BF.MONTH = DEC_TO_BCD(sTm->month),
    };

    REG_RTC_ALARMA.V = uAlarmTime.V;

    REG_W7(RTC_CSRA, INT_STAT,1,
            MATCH_HOURS, 1,
            MATCH_SECONDS, 1,
            MATCH_MINUTES, 1,
            MATCH_MINUTES, 1,
            MATCH_DAY_DATE, 1,
            INT_ENABLE, 1);
    // fence
    REG_GPIO_DEBUG1_READ_ONLY.V = REG_RTC_PWR.V;

    xSemaphoreGive(xRtcSem);
    return 0;
}

/*
 * Disable RTC Alarm
 */
void RtcDisableAlarm(void)
{
    xSemaphoreTake(xRtcSem, portMAX_DELAY);
    REG_W2(RTC_CSRA,
            INT_STAT, 1,
            INT_ENABLE, 0);
    // fence
    REG_GPIO_DEBUG1_READ_ONLY.V = REG_RTC_PWR.V;
    xSemaphoreGive(xRtcSem);
}

/*
 * Convert epoch time to RTC time
 */
void RtcFromEpoch(uint32_t epoch, struct RtcTm *sTm)
{
	uint32_t tm;
	uint32_t t1;
	uint32_t a;
	uint32_t b;
	uint32_t c;
	uint32_t d;
	uint32_t e;
	uint32_t m;
	int16_t  year  = 0;
	int16_t  month = 0;
	int16_t  dow   = 0;
	int16_t  mday  = 0;
	int16_t  hour  = 0;
	int16_t  min   = 0;
	int16_t  sec   = 0;
	uint64_t JD    = 0;
	uint64_t JDN   = 0;

	// These hardcore math's are taken from http://en.wikipedia.org/wiki/Julian_day

	JD  = ((epoch + 43200) / (86400 >>1 )) + (2440587 << 1) + 1;
	JDN = JD >> 1;

    tm = epoch; t1 = tm / 60; sec  = tm - (t1 * 60);
    tm = t1;    t1 = tm / 60; min  = tm - (t1 * 60);
    tm = t1;    t1 = tm / 24; hour = tm - (t1 * 24);

    dow   = JDN % 7;
    a     = JDN + 32044;
    b     = ((4 * a) + 3) / 146097;
    c     = a - ((146097 * b) / 4);
    d     = ((4 * c) + 3) / 1461;
    e     = c - ((1461 * d) / 4);
    m     = ((5 * e) + 2) / 153;
    mday  = e - (((153 * m) + 2) / 5) + 1;
    month = m + 3 - (12 * (m / 10));
    year  = (100 * b) + d - 4800 + (m / 10);

    sTm->year = year;
    sTm->month = month;
    sTm->date = mday;
    sTm->day = dow;
    sTm->hour = hour;
    sTm->min = min;
    sTm->sec = sec;
}

/*
 * Convert RTC to epoch time format
 */
uint32_t RtcToEpoch(struct RtcTm *sTm)
{
	uint8_t  a;
	uint16_t y;
	uint8_t  m;
	uint32_t JDN;

	// These hardcore math's are taken from http://en.wikipedia.org/wiki/Julian_day

	// Calculate some coefficients
	a = (14 - sTm->month) / 12;
	//y = (sTm->year + 2000) + 4800 - a; // years since 1 March, 4801 BC
	y = (sTm->year) + 4800 - a; // years since 1 March, 4801 BC
	m = sTm->month + (12 * a) - 3; // since 1 March, 4801 BC

	// Gregorian calendar date compute
    JDN  = sTm->date;
    JDN += (153 * m + 2) / 5;
    JDN += 365 * y;
    JDN += y / 4;
    JDN += -y / 100;
    JDN += y / 400;
    JDN  = JDN - 32045;
    JDN  = JDN - JULIAN_DATE_BASE;    // Calculate from base date
    JDN *= 86400;                     // Days to seconds
    JDN += sTm->hour * 3600;    // ... and today seconds
    JDN += sTm->min * 60;
    JDN += sTm->sec;

	return JDN;
}

/*
 * Get current time in Epoch format
 */
uint32_t GetEpochTime(uint32_t *etime)
{
    uint32_t tm;
    struct RtcTm ltime;
    RtcGetTime(&ltime);
    tm = RtcToEpoch(&ltime);
    if (etime)
        *etime = tm;
    return tm;
}

/*
 * RTC Register Alarm callback
 *
 */
void RtcRegisterAlarmCb(tRtcAlarmCb fAlarmCb)
{
    xSemaphoreTake(xRtcSem, portMAX_DELAY);
    fRtcHalAlarmCb = fAlarmCb;
    xSemaphoreGive(xRtcSem);
}

void RTC0_ISR(void)
{
    BaseType_t xHPTaskWoken = pdFALSE;
    // clear interrupt status bit
    REG_W2(RTC_CSRA, INT_STAT, 1, INT_ENABLE, 0);
    REG_GPIO_DEBUG0_READ_ONLY.V = REG_RTC_CSRA.V;
    if(fRtcHalAlarmCb)
        fRtcHalAlarmCb();
}

void RtcInit(void)
{
    xRtcSem = xSemaphoreCreateMutex();
    NVIC_SetPriority(RTC0_IRQn, (1 << __NVIC_PRIO_BITS) - 1);
    NVIC_EnableIRQ(RTC0_IRQn);
}
