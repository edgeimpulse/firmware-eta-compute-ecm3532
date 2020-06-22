/***************************************************************************//**
 *
 * @file eta_csp_tsense.c
 *
 * @brief This file contains eta_csp_tsense module implementations.
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
 * @addtogroup ecm3532tsense-m3 Temperature Sensor (TSENSE)
 * @ingroup ecm3532csp-m3
 * @{
 ******************************************************************************/

#include <stdint.h>
#include "eta_chip.h"
#include "eta_csp.h"
#include "eta_csp_flash.h"
#include "eta_csp_tsense.h"
#include "eta_csp_io.h"
#include "eta_csp_uart.h"

#define IFCSPDBG if(0)

static int32_t i32EtaCspTSenseIntercept;
static int64_t i64EtaCspTSenseSlope;
static int32_t i32EtaCspTSenseFrequencyA   = 0xFFFFFFFF;
static int32_t i32EtaCspTSenseTemperatureA = 0xFFFFFFFF;

// current confluence page says word 177 decimal = 708  byte offset in decimal
// 708 decimal == 0x2c4.  Read 4 words at 0x010002C0
// frequencyA is stored at 0x010002C0
// ATE temperature is stored at 0x010002C4 this degrees C not deci-degrees.
// Make sure these values are not 0xFFFFFFFF (erased) before using.
#define FLASH_INFO_PAGE_ADDR (0x010002c0)
#define FLASH_INFO_OFFSET_FREQUENCYA   (0)
#define FLASH_INFO_OFFSET_TEMPERATUREA (1)

//
// This value comes from analog simulations
//
#define ANA_SIM_INTERCEPT_TEMP (-224)

/***************************************************************************//**
 *
 *  EtaCspTSenseInit - Run at least once to setup temp conversion data.
 *
 ******************************************************************************/
void
EtaCspTSenseInit(void)
{
    uint32_t ui32ReadData[4];

    //
    // Read the tsense calibration values one time from FLASH INFO
    //
    ETA_CSP_FLASH_READ(FLASH_INFO_PAGE_ADDR, 1, ui32ReadData);
    i32EtaCspTSenseFrequencyA   = ui32ReadData[FLASH_INFO_OFFSET_FREQUENCYA];
    i32EtaCspTSenseTemperatureA = ui32ReadData[FLASH_INFO_OFFSET_TEMPERATUREA];

    IFCSPDBG etaPrintf("i32EtaCspTSenseFrequencyA = %d i32EtaCspTSenseTemperatureA %d\r\n",
          i32EtaCspTSenseFrequencyA,i32EtaCspTSenseTemperatureA );
    //
    // Compute the slope
    //
// fixme when the algorithm solidifies.
    i64EtaCspTSenseSlope  = (i32EtaCspTSenseTemperatureA 
                          -(ANA_SIM_INTERCEPT_TEMP)) * 65536L;
    IFCSPDBG etaPrintf("i64EtaCspTSenseSlope = %d\r\n",(uint32_t)i64EtaCspTSenseSlope);
    i64EtaCspTSenseSlope /= i32EtaCspTSenseFrequencyA;
    IFCSPDBG etaPrintf("i64EtaCspTSenseSlope = %d\r\n",(uint32_t)i64EtaCspTSenseSlope);


    //
    // Intercept is assumed to be effectively constant across all PV corners
    // convert it here to deci-degrees C
    //
    i32EtaCspTSenseIntercept = ANA_SIM_INTERCEPT_TEMP*10;
    IFCSPDBG etaPrintf("i32EtaCspTSenseIntercept = %d\r\n",i32EtaCspTSenseIntercept);

}

/***************************************************************************//**
 *
 *  EtaCspTSenseStart - Start a temperature measurement
 *
 *  @return EtaSuccess if the LDO is ready to go and the tsense has started.
 ******************************************************************************/
tEtaStatus
EtaCspTSenseStart(void)
{
    //
    // Make sure  the ADC LDO is ready.
    //
    if( ! REG_RTC_PWR.BF.ADC_PWR_STATE)
    {
        return eEtaFailure;
    }

    //
    // Start the temperature sensor
    //
    REG_M1(SOCCTRL_TSENSE_CTRL,RST_N,1);

    //
    // We successfully started a temperature read.
    //
    return eEtaSuccess;
}

/***************************************************************************//**
 *
 *  EtaCspTSenseOff - Turn off the temp sensor until next time.
 *
 ******************************************************************************/
void
EtaCspTSenseOff(void)
{
    //
    // Stop the tsense oscillator.
    //
    REG_M1(SOCCTRL_TSENSE_CTRL,RST_N,0);
}

/***************************************************************************//**
 *
 *  EtaCspTSenseIsReady - Check  temperature sensor ready state.
 *
 *  @return True if it is ready.
 *
 ******************************************************************************/
bool
EtaCspTSenseIsReady(void)
{
    return (REG_SOCCTRL_TSENSE_CTRL.BF.READY == 1);
}

/***************************************************************************//**
 *
 *  EtaCspTSenseGet - Return  temperature sensor value in deci-degrees C.
 *
 *  @return temp in degree C or -273  degrees C if it is in error.
 *         the temp is returned in deci-degrees C so 25C returns as 250 decimal.
 *
 ******************************************************************************/

int32_t
EtaCspTSenseGet (void)
{
    int32_t  i32Measured    = REG_SOCCTRL_TSENSE_FREQ.V;
    int32_t  i32Temperature;
    int64_t  i64Temperature = i32Measured * i64EtaCspTSenseSlope;

    IFCSPDBG etaPrintf("First i32Measured = %d i64Temperature = %d\r\n",
		        i32Measured, i64Temperature);

    //
    // First make sure the trim values are actually stored in flash INFO page.
    //
    if( (i32EtaCspTSenseFrequencyA == 0xFFFFFFFF) || (i32EtaCspTSenseTemperatureA == 0xFFFFFFFF))
    {
        //
        // slope and/or intercept were not programmed in the flash INFO page
        //
        return -2700; // assume 0 degrees Kelvin + 3
    }

    //
    // Make sure the temp sense frequency measurement is valid.
    //
    if( ! REG_SOCCTRL_TSENSE_CTRL.BF.READY)
    {
        //
        // frequency measurement was incomplete. Wait longer.
        //
        return -2710; // assume 0 degrees Kelvin + 2
    }

    //
    // Convert to deci-degrees C
    //
    i64Temperature  *= 10;

    //
    // descale the fractional slope.
    //
    i64Temperature  /= 65536;
    IFCSPDBG etaPrintf("i64Temperature = %d\r\n",  i64Temperature);

    //
    // Add the intercept
    //
    i32Temperature = (int32_t)i64Temperature;
    i32Temperature  += i32EtaCspTSenseIntercept;

    IFCSPDBG etaPrintf("i32Temperature = %d\r\n",  i32Temperature);

    //
    // make sure we are even in the range
    //
    if((i32Measured < 20000) ||(i32Measured > 50000))
    {
        //
        // Frequency is unexpectedly low or high.
        //
        return -2720; // assume 0 degrees Kelvin + 1
    }

    //
    // Does the temperature make sense
    //
    if((i32Temperature < -500) ||(i32Temperature > 1500))
    {
        //
        // Temperature is unexpectedly low or high.
        //
        return -2730; // assume 0 degrees Kelvin
    }

    return i32Temperature;
}

