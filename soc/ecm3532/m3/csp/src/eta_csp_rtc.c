/***************************************************************************//**
 *
 * @file eta_csp_rtc.c
 *
 * @brief This file contains eta_csp_rtc module implementations.
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

#include <stdint.h>
#include "eta_status.h"
#include "eta_chip.h"
#include "eta_csp.h"
#include "eta_csp_rtc.h"

/***************************************************************************//**
 *
 * EtaCspRtcResetModeInit - Initialize the restart mode based on bRunInFlash.
 *
 * @param bRunInFlash boolean indicating if the application is in flash or not.
 *
 ******************************************************************************/
void
EtaCspRtcRestartModeInit(bool bRunInFlash)
{
    //
    // Is the application in flash?
    //
    if(bRunInFlash)
    {
        EtaCspRtcRestartModeSet(eRestartWarmFlashNoTrimHfoTune);
    }
    else
    {
        EtaCspRtcRestartModeSet(eRestartWarmSramHfoTune);
    }
}

/***************************************************************************//**
 *
 *  EtaCspRtcRestartModeSet - Set the restart mode.
 *
 *  @param iMode the desired restart mode.
 *
 ******************************************************************************/
void
EtaCspRtcRestartModeSet(tRtcRestartMode iMode)
{
    //
    // Set the restart mode.
    //
    REG_RTC_AO_CSR.BF.WARM_START_MODE = iMode;

    //
    // Wait for the write to finish.
    //
    REG_GPIO_DEBUG0_READ_ONLY.V = REG_RTC_AO_CSR.V;
}

/***************************************************************************//**
 *
 *  EtaCspRtcRestartModeGet - Get the restart mode.
 *
 *  @return Return the restart mode.
 *
 ******************************************************************************/
tRtcRestartMode
EtaCspRtcRestartModeGet(void)
{
    return((tRtcRestartMode)REG_RTC_AO_CSR.BF.WARM_START_MODE);
}

/***************************************************************************//**
 *
 *  EtaCspRtcRestartModeClear - Clear the restart mode.
 *
 ******************************************************************************/
void
EtaCspRtcRestartModeClear(void)
{
    REG_RTC_AO_CSR.V = 0;

    //
    // Ensure the writes are complete before returning.
    //
    //fixme remove REG_GPIO_DEBUG1_READ_ONLY.V = REG_RTC_AO_CSR.V;
    EtaCspRtcFenceFast();
}

/***************************************************************************//**
 *
 *  EtaCspRtcIsColdStart - Determine if a cold start has occurred.
 *
 ******************************************************************************/
bool
EtaCspRtcIsColdStart(void)
{
    return(REG_RTC_AO_CSR.BF.WARM_START_MODE == 0);
}

/***************************************************************************//**
 *
 *  EtaCspRtcSoftwarePor - Issue a POR to the entire chip (except analog).
 *
 ******************************************************************************/
void
EtaCspRtcSoftwarePor(void)
{
    REG_RTC_SEND_POR.V = 0xdeadd00d;

    //
    // Kill time until the POR happens
    //
    while(1)
    {
    }
}

/***************************************************************************//**
 *
 *  EtaCspRtcClockOutCfgSet - Select a frequency to bring out on CLKOUT pin(s).
 *
 *  @param iFreq select from available frequencies.
 *
 *  @return Returns eEtaSuccess other returns eEtaFailure.
 *
 *  NOTE: the clock out will be enabled for any frequency selected. It will be
 *        disabled if eNone is selected.
 *
 ******************************************************************************/
tEtaStatus
EtaCspRtcClockOutCfgSet(tRtcClkOutFreqs iFreq)
{
    switch(iFreq)
    {
        case eNone:
        {
            REG_RTC_CLKOUT_CTRL.BF.SEL = BFV_RTC_CLKOUT_CTRL_SEL_NONE;
            break;
        }

        case eXtal32KHz:
        {
            REG_RTC_CLKOUT_CTRL.BF.SEL = BFV_RTC_CLKOUT_CTRL_SEL_32KHZ;
            break;
        }

        case eXtal16KHz:
        {
            REG_RTC_CLKOUT_CTRL.BF.SEL = BFV_RTC_CLKOUT_CTRL_SEL_16KHZ;
            break;
        }

        case eXtal8KHz:
        {
            REG_RTC_CLKOUT_CTRL.BF.SEL = BFV_RTC_CLKOUT_CTRL_SEL_8KHZ;
            break;
        }

        case eXtal4KHz:
        {
            REG_RTC_CLKOUT_CTRL.BF.SEL = BFV_RTC_CLKOUT_CTRL_SEL_4KHZ;
            break;
        }

        case eVHF2MHz:
        {
            REG_RTC_CLKOUT_CTRL.BF.SEL = BFV_RTC_CLKOUT_CTRL_SEL_2MHZ;
            break;
        }

        case eVHF1MHz:
        {
            REG_RTC_CLKOUT_CTRL.BF.SEL = BFV_RTC_CLKOUT_CTRL_SEL_1MHZ;
            break;
        }

        case eVHF512KHz:
        {
            REG_RTC_CLKOUT_CTRL.BF.SEL = BFV_RTC_CLKOUT_CTRL_SEL_512KHZ;
            break;
        }

        case eVHF256KHz:
        {
            REG_RTC_CLKOUT_CTRL.BF.SEL = BFV_RTC_CLKOUT_CTRL_SEL_256KHZ;
            break;
        }

        case eAdcToggle:
        {
            REG_RTC_CLKOUT_CTRL.BF.SEL = BFV_RTC_CLKOUT_CTRL_SEL_ADC_TOGGLE;
            break;
        }

        default:
        {
            return(eEtaFailure);
        }
    }

    //
    // If we get here, then enable it
    //
    REG_RTC_CLKOUT_CFG.BF.EN = (iFreq != eNone) ? 1 : 0;

    //
    // Smile at the caller
    //
    return(eEtaSuccess);
}


// *****************************************************************************
//
// RTC SUPPORT FUNCTIONS
//
// *****************************************************************************

/***************************************************************************//**
 *
 *  EtaCspRtcTodSetDec - Set the time of day in decimal.
 *
 *  @param psTimeOfDay pointer to a time of day structure.
 *
 *  @return eEtaSuccess or eEtaFailure.
 *
 ******************************************************************************/
tEtaStatus
EtaCspRtcTodSetDec(tTimeOfDay *psTimeOfDay)
{
    //
    // validate inputs
    //
    bool bTwelve = psTimeOfDay-> bTwelve;
    if(   (psTimeOfDay->ui8Second    > 59)
        | (psTimeOfDay->ui8Minute    > 59)
        | (   bTwelve && (psTimeOfDay->ui8Hour > 12 || psTimeOfDay->ui8Hour == 0))
        | ( ! bTwelve && (psTimeOfDay->ui8Hour > 23))
        | (psTimeOfDay->ui8Day > 31)
        | ((psTimeOfDay->ui8Month == 4) && (psTimeOfDay->ui8Day > 30))
        | ((psTimeOfDay->ui8Month == 6) && (psTimeOfDay->ui8Day > 30))
        | ((psTimeOfDay->ui8Month == 9) && (psTimeOfDay->ui8Day > 30))
        | ((psTimeOfDay->ui8Month == 11) && (psTimeOfDay->ui8Day > 30))
        | ((psTimeOfDay->ui8Month == 2) && (psTimeOfDay->ui8Day > 29 ))
        | ((psTimeOfDay->ui8Month == 2) && (psTimeOfDay->ui16Year%4) && (psTimeOfDay->ui8Day == 29))
        | (psTimeOfDay->ui8Day == 0)
        | (psTimeOfDay->ui8DayOfWeek > 7)
        | (psTimeOfDay->ui8DayOfWeek == 0)
        | (psTimeOfDay->ui8Month > 12)
        | (psTimeOfDay->ui8Month == 0)
        | (psTimeOfDay->ui16Year > 99)
        | (psTimeOfDay->ui8Century > 1)
      )
    {
        return(eEtaFailure);
    }

    //
    // Collect the arguments
    //
    reg_rtc_toda_t uTimeOfDayA =
    {
        .BF.SECONDS = DEC_TO_BCD(psTimeOfDay->ui8Second),
        .BF.MINUTES = DEC_TO_BCD(psTimeOfDay->ui8Minute),
        .BF.HOURS = DEC_TO_BCD(psTimeOfDay->ui8Hour)
                  | ((psTimeOfDay->bTwelve & !psTimeOfDay->bAm) << 5),
        .BF.DATE = DEC_TO_BCD(psTimeOfDay->ui8Day),
        .BF.MONTH = DEC_TO_BCD(psTimeOfDay->ui8Month),
    };

    reg_rtc_todb_t uTimeOfDayB =
    {
        .BF.DAY = DEC_TO_BCD(psTimeOfDay->ui8DayOfWeek),
        .BF.YEAR = DEC_TO_BCD((uint8_t)(psTimeOfDay->ui16Year)),
        .BF.CENTURY = DEC_TO_BCD((uint8_t)(psTimeOfDay->ui8Century)),
        .BF.TWELVE_HOUR = psTimeOfDay->bTwelve,
    };

    //
    // Now write them to the RTC hardware.
    //
    // write TOB before writing TOA
    REG_RTC_TODB = uTimeOfDayB;
    REG_RTC_TODA = uTimeOfDayA;

    //
    // All is good
    //
    return(eEtaSuccess);
}

/***************************************************************************//**
 *
 *  EtaCspRtcTodGetDec - Get the time of day in decimal.
 *
 *  @param psTimeOfDay pointer to a time of day structure to be filled in.
 *
 ******************************************************************************/
void
EtaCspRtcTodGetDec(tTimeOfDay *psTimeOfDay)
{
    reg_rtc_toda_t uTimeOfDayA = REG_RTC_TODA;
    reg_rtc_todb_t uTimeOfDayB = REG_RTC_TODB;

    psTimeOfDay->bTwelve = uTimeOfDayB.BF.TWELVE_HOUR;
    psTimeOfDay->ui8Second = BCD_TO_DEC(uTimeOfDayA.BF.SECONDS);
    psTimeOfDay->ui8Minute = BCD_TO_DEC(uTimeOfDayA.BF.MINUTES);
    psTimeOfDay->ui8Hour = psTimeOfDay->bTwelve ? BCD_TO_DEC((uTimeOfDayA.BF.HOURS & 0x1F))
                                                : BCD_TO_DEC(uTimeOfDayA.BF.HOURS);
    psTimeOfDay->bAm = !((uTimeOfDayA.BF.HOURS >> 5) & 0x1);
    psTimeOfDay->ui8Day = BCD_TO_DEC(uTimeOfDayA.BF.DATE);
    psTimeOfDay->ui8DayOfWeek = BCD_TO_DEC(uTimeOfDayB.BF.DAY);
    psTimeOfDay->ui8Month = BCD_TO_DEC(uTimeOfDayA.BF.MONTH);
    psTimeOfDay->ui16Year = BCD_TO_DEC(uTimeOfDayB.BF.YEAR) + 2000;
    psTimeOfDay->ui8Century = BCD_TO_DEC(uTimeOfDayB.BF.CENTURY);
}

/***************************************************************************//**
 *
 *  EtaCspRtcAlarmSetDec - Set the time of day in decimal.
 *
 *  @param iAlarmNumber enumeration for alarma A or B.
 *  @param psTimeOfDay  pointer to a time of day structure.
 *  @param iEnables     alarm match enable enums.
 *
 *  @return general status: success or !success
 *
 *  @Note: ALARM interrupt should not be enabled when RTC_ALARMA/B is written. 
 *         The ALARM interrupt should be cleared after RTC_ALARMA/B is written.
 ******************************************************************************/
tEtaStatus
EtaCspRtcAlarmSetDec(tRtcAlarmNum iAlarmNumber, tTimeOfDay *psTimeOfDay,
                     tRtcAlarmEnable iEnables)
{
    switch(iAlarmNumber)
    {
        case eAlarmA:
        {  
            REG_RTC_CSRA.V = 0;
            //
            // Collect the arguments
            //
            reg_rtc_alarma_t uTimeOfDayA =
            {
                .BF.SECONDS = DEC_TO_BCD(psTimeOfDay->ui8Second),
                .BF.MINUTES = DEC_TO_BCD(psTimeOfDay->ui8Minute),
                .BF.HOURS = DEC_TO_BCD(psTimeOfDay->ui8Hour)
                          | ((psTimeOfDay->bTwelve & !psTimeOfDay->bAm) << 5),
                .BF.DAY = psTimeOfDay->bMatchDayOfWeek,
                .BF.DATE = (psTimeOfDay->bMatchDayOfWeek ? DEC_TO_BCD(psTimeOfDay->ui8DayOfWeek)
                                                         : DEC_TO_BCD(psTimeOfDay->ui8Day)),
                .BF.MONTH = DEC_TO_BCD(psTimeOfDay->ui8Month),
            };
            //
            // Now write to the RTC hardware.
            //
            REG_RTC_ALARMA = uTimeOfDayA;

            REG_W7(RTC_CSRA,
                   INT_STAT, 1,
                   MATCH_MONTHS, (iEnables & eAlarmMonths) ? 1 : 0,
                   MATCH_DAY_DATE, (iEnables & eAlarmDayDate) ? 1 : 0,
                   MATCH_HOURS, (iEnables & eAlarmHours) ? 1 : 0,
                   MATCH_MINUTES, (iEnables & eAlarmMinutes) ? 1 : 0,
                   MATCH_SECONDS, (iEnables & eAlarmSeconds) ? 1 : 0,
                   INT_ENABLE, 1);
            break;
        }

        case eAlarmB:
        {
            REG_RTC_CSRB.V = 0;
            //
            // Collect the arguments
            //
            reg_rtc_alarmb_t uTimeOfDayB =
            {
                .BF.SECONDS = DEC_TO_BCD(psTimeOfDay->ui8Second),
                .BF.MINUTES = DEC_TO_BCD(psTimeOfDay->ui8Minute),
                .BF.HOURS = DEC_TO_BCD(psTimeOfDay->ui8Hour)
                          | ((psTimeOfDay->bTwelve & !psTimeOfDay->bAm) << 5),
                .BF.DAY = psTimeOfDay->bMatchDayOfWeek,
                .BF.DATE = (psTimeOfDay->bMatchDayOfWeek ? DEC_TO_BCD(psTimeOfDay->ui8DayOfWeek)
                                                         : DEC_TO_BCD(psTimeOfDay->ui8Day)),
                .BF.MONTH = DEC_TO_BCD(psTimeOfDay->ui8Month),
            };
            //
            // Now write to the RTC hardware.
            //
            REG_RTC_ALARMB = uTimeOfDayB;

            REG_W7(RTC_CSRB,
                   INT_STAT, 1,
                   MATCH_MONTHS, (iEnables & eAlarmMonths) ? 1 : 0,
                   MATCH_DAY_DATE, (iEnables & eAlarmDayDate) ? 1 : 0,
                   MATCH_HOURS, (iEnables & eAlarmHours) ? 1 : 0,
                   MATCH_MINUTES, (iEnables & eAlarmMinutes) ? 1 : 0,
                   MATCH_SECONDS, (iEnables & eAlarmSeconds) ? 1 : 0,
                   INT_ENABLE, 1);
            break;
        }

        default:
        {
            return(eEtaFailure);
        }
    }
    return(eEtaSuccess);
}

/***************************************************************************//**
 *
 *  EtaCspRtcAlarmGetDec - Get the alarm settings in decimal.
 *
 *  @param iAlarmNumber enumeration for alarma A or B.
 *  @param psTimeOfDay pointer to a time of day structure to be filled in.
 *  @param piEnables   pointer to an enum to be filled in.
 *
 *  @return general status: success or !success
 *
 ******************************************************************************/
tEtaStatus
EtaCspRtcAlarmGetDec(tRtcAlarmNum iAlarmNumber, tTimeOfDay *psTimeOfDay,
                     tRtcAlarmEnable *piEnables)
{
    switch(iAlarmNumber)
    {
        case eAlarmA:
        {
            psTimeOfDay->bTwelve = REG_RTC_TODB.BF.TWELVE_HOUR;

            reg_rtc_alarma_t uTimeOfDayA = REG_RTC_ALARMA;

            psTimeOfDay->ui8Second = BCD_TO_DEC(uTimeOfDayA.BF.SECONDS);
            psTimeOfDay->ui8Minute = BCD_TO_DEC(uTimeOfDayA.BF.MINUTES);
            psTimeOfDay->bAm = !((uTimeOfDayA.BF.HOURS >> 5) & 0x1);
            psTimeOfDay->ui8Hour = psTimeOfDay->bTwelve ? BCD_TO_DEC((uTimeOfDayA.BF.HOURS & 0x1F))
                                                        : BCD_TO_DEC(uTimeOfDayA.BF.HOURS);
            psTimeOfDay->bMatchDayOfWeek = uTimeOfDayA.BF.DAY;
            if(psTimeOfDay->bMatchDayOfWeek)
            {
               psTimeOfDay->ui8DayOfWeek = BCD_TO_DEC(uTimeOfDayA.BF.DATE);
               psTimeOfDay->ui8Day = 0x0; // unknown from alarm clock register setting 
	    }
            else
            {
               psTimeOfDay->ui8Day = BCD_TO_DEC(uTimeOfDayA.BF.DATE);
               psTimeOfDay->ui8DayOfWeek = 0x0; // unknown from alarm clock register setting 
	    }
            psTimeOfDay->ui8Month = BCD_TO_DEC(uTimeOfDayA.BF.MONTH);
            psTimeOfDay->ui16Year = 0; 

            *piEnables = (REG_RTC_CSRA.V >> 8) & 0x0000001F;
            break;
        }

        case eAlarmB:
        {
            psTimeOfDay->bTwelve = REG_RTC_TODB.BF.TWELVE_HOUR;

            reg_rtc_alarmb_t uTimeOfDayB = REG_RTC_ALARMB;

            psTimeOfDay->bAm = !((uTimeOfDayB.BF.HOURS >> 5) & 0x1);
            psTimeOfDay->ui8Second = BCD_TO_DEC(uTimeOfDayB.BF.SECONDS);
            psTimeOfDay->ui8Minute = BCD_TO_DEC(uTimeOfDayB.BF.MINUTES);
            psTimeOfDay->ui8Hour = psTimeOfDay->bTwelve ? BCD_TO_DEC((uTimeOfDayB.BF.HOURS & 0x1F))
                                                        : BCD_TO_DEC(uTimeOfDayB.BF.HOURS);
            psTimeOfDay->bMatchDayOfWeek = uTimeOfDayB.BF.DAY;
            if(psTimeOfDay->bMatchDayOfWeek)
            {
               psTimeOfDay->ui8DayOfWeek = BCD_TO_DEC(uTimeOfDayB.BF.DATE);
               psTimeOfDay->ui8Day = 0x0; // unknown from alarm clock register setting 
	    }
            else
            {
               psTimeOfDay->ui8Day = BCD_TO_DEC(uTimeOfDayB.BF.DATE);
               psTimeOfDay->ui8DayOfWeek = 0x0; // unknown from alarm clock register setting 
	    }
            psTimeOfDay->ui8Month = BCD_TO_DEC(uTimeOfDayB.BF.MONTH);
            psTimeOfDay->ui16Year = 0; 

            *piEnables = (REG_RTC_CSRB.V >> 8) & 0x0000001F;
            break;
        }

        default:
        {
            return(eEtaFailure);
        }
    }
    return(eEtaSuccess);
}

/***************************************************************************//**
 *
 *  EtaCspRtcSecondTimerStart - Start the second timer.
 *
 *  @param iAlarmNumber enumeration for alarma A or B.
 *
 *  @return general status: success or !success
 ******************************************************************************/
tEtaStatus
EtaCspRtcSecondTimerStart(tRtcAlarmNum iAlarmNumber)
{
    switch(iAlarmNumber)
    {
        case eAlarmA:
        {
            REG_S3(RTC_CSRA, PER_SECOND, INT_ENABLE, INT_STAT);
            break;
        }

        case eAlarmB:
        {
            REG_S3(RTC_CSRB, PER_SECOND, INT_ENABLE, INT_STAT);
            break;
        }

        default:
        {
            return(eEtaFailure);
        }
    }
    return(eEtaSuccess);
}

/***************************************************************************//**
 *
 *  EtaCspRtcSecondTimerStop - Stop the second timer.
 *
 *  @param iAlarmNumber enumeration for alarma A or B.
 *
 *  @return general status: success or !success
 ******************************************************************************/
tEtaStatus
EtaCspRtcSecondTimerStop(tRtcAlarmNum iAlarmNumber)
{
    switch(iAlarmNumber)
    {
        case eAlarmA:
        {
            REG_C1(RTC_CSRA, PER_SECOND);
            break;
        }

        case eAlarmB:
        {
            REG_C1(RTC_CSRB, PER_SECOND);
            break;
        }

        default:
        {
            return(eEtaFailure);
        }
    }
    return(eEtaSuccess);
}

/***************************************************************************//**
 *
 *  EtaCspRtcMinuteTimerStart - Start the minute timer.
 *
 *  @param iAlarmNumber enumeration for alarma A or B.
 *
 *  @return general status: success or !success
 ******************************************************************************/
tEtaStatus
EtaCspRtcMinuteTimerStart(tRtcAlarmNum iAlarmNumber)
{
    switch(iAlarmNumber)
    {
        case eAlarmA:
        {
            REG_S3(RTC_CSRA, PER_MINUTE, INT_ENABLE, INT_STAT);
            break;
        }

        case eAlarmB:
        {
            REG_S3(RTC_CSRB, PER_MINUTE, INT_ENABLE, INT_STAT);
            break;
        }

        default:
        {
            return(eEtaFailure);
        }
    }
    return(eEtaSuccess);
}

/***************************************************************************//**
 *
 *  EtaCspRtcMinuteTimerStop - Stop the minute timer.
 *
 *  @param iAlarmNumber enumeration for alarma A or B.
 *
 *  @return general status: success or !success
 ******************************************************************************/
tEtaStatus
EtaCspRtcMinuteTimerStop(tRtcAlarmNum iAlarmNumber)
{
    switch(iAlarmNumber)
    {
        case eAlarmA:
        {
            REG_C1(RTC_CSRA, PER_MINUTE);
            break;
        }

        case eAlarmB:
        {
            REG_C1(RTC_CSRB, PER_MINUTE);
            break;
        }

        default:
        {
            return(eEtaFailure);
        }
    }
    return(eEtaSuccess);
}

/***************************************************************************//**
 *
 *  EtaCspRtcIntStatusGet - Get the Interrupt status
 *
 * @return Return the state
 *
 ******************************************************************************/
bool
EtaCspRtcIntStatusGet(tRtcAlarmNum iAlarmNumber)
{
    switch(iAlarmNumber)
    {
        default:
        case eAlarmA:
        {
            return(REG_RTC_CSRA.BF.INT_STAT);
        }

        case eAlarmB:
        {
            return(REG_RTC_CSRB.BF.INT_STAT);
        }
    }
}

/***************************************************************************//**
 *
 *  EtaCspRtcIntClear - Clear the RTC interrupt.
 *
 *  @param iAlarmNumber enumeration for alarma A or B.
 *
 *  @return general status: success or !success
 ******************************************************************************/
tEtaStatus
EtaCspRtcIntClear(tRtcAlarmNum iAlarmNumber)
{
    switch(iAlarmNumber)
    {
        case eAlarmA:
        {
            REG_M1(RTC_CSRA, INT_STAT, 1);

            //
            // Wait for interrupt to clear.
            //
            while(REG_R(RTC_CSRA, INT_STAT) == true)
            {
            }
            break;
        }

        case eAlarmB:
        {
            REG_M1(RTC_CSRB, INT_STAT, 1);

            //
            // Wait for interrupt to clear.
            //
            while(REG_R(RTC_CSRB, INT_STAT) == true)
            {
            }
            break;
        }

        default:
        {
            return(eEtaFailure);
        }
    }
    return(eEtaSuccess);
}

/***************************************************************************//**
 *
 *  EtaCspRtcIntEnable - Enable the RTC interrupt.
 *
 *  @param iAlarmNumber enumeration for alarma A or B.
 *
 *  @return general status: success or !success
 ******************************************************************************/
tEtaStatus
EtaCspRtcIntEnable(tRtcAlarmNum iAlarmNumber)
{
    switch(iAlarmNumber)
    {
        case eAlarmA:
        {
            //
            // RMW and set this bit.
            //
            REG_S1(RTC_CSRA, INT_ENABLE);
            break;
        }

        case eAlarmB:
        {
            //
            // RMW and set this bit.
            //
            REG_S1(RTC_CSRB, INT_ENABLE);
            break;
        }

        default:
        {
            return(eEtaFailure);
        }
    }

    //
    // Fence to make sure it is enabled before returning.
    //
    EtaCspRtcFenceFast();

    //
    // Return success.
    //
    return(eEtaSuccess);
}

/***************************************************************************//**
 *
 *  EtaCspRtcIntDisable - Disable the RTC interrupt.
 *
 *  @param iAlarmNumber enumeration for alarma A or B.
 *
 *  @return general status: success or !success
 ******************************************************************************/
tEtaStatus
EtaCspRtcIntDisable(tRtcAlarmNum iAlarmNumber)
{
    switch(iAlarmNumber)
    {
        case eAlarmA:
        {
            REG_C1(RTC_CSRA, INT_ENABLE);
            break;
        }

        case eAlarmB:
        {
            REG_C1(RTC_CSRB, INT_ENABLE);
            break;
        }

        default:
        {
            return(eEtaFailure);
        }
    }
    return(eEtaSuccess);
}

/***************************************************************************//**
 *
 *  EtaCspRtcIntEnableGet - Get the Interrupt enable status
 *
 * @return Return the state
 *
 ******************************************************************************/
bool
EtaCspRtcIntEnableGet(tRtcAlarmNum iAlarmNumber)
{
    switch(iAlarmNumber)
    {
        case eAlarmA:
        {
            return(REG_RTC_CSRA.BF.INT_ENABLE);
        }

        case eAlarmB:
        {
            return(REG_RTC_CSRB.BF.INT_ENABLE);
        }

        default:
        {
            return(0);
        }
    }
}

/***************************************************************************//**
 *
 *  EtaCspRtcSecondsGet - Get the seconds read.
 *
 * @return Return the seconds read.
 *
 ******************************************************************************/
uint32_t
EtaCspRtcSecondsGet(void)
{
    return(BCD_TO_DEC(REG_RTC_TODA.BF.SECONDS));
}

/***************************************************************************//**
 *
 *  EtaCspRtcIsMasterTmrInt - Determine if a TMR has interrupted
 *  This is a copy of the status bit that is much closer to the M3 oscillator
 *  domain. It can be read at much less expense than an RTC register in the AO.
 *
 ******************************************************************************/
bool
EtaCspRtcIsMasterTmrInt(void)
{
    return(REG_GPIO_XTERN_STATUSA.BF.RTC_TMR_INT == 1);
}

/***************************************************************************//**
 *
 *  EtaCspRtcTmrIntStatusGet - Get the RTC interrupt status.
 *
 * @return Return true if interrupt occurred.
 *
 ******************************************************************************/
bool
EtaCspRtcTmrIntStatusGet(void)
{
    return(REG_RTC_TMR.BF.INT_STAT == 1);
}

/***************************************************************************//**
 *
 * EtaCspRtcTmrIntClear - Clear the RTC Timer interrupt.
 *
 ******************************************************************************/
void
EtaCspRtcTmrIntClear(void)
{
    REG_RTC_TMR.BF.INT_STAT = 1;
}

/***************************************************************************//**
 *
 *  EtaCspRtcTmrClear - Clear the RTC TMR interrupt.
 *
 ******************************************************************************/
void
EtaCspRtcTmrClear(void)
{
    REG_S1(RTC_TMR, INT_STAT);

    //
    // Wait for interrupt to clear.
    //
    while(REG_R(RTC_TMR, INT_STAT) == true)
    {
    }
}

/***************************************************************************//**
 *
 * EtaCspRtcTmrIntEnableGet - Get the RTC interrupt enable state.
 *
 * @return Return true if interrupt occurred.
 *
 ******************************************************************************/
bool
EtaCspRtcTmrIntEnableGet(void)
{
    return(REG_RTC_TMR.BF.INT_ENABLE == 1);
}

/***************************************************************************//**
 *
 *  EtaCspRtcTmrPeriodic - Start the RTC TMR in periodic mode
 *
 *  @param ui16ReloadMS The desired period between interrupts in approximate
 *  mSeconds (1000mS/1024).
 *
 ******************************************************************************/
void
EtaCspRtcTmrPeriodic(uint32_t ui32ReloadMS)
{
    //
    // First, set the reload value
    //
    REG_RTC_TMR_RELOAD.V = ui32ReloadMS;

    //
    // Now kick of the RTC TMR. The interrupt status bit will be set.
    //
    REG_M5(RTC_TMR, INT_STAT, 1,
           TIMER, 1,
           PERIODIC, 1,
           INT_ENABLE, 1,
           COUNT, ui32ReloadMS); // ticks
}

/***************************************************************************//**
 *
 * EtaCspRtcTmrModeGet - Get the RTC operating mode.
 *
 * @return Return true if in periodic mode.
 *
 ******************************************************************************/
bool
EtaCspRtcTmrPeriodicModeGet(void)
{
    return(!!REG_RTC_TMR.BF.PERIODIC);
}

/***************************************************************************//**
 *
 * EtaCspRtcTmrDisable - Disable the RTC Operation.
 *
 ******************************************************************************/
void
EtaCspRtcTmrDisable(void)
{
    REG_RTC_TMR.V = 0;
}

// *****************************************************************************
//
// WATCHDOG TIMER SUPPORT FUNCTIONS
//
// *****************************************************************************

/***************************************************************************//**
 *
 *  EtaCspRtcWdtInit - Initialize the Watchdog Timer
 *
 *  @param iWdtOperation - The desired WDT operation mode, Interrupt or POR 
 *                         or RESET or any combinations.
 *  @param ui32TiggerLevel The desired interrupt compare count.
 *  @param ui32Reload - The desired starting count down value.
 *  @return  Return the success status.
 *
 *  @Note If POR is enabled, POR is issued when ui32Reload value counts down to 0.
 *        If Interrupt is enabled, interrupt is set when ui32Reload counts down
 *        to ui32TriggerLevel. 
 ******************************************************************************/
tEtaStatus
EtaCspRtcWdtInit(tRtcWdtOp iWdtOperation,
                 uint32_t ui32TriggerLevel,
                 uint32_t ui32Reload)
{
    //
    // validate the operating modes requested.
    //
    if(iWdtOperation & ~(eWdtPor | eWdtInterrupt | eWdtReset))
    {
        return(eEtaFailure);
    }

    //
    // You have to ask for at least one 
    //
    if(!iWdtOperation)
    {
        return(eEtaFailure);
    }

    //
    // Check trigger range.
    //
    if(ui32TriggerLevel > ((1<<20)-1))
    {
        return(eEtaFailure);
    }

    //
    // Check reload range.
    //
    if(ui32Reload > ((1<<20)-1))
    {
        return(eEtaFailure);
    }


    //
    // Set the trigger level
    //
    REG_RTC_WDT_ALARM.BF.VALUE = ui32TriggerLevel;

    //
    // Set the Reload Count
    //
    REG_RTC_WDT_RELOAD.BF.VALUE = ui32Reload;

    //
    // Are we setting up for POR when the WDT expires?
    //
    if(iWdtOperation & eWdtPor)
    {
        REG_S1(RTC_WDT_ALARM, POR);
    }
    else
    {
        REG_C1(RTC_WDT_ALARM, POR);
    }

    //
    // Are we setting up for RESET when the WDT expires?
    //
    if(iWdtOperation & eWdtReset)
    {
        REG_S1(RTC_WDT_ALARM, RESET);
    }
    else
    {
        REG_C1(RTC_WDT_ALARM, RESET);
    }

    //
    // Are we setting up for simple interrupt generation?
    //
    if(iWdtOperation & eWdtInterrupt)
    {
        REG_S1(RTC_WDT_ALARM, IRQ_EN);
    }
    else
    {
        REG_C1(RTC_WDT_ALARM, IRQ_EN);
    }


    //
    // OK, load the counter.
    //
    REG_RTC_WDT_COUNTER.V = 0x00000000;

    return(eEtaSuccess);
}

/***************************************************************************//**
 *
 *  EtaCspRtcWdtStart - Kick off the watchdog timer.
 *
 ******************************************************************************/
void
EtaCspRtcWdtStart(void)
{
    //
    // OK, let the wdt timer run .
    //
    REG_S1(RTC_WDT_ALARM, WDT_RUN);
}

/***************************************************************************//**
 *
 *  EtaCspRtcWdtRestart - pet the watchdog timer.
 *
 ******************************************************************************/
void
EtaCspRtcWdtRestart(void)
{
    REG_RTC_WDT_COUNTER.V = 0x00000000;
}

/***************************************************************************//**
 *
 *  EtaCspRtcWdtStop - Stop the watchdog timer.
 *
 ******************************************************************************/
void
EtaCspRtcWdtStop(void)
{
    REG_C1(RTC_WDT_ALARM, WDT_RUN);
}

/***************************************************************************//**
 *
 * EtaCspRtcWdtOperationModeGet - Get the Watchdog Timer Operating Mode.
 *
 * @return  Return the potentially combined operating mode information.
 *
 * NOTE: the enums are one hot encoded so they can be ORed together.
 *
 ******************************************************************************/
tRtcWdtOp
EtaCspRtcWdtOperationModeGet(void)
{
    int iOpMode = 0;

    //
    // check for POR enabled.
    //
    if(REG_RTC_WDT_ALARM.BF.POR)
    {
        iOpMode |= eWdtPor;
    }

    //
    // check for RESET enabled.
    //
    if(REG_RTC_WDT_ALARM.BF.RESET)
    {
        iOpMode |= eWdtReset;
    }

    //
    // check for INT enabled.
    //
    if(REG_RTC_WDT_ALARM.BF.IRQ_EN)
    {
        iOpMode |= eWdtInterrupt;
    }

    return(iOpMode);
}

/***************************************************************************//**
 *
 *  EtaCspRtcWdtIntGet - Get the Watchdog Timer Interrupt Status Bit.
 *
 ******************************************************************************/
bool
EtaCspRtcWdtIntGet(void)
{
    return(REG_RTC_WDT_ALARM.BF.INT_STAT);
}

/***************************************************************************//**
 *
 *  EtaCspRtcWdtIntClear - Clear the Watchdog Timer Interrupt Status Bit.
 *
 ******************************************************************************/
void
EtaCspRtcWdtIntClear(void)
{
    REG_S1(RTC_WDT_ALARM, INT_STAT);

    // Do a read from the apb_gpio to process the IRQ clear.
    // fixme remove after testing REG_RTC_DEBUG0_READ_ONLY.V = REG_RTC_WDT_ALARM.V; // fence
    EtaCspRtcFenceFast();
}

/***************************************************************************//**
 *
 *  EtaCspRtcWdtIntEnable - Clear the Watchdog Timer Interrupt Enable Bit.
 *
 ******************************************************************************/
void
EtaCspRtcWdtIntEnable(void)
{
    REG_S1(RTC_WDT_ALARM, IRQ_EN);

    // Do a read from the apb_gpio to process the IRQ clear.
    // fixme remove after testing REG_RTC_DEBUG0_READ_ONLY.V = REG_RTC_WDT_ALARM.V; // fence
    EtaCspRtcFenceFast();
}

/***************************************************************************//**
 *
 *  EtaCspRtcWdtIntDisable - Clear the Watchdog Timer Interrupt Enable Bit.
 *
 ******************************************************************************/
void
EtaCspRtcWdtIntDisable(void)
{
    REG_C1(RTC_WDT_ALARM, IRQ_EN);

    // Do a read from the apb_gpio to process the IRQ clear.
    // fixme remove after testing REG_RTC_DEBUG0_READ_ONLY.V = REG_RTC_WDT_ALARM.V; // fence
    EtaCspRtcFenceFast();
}

/***************************************************************************//**
 *
 *  EtaCspRtcWdtIntEnableGet - Get the Watchdog Timer Interrupt Enable Bit.
 *
 ******************************************************************************/
bool
EtaCspRtcWdtIntEnableGet(void)
{
    return(REG_RTC_WDT_ALARM.BF.IRQ_EN);
}

/***************************************************************************//**
 *
 * EtaCspRtcWdtResetEnable - Set the Watchdog Timer Reset Enable Bit.
 *
 ******************************************************************************/
void
EtaCspRtcWdtResetEnable(void)
{
    REG_S1(RTC_WDT_ALARM, RESET);
}

/***************************************************************************//**
 *
 * EtaCspRtcWdtResetDisable - Clear the Watchdog Timer Reset Enable Bit.
 *
 ******************************************************************************/
void
EtaCspRtcWdtResetDisable(void)
{
    REG_C1(RTC_WDT_ALARM, RESET);
}

/***************************************************************************//**
 *
 * EtaCspRtcWdtResetEnableGet - Get the Watchdog Timer Reset Enable Bit.
 *
 ******************************************************************************/
bool
EtaCspRtcWdtResetEnableGet(void)
{
    return(REG_RTC_WDT_ALARM.BF.RESET);
}

/***************************************************************************//**
 *
 *  EtaCspRtcWdtPorSet - Set the Watchdog Timer Por Enable Bit.
 *
 *  @param bNewPorState turn the POR function on or off.
 *
 ******************************************************************************/
void
EtaCspRtcWdtPorSet(bool bNewPorState)
{
    REG_RTC_WDT_ALARM.BF.POR = bNewPorState;
}

/***************************************************************************//**
 *
 *  EtaCspRtcWdtPorGet - Get the Watchdog Timer Por Enable Bit.
 *
 ******************************************************************************/
bool
EtaCspRtcWdtPorGet(void)
{
    return(REG_RTC_WDT_ALARM.BF.POR);
}

/***************************************************************************//**
 *
 *  EtaCspRtcWdtCountGet - Get the current Watchdog Timer Count Value.
 *
 ******************************************************************************/
uint32_t
EtaCspRtcWdtCountGet(void)
{
    return(REG_RTC_WDT_COUNTER.V);
}

/***************************************************************************//**
 *
 * EtaCspRtcFence - Add a fence on the last bus operation to the RTC.
 *
 ******************************************************************************/
void
EtaCspRtcFence(void)
{
    // fixme remove after testing REG_GPIO_DEBUG0_READ_ONLY.V = REG_RTC_CSRA.V;
    EtaCspRtcFenceFast();
}


/***************************************************************************//**
 *
 *  EtaCspRtcTmrTimedRun - Start the timer and notify when time has passed.
 *
 *  @param ui32TimeMs - The desired number of approximate milliseconds
 *                      (1000/1024) to delay for.
 *
 ******************************************************************************/
void
EtaCspRtcTmrTimedRun(uint32_t ui32TimeMs)
{
    REG_W5(RTC_TMR, INT_STAT, 1,
           TIMER, 1,
           PERIODIC, 0,
           INT_ENABLE, 1,
           COUNT, ui32TimeMs); // ticks

    // fixme remove REG_GPIO_DEBUG1_READ_ONLY.V = REG_RTC_TMR.V;
    EtaCspRtcFenceFast();
}

/***************************************************************************//**
 *
 *  EtaCspRtcTmrDelayMs - Polled RTC delay in milliseconds.
 *
 *  @param ui32TimeMs - The desired number of approximate milliseconds
 *                      (1000/1024) to delay for.
 *
 ******************************************************************************/
void
EtaCspRtcTmrDelayMs(uint32_t ui32TimeMs)
{
    //
    // Setup the RTC for the delay.
    //
    REG_W4(RTC_TMR, TIMER, 1, INT_ENABLE, 1, INT_STAT, 1, COUNT, ui32TimeMs);

    // fixme remove REG_GPIO_DEBUG1_READ_ONLY.V = REG_RTC_TMR.V;
    EtaCspRtcFenceFast();

    //
    // Wait for the RTC to generate an interrupt.
    //
    while(REG_RTC_TMR.BF.INT_STAT == 0)
    {
    }

    //
    // Clear the RTC interrupt.
    //
    EtaCspRtcTmrIntClear();
}

/***************************************************************************//**
 *
 * EtaCspRtcTmrIntEnable - Enable the RTC Timer interrupt.
 *
 ******************************************************************************/
void
EtaCspRtcTmrIntEnable(void)
{
    REG_RTC_TMR.BF.INT_ENABLE = 1;
}

/***************************************************************************//**
 *
 * EtaCspRtcTmrIntDisable - Disable the RTC Timer interrupt.
 *
 ******************************************************************************/
void
EtaCspRtcTmrIntDisable(void)
{
    REG_RTC_TMR.BF.INT_ENABLE = 0;
}

/***************************************************************************//**
 *
 *  EtaCspRtcMasterFlagGet - Retrieve the state of the RTC Toggle flag
 *  i.e. once per second.
 *
 ******************************************************************************/
bool
EtaCspRtcMasterFlagGet(void)
{
    return(REG_GPIO_XTERN_STATUSA.BF.RTC_FLAG == 0);
}


