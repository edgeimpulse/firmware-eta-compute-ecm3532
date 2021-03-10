/***************************************************************************//**
 *
 * @file eta_csp_lfo.c
 *
 * @brief This file contains eta_csp_lfo module implementations.
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
 ******************************************************************************/

/***************************************************************************//**
 * @addtogroup ecm3532lfo-m3 Very High Frequency Oscillator (LFO)
 * @ingroup ecm3532csp-m3
 * @{
 ******************************************************************************/

#include "eta_chip.h"
#include "eta_chip.h"
#include "eta_csp.h"
#include "eta_csp_lfo.h"
#include "eta_csp_flash.h"
#include "eta_csp_hfo.h"

//
// The following can be uncommented for debug etaPrintf messages.
//
#include "eta_csp_uart.h"
#include "eta_csp_io.h"
//

// NOTE: this comes from the customer part of the INFO page at
// 0x110 and 0x114
#define LFO_TRIM_ADDRESS (0x01000410)
#define LFO_TRIM_KEY_INDEX (2)  
#define LFO_TRIM_VALUE_INDEX (3)

/***************************************************************************//**
 *
 *  EtaCspLfoInit - Initialize the LFO if flash INFO page trim data is
 *                   available, use it.
 * @return  eta CSP status
 *
 * WARNING: This function assumes the flash is up and running when called.
 ******************************************************************************/
tEtaStatus
EtaCspLfoInit(void)
{
    uint32_t ui32Value[4];
    reg_rtc_osc_csr_t osc_values;

    //
    // Read the LFO trm values from flash INFO page.
    // x4 this is 0-x140
    //
    EtaCspFlashInfoRead(LFO_TRIM_ADDRESS, ui32Value);

    //
    // Check that LFO trim key value matches 0x9999AA55
    //
    if(ui32Value[LFO_TRIM_KEY_INDEX] == 0x9999AA55)
    {
        //
        // OK then we will use the trim value from flash INFO page.
        //
        osc_values.V = ui32Value[LFO_TRIM_VALUE_INDEX];
	REG_RTC_OSC_CSR.BF.LFO_TRIM = osc_values.BF.LFO_TRIM;
	REG_RTC_OSC_CSR.BF.LFO_36NA_TRIM = osc_values.BF.LFO_36NA_TRIM;
	REG_RTC_OSC_CSR.BF.LFO_36NA_DIS = osc_values.BF.LFO_36NA_DIS;
	REG_RTC_OSC_CSR.BF.LFO_3NA_DIS = osc_values.BF.LFO_3NA_DIS;
    }


    return(eEtaSuccess);
}



/***************************************************************************//**
 *
 *  EtaCspLfo36nATrimSet - Set the LFO 36nA current mode trim.
 *
 * @param ui32TrimCl new capacitive trim value.
 *
 ******************************************************************************/
void
EtaCspLfo36nATrimSet(uint32_t ui32Trim36nA)
{
    REG_RTC_OSC_CSR.BF.LFO_36NA_TRIM = ui32Trim36nA;
}

/***************************************************************************//**
 *
 *  EtaCspLfo36nATrimGet - Get the LFO 32nA trim (LFO_36na_TRIM)
 *
 * @return current setting of 36nA trim field for the LFO.
 *
 * This is the current mode reference trim value.
 *
 ******************************************************************************/
uint32_t
EtaCspLfo36nATrimGet(void)
{
    return(REG_RTC_OSC_CSR.BF.LFO_36NA_TRIM);
}

/***************************************************************************//**
 *
 *  EtaCspLfoTrimSet - Set the LFO gain trim (LfoTrim).
 *
 * @param ui32LfoTrim new 8-bit trim value in a 32-bit reg.
 *
 * This is the frequency trim value.
 *
 ******************************************************************************/
void
EtaCspLfoTrimSet(uint32_t ui8LfoTrim)
{
    REG_RTC_OSC_CSR.BF.LFO_TRIM = ui8LfoTrim;
}


/***************************************************************************//**
 *
 *  EtaCspLfoTrimGet - Get the 8-bit value of the  LFO trim bit field.
 *
 * @return current 8-bit value of the LFO trim bit field.
 *
 ******************************************************************************/
uint32_t
EtaCspLfoTrimGet(void)
{
    return(REG_RTC_OSC_CSR.BF.LFO_TRIM);
}

/***************************************************************************//**
 *
 *  EtaCspLfoTrimRecord - Record the current trim settings in the flash INO
 *                   page so that it will be used on all subsequent calls
 *                   to initialize the LFO.
 * @return  eta CSP status
 *          this function will return status fail if any trim values have
 *          already been programmed in to the INFO page. This is OTP!!!
 *
 *
 * WARNING: This function assumes the flash is up and running when called.
 ******************************************************************************/
tEtaStatus
EtaCspLfoTrimRecord(void)
{
    tEtaStatus EtaStatus = eEtaSuccess;
    uint32_t ui32Value[4];
    uint32_t ui32InfoPage[ETA_CSP_FLASH_PAGE_SIZE_WORDS];
    uint32_t ui32AddressKey   = (LFO_TRIM_ADDRESS & ~ETA_CSP_FLASH_PAGE_SIZE_MASK)
                              + (LFO_TRIM_KEY_INDEX<<2);
    uint32_t ui32AddressValue = (LFO_TRIM_ADDRESS & ~ETA_CSP_FLASH_PAGE_SIZE_MASK)
                              + (LFO_TRIM_VALUE_INDEX<<2);

    //
    // Read the LFO trm values from flash INFO page.
    //
    EtaCspFlashInfoRead(LFO_TRIM_ADDRESS, ui32Value);

    //
    // Check that LFO trim key value and trim value are in their unprogrammed
    // state.
    //
    if(   (ui32Value[LFO_TRIM_KEY_INDEX] != 0xFFFFFFFF) 
       || (ui32Value[LFO_TRIM_VALUE_INDEX] != 0xFFFFFFFF))
    {
        return(eEtaFailure);
    }

    //
    // , read the entire INFO page.
    //
    EtaCspFlashInfoGet(ETA_CSP_FLASH_BASE,(uint8_t *)ui32InfoPage,ETA_CSP_FLASH_PAGE_SIZE_BYTES);


    //
    // Setup for an 8 byte program operation to the flash INFO page.
    //
    ui32InfoPage[ui32AddressKey >> 2]   = 0x9999AA55;
    ui32InfoPage[ui32AddressValue >> 2] = REG_RTC_OSC_CSR.V
                                        & (   BM_RTC_OSC_CSR_LFO_3NA_DIS
                                            | BM_RTC_OSC_CSR_LFO_36NA_DIS
                                            | BM_RTC_OSC_CSR_LFO_36NA_TRIM
                                            | BM_RTC_OSC_CSR_LFO_TRIM);


    //
    // The trim key and trim value are OK to program so do it.
    //
    ETA_CSP_FLASH_INFO_ERASE();
    EtaStatus = ETA_CSP_FLASH_INFO_PROGRAM(ETA_CSP_FLASH_BASE, 
                                           (uint8_t *)ui32InfoPage, 
                                            ETA_CSP_FLASH_PAGE_SIZE_BYTES);

    //
    // Return success (I hope).
    //
    return(EtaStatus);
}

