/***************************************************************************//**
 *
 * @file eta_csp_vhfo.c
 *
 * @brief This file contains eta_csp_vhfo module implementations.
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
 * @addtogroup ecm3532vhfo-m3 Very High Frequency Oscillator (VHFO)
 * @ingroup ecm3532csp-m3
 * @{
 ******************************************************************************/

#include "eta_chip.h"
#include "eta_chip.h"
#include "eta_csp.h"
#include "eta_csp_vhfo.h"
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
#define VHFO_TRIM_ADDRESS (0x01000410)
#define VHFO_TRIM_KEY_INDEX (0)  
#define VHFO_TRIM_VALUE_INDEX (1)

/***************************************************************************//**
 *
 *  EtaCspVhfoInit - Initialize the VHFO if flash INFO page trim data is
 *                   available, use it.
 * @return  eta CSP status
 *
 * WARNING: This function assumes the flash is up and running when called.
 ******************************************************************************/
tEtaStatus
EtaCspVhfoInit(void)
{
    uint32_t ui32Value[4];

    //
    // Read the VHFO trm values from flash INFO page.
    // x4 this is 0-x140
    //
    EtaCspFlashInfoRead(VHFO_TRIM_ADDRESS, ui32Value);

    //
    // Check that VHFO trim key value matches 0x9999AA55
    //
    if(ui32Value[VHFO_TRIM_KEY_INDEX] == 0x9999AA55)
    {
        //
        // OK then we will use the trim value from flash INFO page.
        //
        REG_RTC_OSC_CSR2.V = ui32Value[VHFO_TRIM_VALUE_INDEX];
    }

    //
    // Now turn it on.
    //
    REG_RTC_OSC_CSR2.BF.VHFO_PD_N = 1;

    return(eEtaSuccess);
}

/***************************************************************************//**
 *
 *  EtaCspVhfoOff - Turn off the VHFO.
 *
 ******************************************************************************/
void
EtaCspVhfoOff(void)
{
    //
    // Now turn it off.
    //
    REG_RTC_OSC_CSR2.BF.VHFO_PD_N = 0;
}

/***************************************************************************//**
 *
 *  EtaCspVhfoTrimClSet - Set the VHFO capacitive trim (TrimCL).
 *
 * @param ui32TrimCl new capacitive trim value.
 *
 ******************************************************************************/
void
EtaCspVhfoTrimClSet(uint32_t ui32TrimCl)
{
    REG_RTC_OSC_CSR2.BF.VHFO_TRIM_CL = ui32TrimCl;
}

/***************************************************************************//**
 *
 *  EtaCspVhfoTrimClGet - Get the VHFO capacitive trim (TrimCl)
 *
 * @return current setting of capacitive trim field for the VHFO.
 *
 ******************************************************************************/
uint32_t
EtaCspVhfoTrimClGet(void)
{
    return(REG_RTC_OSC_CSR2.BF.VHFO_TRIM_CL);
}

/***************************************************************************//**
 *
 *  EtaCspVhfoTrimGmSet - Set the VHFO gain trim (TrimGm).
 *
 * @param ui32TrimGm new capacitive trim value.
 *
 ******************************************************************************/
void
EtaCspVhfoTrimGmSet(uint32_t ui32TrimGm)
{
    REG_RTC_OSC_CSR2.BF.VHFO_TRIM_GM = ui32TrimGm;
}

/***************************************************************************//**
 *
 *  EtaCspVhfoTrimGmGet - Get the VHFO capacitive trim (TrimGm)
 *
 * @return current setting of capacitive trim field for the VHFO.
 *
 ******************************************************************************/
uint32_t
EtaCspVhfoTrimGmGet(void)
{
    return(REG_RTC_OSC_CSR2.BF.VHFO_TRIM_GM);
}

/***************************************************************************//**
 *
 *  EtaCspVhfoTrimGet - Get the 32-bit value of the  VHFO trim register.
 *
 * @return current 32-bit value of the VHFO trim register.
 *
 ******************************************************************************/
uint32_t
EtaCspVhfoTrimGet(void)
{
    return(REG_RTC_OSC_CSR2.V);
}

/***************************************************************************//**
 *
 *  EtaCspVhfoTrimRecord - Record the current trim settings in the flash INO
 *                   page so that it will be used on all subsequent calls
 *                   to initialize the VHFO.
 * @return  eta CSP status
 *          this function will return status fail if any trim values have
 *          already been programmed in to the INFO page. This is OTP!!!
 *
 *
 * WARNING: This function assumes the flash is up and running when called.
 ******************************************************************************/
tEtaStatus
EtaCspVhfoTrimRecord(void)
{
    tEtaStatus EtaStatus = eEtaSuccess;
    uint32_t ui32Value[4];
    uint32_t ui32InfoPage[ETA_CSP_FLASH_PAGE_SIZE>>2];
    uint32_t ui32AddressKey   = (VHFO_TRIM_ADDRESS & ~ETA_CSP_FLASH_PAGE_SIZE_MASK)
                              + (VHFO_TRIM_KEY_INDEX<<2);
    uint32_t ui32AddressValue = (VHFO_TRIM_ADDRESS & ~ETA_CSP_FLASH_PAGE_SIZE_MASK)
                              + (VHFO_TRIM_VALUE_INDEX<<2);

    //
    // Read the VHFO trm values from flash INFO page.
    //
    EtaCspFlashInfoRead(VHFO_TRIM_ADDRESS, ui32Value);

    //
    // Check that VHFO trim key value and trim value are in their unprogrammed
    // state.
    //
    if(   (ui32Value[VHFO_TRIM_KEY_INDEX] != 0xFFFFFFFF) 
       || (ui32Value[VHFO_TRIM_VALUE_INDEX] != 0xFFFFFFFF))
    {
        return(eEtaFailure);
    }

    //
    // , read the entire INFO page.
    //
    EtaCspFlashInfoGet(ETA_CSP_FLASH_BASE,(uint8_t *)ui32InfoPage,ETA_CSP_FLASH_PAGE_SIZE);


    //
    // Setup for an 8 byte program operation to the flash INFO page.
    //
    ui32InfoPage[ui32AddressKey >> 2]   = 0x9999AA55;
    ui32InfoPage[ui32AddressValue >> 2] = REG_RTC_OSC_CSR2.V;


    //
    // The trim key and trim value are OK to program so do it.
    //
    ETA_CSP_FLASH_INFO_ERASE();
    EtaStatus = ETA_CSP_FLASH_INFO_PROGRAM(ETA_CSP_FLASH_BASE, 
                                           (uint8_t *)ui32InfoPage, 
                                            ETA_CSP_FLASH_PAGE_SIZE);

    //
    // Return success (I hope).
    //
    return(EtaStatus);
}

