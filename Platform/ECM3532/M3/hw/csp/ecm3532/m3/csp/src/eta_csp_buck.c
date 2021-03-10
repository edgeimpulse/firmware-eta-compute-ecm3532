/***************************************************************************//**
 *
 * @file eta_csp_buck.c
 *
 * @brief This file contains eta_csp_buck module implementations.
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
 * This is part of revision 1.0.0b1 of the Tensai Chip Support Package.
 *
 ******************************************************************************/

/***************************************************************************//**
 * @addtogroup ecm3532buck-m3 Buck
 * @ingroup ecm3532csp-m3
 * @{
 ******************************************************************************/

#include "eta_status.h"
#include "eta_chip.h"
#include "eta_csp.h"
#include "eta_csp_buck.h"
#include "eta_csp_flash.h"
#include "eta_csp_hfo.h"
#include "eta_csp_rtc.h"
#include "eta_csp_dsp.h"
#include "eta_csp_socctrl.h"


// Maintain the current state of the Buck CSR's
static tBuckAllState    sregBuckShadow;

// Internal function prototype
static tEtaStatus       EtaCspBuckAllSetFinish();


/*****************************************************************************
 *
 *  BuckVoltageFromFrequency - Returns an approximation of the voltage
 *                             of a Buck for a particular frequency
 *
 ******************************************************************************/
static uint32_t
BuckVoltageFromFrequency(uint32_t iFrequency)
{
    if (iFrequency <= 20000) {
        return (630);
    } else if (iFrequency <= 30000) {
        return (660);
    } else if (iFrequency <= 40000) {
        return (700);
    } else if (iFrequency <= 50000) {
        return (730);
    } else if (iFrequency <= 60000) {
        return (760);
    } else if (iFrequency <= 70000) {
        return (800);
    } else if (iFrequency <= 80000) {
        return (830);
    } else if (iFrequency <= 100000) {
        return (870);
    } else {
        return (900);
    }
}


/*****************************************************************************
 *
 *  BuckFrequencyFromVoltage - Returns an approximation of the frequency
 *                             of a Buck for a particular voltage
 *
 ******************************************************************************/
static uint32_t
BuckFrequencyFromVoltage(tBuckM3Target iVoltage)
{
    switch (iVoltage) {
        case eBuckM3Voltage600Mv: {
            return (20000);
            break;
        }
        case eBuckM3Voltage700Mv: {
            return (40000);
            break;
        }
        case eBuckM3Voltage800Mv: {
            return (80000);
            break;
        }
        // We should never get here
        default: {
            return (40000);
            break;
        }
    }
}


/*****************************************************************************
 *
 *  BuckAoTargetValid - Return true if the iAoTarget is valid otherwise
 *                      return false
 *
 ******************************************************************************/
static bool
BuckAoTargetValid(tBuckAoTarget   iAoTarget)
{
    if ((iAoTarget != eBuckAo600Mv) &&
        (iAoTarget != eBuckAo700Mv)) {
        return (false);
    }

    return (true);
}


/*****************************************************************************
 *
 *  BuckM3TargetValid - Return true if the iM3Target is valid otherwise
 *                      return false
 *
 ******************************************************************************/
static bool
BuckM3TargetValid(tBuckM3Target  iM3Target,
                  uint32_t       ui32M3Frequency)
{
    if ((iM3Target != eBuckM3Voltage600Mv) &&
        (iM3Target != eBuckM3Voltage700Mv) &&
        (iM3Target != eBuckM3Voltage800Mv) &&
        (iM3Target != eBuckM3FrequencyMode)) {
        return (false);
    }

    if (iM3Target == eBuckM3FrequencyMode) {
        if (ui32M3Frequency < ETA_BUCK_MIN_M3_FREQ_TARGET) {    // FIXME - need to determine correct limit here
            return (false);
        }

        if (ui32M3Frequency > ETA_BUCK_MAX_M3_FREQ_TARGET) {    // FIXME - need to determine correct limit here
            return (false);
        }
    }

    return (true);
}


/*****************************************************************************
 *
 *  BuckDspTargetValid - Return true if the iDspTarget is valid otherwise
 *                       return false
 *
 ******************************************************************************/
static bool
BuckDspTargetValid(tBuckDspTarget iDspTarget,
                   uint32_t       ui32DspFrequency)
{
    if ((iDspTarget != eBuckDspOff)          &&
        (iDspTarget != eBuckDspVoltage600Mv) &&
        (iDspTarget != eBuckDspVoltage700Mv) &&
        (iDspTarget != eBuckDspVoltage800Mv) &&
        (iDspTarget != eBuckDspFrequencyMode)) {
        return (false);
    }

    if (iDspTarget == eBuckDspFrequencyMode) {
        if (ui32DspFrequency < ETA_BUCK_MIN_M3_FREQ_TARGET) {
            return (false);
        }

        if (ui32DspFrequency > ETA_BUCK_MAX_M3_FREQ_TARGET) {
            return (false);
        }
    }

    return (true);
}


/*****************************************************************************
 *
 *  BuckMemTargetValid - Return true if the iMemTarget is valid otherwise
 *                       return false
 *
 ******************************************************************************/
static bool
BuckMemTargetValid(tBuckMemTarget  iMemTarget)
{
    if ((iMemTarget != eBuckMem900Mv) &&
        (iMemTarget != eBuckMem1100Mv)) {
        return (false);
    }

    return (true);
}


/*****************************************************************************
 *
 *  BuckTargetsValid - Return true if the targets passed to the function
 *                     result in a valid combination of Buck settings.
 *
 ******************************************************************************/
static bool
BuckTargetsValid(tBuckAoTarget  iAoTarget,
                 tBuckM3Target  iM3Target,
                 uint32_t       ui32M3Frequency,
                 tBuckDspTarget iDspTarget,
                 uint32_t       ui32DspFrequency,
                 tBuckMemTarget iMemTarget)
{
    //
    // Check the actual target values before checking the relationships
    //
    if ((!BuckAoTargetValid(iAoTarget))                  ||
        (!BuckM3TargetValid(iM3Target, ui32M3Frequency))    ||
        (!BuckDspTargetValid(iDspTarget, ui32DspFrequency)) ||
        (!BuckMemTargetValid(iMemTarget))                ) {
        return (false);
    }

    //
    // Now confirm if the AO/MEM relationship is valid.
    //
    if ((iAoTarget == eBuckAo600Mv) && (iMemTarget == eBuckMem1100Mv)) {
        return (false);
    }

    //
    // If the M3 is in Voltage mode, check the relationships
    //
    if (iM3Target != eBuckM3FrequencyMode) {
        // Check the AO and M3 relationship
        if ((uint32_t)iM3Target < (uint32_t)iAoTarget) {
            return (false);
        }
        // Check the M3 and MEM relationship
        if ((iM3Target >= eBuckM3Voltage800Mv) && (iMemTarget == eBuckMem900Mv)) {
            return (false);
        }
        if ((iM3Target <= eBuckM3Voltage600Mv) && (iMemTarget == eBuckMem1100Mv)) {
            return (false);
        }

    } else {
        // M3 is in frequency mode, so check frequency targets
        if ((ui32M3Frequency > ETA_BUCK_AO_0P6V_MEM_0P9V_MAX_FREQ) && ((iAoTarget == eBuckAo600Mv) || (iMemTarget == eBuckMem900Mv))) {
            return (false);
        }

        if ((ui32M3Frequency < ETA_BUCK_AO_0P7V_MEM_0P9V_MIN_FREQ) && ((iAoTarget == eBuckAo700Mv) || (iMemTarget == eBuckMem1100Mv))) {
            return (false);
        }
    }

    //
    // If the DSP is in Voltage mode, check the relationships
    //
    if (iDspTarget != eBuckDspOff) {
        if (iDspTarget != eBuckDspFrequencyMode) {
            // Check the AO and Dsp relationship
            if ((uint32_t)iDspTarget < (uint32_t)iAoTarget) {
                return (false);
            }
            // Check the Dsp and MEM relationship
            if ((iDspTarget >= eBuckDspVoltage800Mv) && (iMemTarget == eBuckMem900Mv)) {
                return (false);
            }
            if ((iDspTarget <= eBuckDspVoltage600Mv) && (iMemTarget == eBuckMem1100Mv)) {
                return (false);
            }

        } else {
            // DSP is in frequency mode, so check frequency targets
            if ((ui32DspFrequency > ETA_BUCK_AO_0P6V_MEM_0P9V_MAX_FREQ) && ((iAoTarget == eBuckAo600Mv) || (iMemTarget == eBuckMem900Mv))) {
                return (false);
            }

            if ((ui32DspFrequency < ETA_BUCK_AO_0P7V_MEM_0P9V_MIN_FREQ) && ((iAoTarget == eBuckAo700Mv) || (iMemTarget == eBuckMem1100Mv))) {
                return (false);
            }
        }
    }

    return (true);
}


/*****************************************************************************
 *
 *  BuckFlashInfoAoTrim - Return the Trim value stored in the Flash
 *  Info space for the AO Buck
 *
 ******************************************************************************/
static bool
BuckGetFlashInfoAoTrim(uint8_t *ui8AoTrim)
{
    uint32_t    ui32InfoData[4] = {
                    0x00000000,
                    0x00000000,
                    0x00000000,
                    0x00000000,
                };

    //
    // Read the data from the Flash
    //
    EtaCspFlashInfoRead(0x40, ui32InfoData);       // FIXME - should use a define for the address

    //
    // Confirm that the data is valid
    //
    if (ui32InfoData[0] == 0x99997777) {
        *ui8AoTrim = (uint8_t)((ui32InfoData[1] & BM_RTC_PMIC_AO_CSR_VREG_TRIM) >> BP_RTC_PMIC_AO_CSR_VREG_TRIM);
        return (true);
    } else {
        *ui8AoTrim = 0;
        return (false);
    }
}

/*****************************************************************************
 *
 *  BuckFlashInfoM3Trim - Return the Trim value stored in the Flash
 *  Info space for the M3 Buck
 *
 ******************************************************************************/
static bool
BuckGetFlashInfoM3Trim(uint8_t *ui8M3Trim)
{
    uint32_t    ui32InfoData[4] = {
                    0x00000000,
                    0x00000000,
                    0x00000000,
                    0x00000000,
                };

    //
    // Read the data from the Flash
    //
    EtaCspFlashInfoRead(0x40, ui32InfoData);       // FIXME - should use a define for the address

    //
    // Confirm that the data is valid
    //
    if (ui32InfoData[2] == 0x99997777) {
        *ui8M3Trim = (uint8_t)((ui32InfoData[3] & BM_RTC_PMIC_M3_CSR_VREG_TRIM) >> BP_RTC_PMIC_M3_CSR_VREG_TRIM);
        return (true);
    } else {
        *ui8M3Trim = 0;
        return (false);
    }
}

/*****************************************************************************
 *
 *  BuckFlashInfoDspTrim - Return the Trim value stored in the Flash
 *  Info space for the DSP Buck
 *
 ******************************************************************************/
static bool
BuckGetFlashInfoDspTrim(uint8_t *ui8DspTrim)
{
    uint32_t    ui32InfoData[4] = {
                    0x00000000,
                    0x00000000,
                    0x00000000,
                    0x00000000,
                };

    //
    // Read the data from the Flash
    //
    EtaCspFlashInfoRead(0x60, ui32InfoData);       // FIXME - should use a define for the address

    //
    // Confirm that the data is valid
    //
    if (ui32InfoData[0] == 0x99997777) {
        *ui8DspTrim = (uint8_t)((ui32InfoData[1] & BM_RTC_BUCK_DSP_CSR_VREG_TRIM) >> BP_RTC_BUCK_DSP_CSR_VREG_TRIM);
        return (true);
    } else {
        *ui8DspTrim = 0;
        return (false);
    }
}

/*****************************************************************************
 *
 *  BuckFlashInfoMemTrim - Return the Trim value stored in the Flash
 *  Info space for the MEM Buck
 *
 ******************************************************************************/
static bool
BuckGetFlashInfoMemTrim(uint8_t *ui8MemTrim)
{
    uint32_t    ui32InfoData[4] = {
                    0x00000000,
                    0x00000000,
                    0x00000000,
                    0x00000000,
                };

    //
    // Read the data from the Flash
    //
    EtaCspFlashInfoRead(0x50, ui32InfoData);       // FIXME - should use a define for the address

    //
    // Confirm that the data is valid
    //
    if (ui32InfoData[0] == 0x99997777) {
        *ui8MemTrim = (uint8_t)((ui32InfoData[1] & BM_RTC_PMIC_MEM_CSR_VREG_TRIM) >> BP_RTC_PMIC_MEM_CSR_VREG_TRIM);
        return (true);
    } else {
        *ui8MemTrim = 0;
        return (false);
    }
}


/*****************************************************************************
 *
 *  BuckGetFlashInfoFreqOffsets - Look in the flash to determine if the
 *  offsets for the frequency targets for M3 and DSP have been programmed.
 *  If so, extract those 2 values and populate the structure, then return 0.
 *  If not, return -1.
 *
 ******************************************************************************/
static bool
BuckGetFlashInfoFreqOffsets(int8_t *i8M3FreqOffset, int8_t *i8DspFreqOffset)
{
    uint32_t    ui32InfoData[4] = {
                    0x00000000,
                    0x00000000,
                    0x00000000,
                    0x00000000,
                };

    //
    // Read the data from the Flash
    //
    EtaCspFlashInfoRead((FLASH_INFO_BUCK_FREQ_OFFSET_KEY_ADDRESS & 0xfffffff0), ui32InfoData);


    //
    // Confirm that the data is valid
    //
    if (ui32InfoData[FLASH_INFO_BUCK_FREQ_OFFSET_KEY_INDEX] == 0x99997777) {
        *i8M3FreqOffset  = (int8_t)(ui32InfoData[FLASH_INFO_BUCK_FREQ_OFFSET_TRIM_INDEX] & 0x000000FF);
        *i8DspFreqOffset = (int8_t)((ui32InfoData[FLASH_INFO_BUCK_FREQ_OFFSET_TRIM_INDEX] & 0x00FF0000) >> 16);
        return (true);
    } else {
        *i8M3FreqOffset  = -2;
        *i8DspFreqOffset = -2;
        return (false);
    }
}


//
// FOR NOW WE DON'T NEED THIS FUNCTION
//
// /*****************************************************************************
//  *
//  *  BuckTuneFrequencyTarget - This function will iterate either the M3 or the DSP
//  *  Buck CSR2 freq_target setting to determine the most accurate setting
//  *  based on the desired frequency target. If it does not converge within
//  *  10 iterations it will exit and return false.
//  *
//  ******************************************************************************/
// static bool
// BuckTuneFrequencyTarget(uint8_t     ui8CsrFreqTarget,
//                         uint32_t    ui32FreqTarget,
//                         uint32_t    ui32CsrAddr,
//                         uint32_t    (*FreqGet)(),
//                         int8_t      *i8Offset  )
// {
//     int         i32Diff;
//     bool        bUp;
//     uint8_t     ui8LoopCount = 0;
//     uint32_t    ui32Tmp = 10000000;
//
//     //
//     // Initize the offset to 0
//     //
//     *i8Offset = 0;
//
//     //
//     // Determine how far off we are
//     //
//     i32Diff = FreqGet() - ui32FreqTarget;
//
//     //
//     // If diff is negative, convert and note direction
//     //
//     bUp     = false;
//     if (i32Diff < 0) {
//         i32Diff = 0 - i32Diff;
//         bUp     = true;
//     }
//
//
//     //
//     // Loop until we are within 700kHz
//     //
//     while (i32Diff > 1000000) {
//         //
//         // Increment/decrement offset
//         //
//         *i8Offset += (bUp) ? 1 : -1;
//
//
//         //
//         // Update the frequency target "live" in the Buck CSR
//         //
//         (*(volatile uint32_t*)(ui32CsrAddr)) = (uint32_t)(ui8CsrFreqTarget + *i8Offset);
//
//
//         //
//         // Add a delay here to allow Buck to settle which then allows oscillator
//         // counter to count. It updates at 8Hz, so it needs at least 250mS to
//         // ensure a good count after the buck settles.
//         //
//         EtaCspTimerDelayMs(500);
//
//         //
//         // Determine how far off we are again
//         //
//         i32Diff = FreqGet() - ui32FreqTarget;
//
//         //
//         // If diff is negative, convert and note direction
//         //
//         bUp     = false;
//         if (i32Diff < 0) {
//             i32Diff = 0 - i32Diff;
//             bUp     = true;
//         }
//
//
//         //
//         // If we have looped more than 10 times then we are not converging,
//         // so reset offset and exit
//         //
//         if (ui8LoopCount++ == 10) {
//             *i8Offset = 0;
//             return (false);
//         }
//     }
//
//     //
//     // We're done
//     //
//     return (true);
// }
//
//
// /*****************************************************************************
//  *
//  *  BuckCalculateFreqTargets - Start the M3 and DSP bucks in frequency mode,
//  *  target = 50MHz, and determine how far off they are. Tweak the offset
//  *  for the frequency target until we get as close as we can to the desired
//  *  50MHz frequency. Then turn the DSP buck off and M3 back to POR state.
//  *
//  ******************************************************************************/
// static bool
// BuckCalculateFreqTargets()
// {
//     uint8_t     ui8M3Offset;
//     uint8_t     ui8DspOffset;
//
//     uint32_t    ui32InfoPage[ETA_CSP_FLASH_PAGE_SIZE>>2];
//
//     tEtaStatus  eStatus;
//
//     //
//     // Put the M3 into frequency mode with 50MHz target
//     //   (should just work, no check for rc)
//     //
//     EtaCspBuckM3FrequencySet(50000, eBuckStableWait);
//
//     //
//     // Turn on the DSP Buck
//     //
//     EtaCspDspPowerUp();
//
//     //
//     // Put the DSP into frequency mode with 50MHz target
//     //   (should just work, no check for rc)
//     //
//     EtaCspBuckDspFrequencySet(50000, eBuckStableWait);
//
//     //
//     // Wait for 500mS for stability
//     //
//     EtaCspTimerDelayMs(500);
//
//     //
//     // Tune the M3 oscillator with the Buck
//     //
//     if (!BuckTuneFrequencyTarget(sregBuckShadow.sregM3Csr2.V,
//                                  50000000,
//                                  REG_RTC_PMIC_M3_CSR2_ADDR,
//                                  &EtaCspSocCtrlM3FrequencyGet,
//                                  &(sregBuckShadow.i8M3FreqOffset)) ) {
//         return (false);
//     }
//
//
//     //
//     // Tune the DSP oscillator with the Buck
//     //
//     if (!BuckTuneFrequencyTarget(sregBuckShadow.sregDspCsr2.V,
//                                  50000000,
//                                  REG_RTC_BUCK_DSP_CSR2_ADDR,
//                                  &EtaCspSocCtrlDspFrequencyGet,
//                                  &(sregBuckShadow.i8DspFreqOffset)) ) {
//         return (false);
//     }
//
//
//     //
//     // Turn off the DSP Buck
//     //
//     EtaCspDspPowerDown();
//     sregBuckShadow.sregDspCsr2.V = 0;     // This is to appear like it's the first time through.
//
//     //
//     // Update the flash info space with the offset values so that I
//     // don't have to go through this code again.
//     //
//     EtaCspFlashInfoGet(ETA_CSP_FLASH_BASE, (uint8_t *)ui32InfoPage, ETA_CSP_FLASH_PAGE_SIZE);
//
//     ui32InfoPage[FLASH_INFO_BUCK_FREQ_OFFSET_KEY_ADDRESS >> 2]   = 0x99997777;
//     ui32InfoPage[FLASH_INFO_BUCK_FREQ_OFFSET_TRIM_ADDRESS >> 2] = (sregBuckShadow.i8DspFreqOffset << 16) |
//                                                                   (sregBuckShadow.i8M3FreqOffset & 0xFFFF);
//
//     IFDBG etaPrintf ("Post Tune Values in info space: KEY = %x, VALUE = %x\r\n\n", ui32InfoPage[FLASH_INFO_BUCK_FREQ_OFFSET_KEY_ADDRESS >> 2],
//                                                                                    ui32InfoPage[FLASH_INFO_BUCK_FREQ_OFFSET_TRIM_ADDRESS >> 2]);
//
//     ETA_CSP_FLASH_INFO_ERASE();
//     eStatus = ETA_CSP_FLASH_INFO_PROGRAM(ETA_CSP_FLASH_BASE,
//                                          (uint8_t *)ui32InfoPage,
//                                          ETA_CSP_FLASH_PAGE_SIZE);
//
//     //
//     // All done
//     //
//     return (true);
// }



/*****************************************************************************
 *
 *  BuckGetAoVoltage - Return the current voltage setting
 *  of the AO PMIC
 *
 ******************************************************************************/
static tBuckAoTarget
BuckGetAoVoltage(void)
{
    uint8_t     ui8OpMode;

    //
    // Only read from the CSR if we have not already initialized the structure
    //
    if (sregBuckShadow.sregAoCsr.V != 0) {
        ui8OpMode = sregBuckShadow.sregAoCsr.BF.OP_MODE;
    } else {
        ui8OpMode = REG_RTC_PMIC_AO_CSR.BF.OP_MODE;
    }

    //
    // Return value based on OP_MODE
    //
    switch (ui8OpMode)
    {
        case 0:
        {
            return (eBuckAo600Mv);
        }
        case 1:
        {
            return (eBuckAo700Mv);
        }
        default:
        {
            return (eBuckAoInvalid);
        }
    }
}


/*****************************************************************************
 *
 *  BuckGetM3Voltage - Return the current voltage setting of the
 *  M3 PMIC
 *
 ******************************************************************************/
static tBuckM3Target
BuckGetM3Voltage(void)
{
    uint8_t     ui8OpMode;

    //
    // Only read from the CSR if we have not already initialized the structure
    //
    if (sregBuckShadow.sregM3Csr.V != 0) {
        ui8OpMode = sregBuckShadow.sregM3Csr.BF.OP_MODE;
    } else {
        ui8OpMode = REG_RTC_PMIC_M3_CSR.BF.OP_MODE;
    }

    //
    // Return value based on OP_MODE
    //
    switch (ui8OpMode)
    {
        case 0:
        {
            return (eBuckM3Voltage600Mv);
        }
        case 1:
        {
            return (eBuckM3Voltage700Mv);
        }
        case 2:
        {
            return (eBuckM3Voltage800Mv);
        }
        case 3:
        {
            return (eBuckM3Invalid);
        }
        default:
        {
            // If the op_mode field is different from the above options, we are in
            //      frequency mode.
            return (eBuckM3FrequencyMode);
        }
    }
}


/*****************************************************************************
 *
 *  BuckGetM3Frequency - Return the current frequency setting of the
 *  M3 PMIC
 *
 ******************************************************************************/
static uint32_t
BuckGetM3Frequency(void)
{
    return (sregBuckShadow.ui32M3Frequency);
}


/*****************************************************************************
 *
 *  BuckGetDspVoltage - Return the current voltage setting
 *  of the DSP PMIC
 *
 ******************************************************************************/
static tBuckDspTarget
BuckGetDspVoltage(void)
{
    uint8_t         ui8OpMode;

    //
    // Check whether the DSP Buck is powered on. If not,
    // just exit with an 'off' indication.
    //
    if (EtaCspDspPowerStateGet() == eDspPowerOff) {
        return (eBuckDspOff);
    }

    //
    // Only read from the CSR if we have not already initialized
    // the structure
    //
    if (sregBuckShadow.sregDspCsr.V != 0) {
        ui8OpMode = sregBuckShadow.sregDspCsr.BF.OP_MODE;
    } else {
        ui8OpMode = REG_RTC_BUCK_DSP_CSR.BF.OP_MODE;
    }

    //
    // Return value based on OP_MODE
    //
    switch (ui8OpMode)
    {
        case 0:
        {
            return (eBuckDspVoltage600Mv);
        }
        case 1:
        {
            return (eBuckDspVoltage700Mv);
        }
        case 2:
        {
            return (eBuckDspVoltage800Mv);
        }
        case 3:
        {
            return (eBuckDspInvalid);
        }
        default:
        {
            // If the op_mode field is different from the above options,
            // we are in frequency mode.
            return (eBuckDspFrequencyMode);
        }
    }
}


/*****************************************************************************
 *
 *  BuckGetDspFrequency - Return the current frequency setting of the
 *  DSP PMIC
 *
 ******************************************************************************/
static uint32_t
BuckGetDspFrequency(void)
{
    return (sregBuckShadow.ui32DspFrequency);
}


/*****************************************************************************
 *
 *  BuckGetMemVoltage - Return the current voltage setting
 *  of the MEM PMIC
 *
 ******************************************************************************/
static tBuckMemTarget
BuckGetMemVoltage(void)
{
    uint8_t     ui8OpMode;

    //
    // Only read from the CSR if we have not already initialized the structure
    //
    if (sregBuckShadow.sregMemCsr.V != 0) {
        ui8OpMode = sregBuckShadow.sregMemCsr.BF.OP_MODE;
    } else {
        ui8OpMode = REG_RTC_PMIC_MEM_CSR.BF.OP_MODE;
    }

    //
    // Return value based on OP_MODE
    //
    switch (ui8OpMode)
    {
        case 2:
        {
            return (eBuckMem900Mv);
        }
        case 3:
        {
            return (eBuckMem1100Mv);
        }
        default:
        {
            return (eBuckMemInvalid);
        }
    }
}


/***************************************************************************//**
 *
 *  EtaCspBuckAoVoltageSet - Set the target voltage for the AO
 *  Buck
 *
 *  @param iAoTarget AO Buck Voltage Target
 *  @param iWaitForStable Indicates whether to wait for PMIC
 *                        stability or higher level code will
 *                        handle this
 *  @return Return success/fail status
 *
 ******************************************************************************/
tEtaStatus
EtaCspBuckAoVoltageSet(tBuckAoTarget    iAoTarget,
                       tBuckWait4Stable iWaitForStable)
{
    reg_rtc_pmic_ao_csr_t   sregBuckWriteVal;


    //
    // Check to ensure that the target is legal (ie: AO cannot be set to 600mV
    //      if MEM is at 1.2V, AO cannot be 0.7V if M3 or DSP is less)
    //
    if (!sregBuckShadow.bTargetsQualified) {
        if (!BuckTargetsValid(iAoTarget,
                              BuckGetM3Voltage(),
                              BuckGetM3Frequency(),
                              BuckGetDspVoltage(),
                              BuckGetDspFrequency(),
                              BuckGetMemVoltage() ) ) {
            return (eEtaBuckInvalidAoVoltage);
        }
    }

    //
    // Initialize all fields to 0's
    //
    sregBuckWriteVal.V = 0x0;

    //
    // Set the Buck to use the external reference clock in all cases
    //
    sregBuckWriteVal.BF.SEL_REF_EXT = 1;

    //
    // The VREG TRIM will be the same for all voltage settings
    //
    sregBuckWriteVal.BF.VREG_TRIM = sregBuckShadow.ui8AoVregTrim;

    //
    // Now set the other AO PMIC CSR values:
    //
    switch(iAoTarget)
    {
        case eBuckAo600Mv:
        {
            sregBuckWriteVal.BF.OP_MODE     = 0;
            sregBuckWriteVal.BF.SEL_OSC     = 1;
            sregBuckWriteVal.BF.SEL_OSC_DIV = 0;
            sregBuckWriteVal.BF.SEL_PD      = 72;
            break;
        }

        case eBuckAo700Mv:
        {
            sregBuckWriteVal.BF.OP_MODE     = 1;
            sregBuckWriteVal.BF.SEL_OSC     = 0;
            sregBuckWriteVal.BF.SEL_OSC_DIV = 4;
            sregBuckWriteVal.BF.SEL_PD      = 66;

            break;
        }

    default:
        {
            return (eEtaBuckInvalidAoVoltage);
        }
    }

    //
    // Write the new value first allowing time for value to stabilize before
    // strobing the write signal.
    //
    REG_RTC_PMIC_AO_CSR.V =  sregBuckWriteVal.V;

    //
    // Finally, set the write bit.
    //
    sregBuckWriteVal.BF.WRITE = 1;
    REG_RTC_PMIC_AO_CSR.V =  sregBuckWriteVal.V;

    //
    // Capture that we have set the AO Buck CSR
    //
    sregBuckShadow.bAoValid     = true;
    sregBuckShadow.sregAoCsr.V  = sregBuckWriteVal.V;

    //
    // Wait for the write to complete.
    //
    EtaCspRtcFenceFast();

    //
    // Wait for AO power domain to be stable
    //
    if (iWaitForStable == eBuckStableWait) {
        // There is only a single counter to check for stability of a Buck within the
        //  RTC register space. This counter's stability is not reflected in the Buck AO
        //  CSR, so instead use the bit as reflected in the Buck M3 CSR. The counter
        //  is restarted when ANY Buck CSR register is written.
        while(!EtaCspBuckStable());
    }

    //
    // Retune the HFO after waiting for the PMIC to be stable.
    //      Caller of this function should handle this after all Bucks are set
    //
    // EtaCspHfoTuneDefaultUpdate();

    //
    // Done
    //
    return (eEtaSuccess);
}


/***************************************************************************//**
 *
 *  EtaCspBuckM3VoltageSet - Set the target voltage for the M3
 *  Buck
 *
 *  @param iM3Target M3 Buck Voltage Target
 *  @param iWaitForStable Indicates whether to wait for PMIC
 *                        stability or higher level code will
 *                        handle this
 *  @return Return success/fail status
 *
 ******************************************************************************/
tEtaStatus
EtaCspBuckM3VoltageSet(tBuckM3Target        iM3Target,
                       tBuckWait4Stable     iWaitForStable)
{
    reg_rtc_pmic_m3_csr_t   sregBuckWriteVal;

    uint32_t ui8NewFlashWS;
    uint32_t ui8CurrFlashWS;


    //
    // Check to ensure that the target is legal
    //  * M3 cannot be set to 0.6V if AO is > 600mV
    //  * M3 cannot be set to 0.9V if MEM is at 900mV
    //
    if (!sregBuckShadow.bTargetsQualified) {
        if (!BuckTargetsValid(BuckGetAoVoltage(),
                              iM3Target,
                              0,
                              BuckGetDspVoltage(),
                              BuckGetDspFrequency(),
                              BuckGetMemVoltage() ) ) {
        return (eEtaBuckInvalidM3Voltage);
        }
    }

    //
    // Initialize all fields to 0's
    //
    sregBuckWriteVal.V  = 0x0;

    //
    // Extract the current # flash wait states.
    //
    ui8CurrFlashWS = REG_SOCCTRL_FLASH_CFG_STATUS.BF.NUM_WS;

    //
    // Set the Buck to use the external reference clock in all cases
    //
    sregBuckWriteVal.BF.SEL_REF_EXT = 1;

    //
    // The VREG TRIM will be the same for all voltage settings (unless
    // targeting 800mV, we'll deal with that below)
    //
    sregBuckWriteVal.BF.VREG_TRIM = sregBuckShadow.ui8M3VregTrim;

    //
    // The Buck needs to monitor the voltage inside of the pad switch
    // when we are active and outside of the pad switch if we are going
    // to sleep/powerdown state. The parameter sregBuckShadow.bEnterSleep
    // indicates the goal of the function, so set SEL_VREG based on this.
    //
    sregBuckWriteVal.BF.SEL_VREG = sregBuckShadow.bEnterSleep ? 0 : 1;

    //
    // Need to manage the transition from frequency to voltage mode here.
    // If we were in frequency mode and are transitioning to voltage mode,
    // leave the FMODE_RST_N signal asserted here until after the new
    // load to the CSR is complete. Then follow up with cleaning that bit.
    //
    sregBuckWriteVal.BF.FMODE_RST_L_N = sregBuckShadow.bM3FrequencyMode;    // This bool hasn't been updated yet so we can use it here

    //
    // Create the value to write to the M3 Buck CSR
    //
    switch(iM3Target)
    {
        case eBuckM3Voltage600Mv:
        {
            sregBuckWriteVal.BF.OP_MODE     = 0;
            sregBuckWriteVal.BF.SEL_OSC     = 0;
            sregBuckWriteVal.BF.SEL_OSC_DIV = 3;
            sregBuckWriteVal.BF.SEL_PD      = 72;

            ui8NewFlashWS = 1;                                                 // FIXME - need to verify
            break;
        }
        case eBuckM3Voltage700Mv:
        {
            sregBuckWriteVal.BF.OP_MODE     = 1;
            sregBuckWriteVal.BF.SEL_OSC     = 0;
            sregBuckWriteVal.BF.SEL_OSC_DIV = 2;
            sregBuckWriteVal.BF.SEL_PD      = 66;

            ui8NewFlashWS = 2;                                                 // FIXME - need to verify
            break;
        }
        case eBuckM3Voltage800Mv:
        {
            sregBuckWriteVal.BF.OP_MODE     = 2;
            sregBuckWriteVal.BF.SEL_OSC     = 0;
            sregBuckWriteVal.BF.SEL_OSC_DIV = 1;
            sregBuckWriteVal.BF.SEL_PD      = 55;

            // Need to target 800mV which is op_mode = 2, vreg_trim = 5
            switch (sregBuckShadow.ui8M3VregTrim) {
                case 0: {
                    sregBuckWriteVal.BF.VREG_TRIM   = 5;
                    break;
                }
                case 1: {
                    sregBuckWriteVal.BF.VREG_TRIM   = 6;
                    break;
                }
                case 2: {
                    sregBuckWriteVal.BF.VREG_TRIM   = 7;
                    break;
                }
                case 3: {
                    sregBuckWriteVal.BF.VREG_TRIM   = 0;
                    break;
                }
                default: {
                    sregBuckWriteVal.BF.VREG_TRIM   = 4;
                    break;
                }
            }

            ui8NewFlashWS = 4;                                                 // FIXME - need to verify
            break;
        }
        default:
        {
            return (eEtaBuckInvalidM3Voltage);
        }
    }

    //
    // If we are increasing the # of wait states, do that first.
    //
    if(ui8NewFlashWS > ui8CurrFlashWS)
    {
        EtaCspFlashBrWsSet(ui8NewFlashWS);
    }

    //
    // Write the new value first allowing time for value to stabilize before
    // strobing the write signal.
    //
    REG_RTC_PMIC_M3_CSR.V = sregBuckWriteVal.V;

    //
    // Finally, set the write bit.
    //
    sregBuckWriteVal.BF.WRITE = 1;
    REG_RTC_PMIC_M3_CSR.V = sregBuckWriteVal.V;

    //
    // We are setting voltage mode, so switch to that. This bit goes active
    // immediately when written to the CSR, so write at the same time as
    // the load strome (WRITE bit). Only do this write if we were in frequency
    // mode before entering this function.
    //
    if (sregBuckShadow.bM3FrequencyMode) {
        sregBuckWriteVal.BF.FMODE_RST_L_N   = 0;
        sregBuckWriteVal.BF.WRITE           = 0;    // no need to strobe WRITE again
        REG_RTC_PMIC_M3_CSR.V               = sregBuckWriteVal.V;
    }

    //
    // As we are in Voltage mode, clear the frequency target value
    //
    REG_RTC_PMIC_M3_CSR2.V = 0x0;

    //
    // Capture that we have set the M3 Buck CSR
    //
    sregBuckShadow.bM3Valid         = true;
    sregBuckShadow.bM3FrequencyMode = false;
    sregBuckShadow.ui32M3Frequency  = ETA_BUCK_AO_0P7V_MEM_0P9V_MIN_FREQ;       // Initialize with an arbitrary mid-point value
    sregBuckShadow.sregM3Csr.V      = sregBuckWriteVal.V;
    sregBuckShadow.sregM3Csr2.V     = 0x0;

    //
    // Wait for the write to complete.
    //
    EtaCspRtcFenceFast();

    //
    // Wait for M3 power domain to be stable
    //
    if (iWaitForStable == eBuckStableWait) {
        while(!EtaCspBuckStable());
    }

    //
    // If we are decreasing the # of wait states, do it last.
    //
    if(ui8NewFlashWS < ui8CurrFlashWS)
    {
        EtaCspFlashBrWsSet(ui8NewFlashWS);
    }

    //
    // Done
    //
    return (eEtaSuccess);
}


/***************************************************************************//**
 *
 *  EtaCspBuckDspVoltageSet - If requested, turn on the DSP buck
 *  and set the target voltage. ALternatively turn it off if
 *  requested.
 *
 *  @param iDspTarget DSP Buck Voltage Target
 *  @param iWaitForStable Indicates whether to wait for PMIC
 *                        stability or higher level code will
 *                        handle this
 *  @return Return success/fail status
 *
 ******************************************************************************/
tEtaStatus
EtaCspBuckDspVoltageSet(tBuckDspTarget      iDspTarget,
                        tBuckWait4Stable    iWaitForStable)
{
    reg_rtc_buck_dsp_csr_t   sregBuckWriteVal;


    //
    // If this function was called indicating DSP off, return error
    //
    if ((iDspTarget == eBuckDspOff) || (EtaCspDspPowerStateGet() == eDspPowerOff)) {
        return (eEtaBuckDspPoweredOff);
    }

    //
    // Check to ensure that the target is legal
    //  * DSP cannot be set to 0.6V if AO is > 600mV
    //  * DSP cannot be set to 0.9V if MEM is at 900mV
    //
    if (!sregBuckShadow.bTargetsQualified) {
        if (!BuckTargetsValid(BuckGetAoVoltage(),
                              BuckGetM3Voltage(),
                              BuckGetM3Frequency(),
                              iDspTarget,
                              0,
                              BuckGetMemVoltage() ) ) {
            return (eEtaBuckInvalidDspVoltage);
        }
    }

    //
    // Initialize all fields to 0's
    //
    sregBuckWriteVal.V  = 0x0;

    //
    // Set the Buck to use the external reference clock in all cases
    //
    sregBuckWriteVal.BF.SEL_REF_EXT = 1;

    //
    // The VREG TRIM will be the same for all voltage settings
    //
    sregBuckWriteVal.BF.VREG_TRIM = sregBuckShadow.ui8DspVregTrim;

    //
    // The Buck needs to monitor the voltage inside of the pad switch
    // when we are active and outside of the pad switch if we are going
    // to sleep/powerdown state. The parameter sregBuckShadow.bEnterSleep
    // indicates the goal of the function, so set SEL_VREG based on this.
    //
    sregBuckWriteVal.BF.SEL_VREG = sregBuckShadow.bEnterSleep ? 0 : 1;

    //
    // Need to manage the transition from frequency to voltage mode here.
    // If we were in frequency mode and are transitioning to voltage mode,
    // leave the FMODE_RST_N signal asserted here until after the new
    // load to the CSR is complete. Then follow up with cleaning that bit.
    //
    sregBuckWriteVal.BF.FMODE_RST_L_N = sregBuckShadow.bDspFrequencyMode;

    //
    // Create the value to write to the DSP Buck CSR
    //
    switch(iDspTarget)
    {
        case eBuckDspVoltage600Mv:
        {
            sregBuckWriteVal.BF.OP_MODE     = 0;
            sregBuckWriteVal.BF.SEL_OSC     = 0;
            sregBuckWriteVal.BF.SEL_OSC_DIV = 3;
            sregBuckWriteVal.BF.SEL_PD      = 72;
            break;
        }
        case eBuckDspVoltage700Mv:
        {
            sregBuckWriteVal.BF.OP_MODE     = 1;
            sregBuckWriteVal.BF.SEL_OSC     = 0;
            sregBuckWriteVal.BF.SEL_OSC_DIV = 2;
            sregBuckWriteVal.BF.SEL_PD      = 66;
            break;
        }
        case eBuckDspVoltage800Mv:
        {
            sregBuckWriteVal.BF.OP_MODE     = 2;
            sregBuckWriteVal.BF.SEL_OSC     = 0;
            sregBuckWriteVal.BF.SEL_OSC_DIV = 1;
            sregBuckWriteVal.BF.SEL_PD      = 55;

            // Need to target 800mV which is op_mode = 2, vreg_trim = 5
            switch (sregBuckShadow.ui8DspVregTrim) {
                case 0: {
                    sregBuckWriteVal.BF.VREG_TRIM   = 5;
                    break;
                }
                case 1: {
                    sregBuckWriteVal.BF.VREG_TRIM   = 6;
                    break;
                }
                case 2: {
                    sregBuckWriteVal.BF.VREG_TRIM   = 7;
                    break;
                }
                case 3: {
                    sregBuckWriteVal.BF.VREG_TRIM   = 0;
                    break;
                }
                default: {
                    sregBuckWriteVal.BF.VREG_TRIM   = 4;
                    break;
                }
            }

            break;
        }
        default:
        {
            return (eEtaBuckInvalidDspVoltage);
        }
    }

    //
    // Write the new value first allowing time for value to stabilize before
    // strobing the write signal.
    //
    REG_RTC_BUCK_DSP_CSR.V = sregBuckWriteVal.V;

    //
    // Finally, set the write bit.
    //
    sregBuckWriteVal.BF.WRITE = 1;
    REG_RTC_BUCK_DSP_CSR.V = sregBuckWriteVal.V;

    //
    // We are setting voltage mode, so switch to that. This bit goes active
    // immediately when written to the CSR, so write at the same time as
    // the load strome (WRITE bit). Only do this write if we were in frequency
    // mode before entering this function.
    //
    if (sregBuckShadow.bDspFrequencyMode) {
        sregBuckWriteVal.BF.FMODE_RST_L_N   = 0;
        sregBuckWriteVal.BF.WRITE           = 0;    // no need to strobe WRITE again
        REG_RTC_BUCK_DSP_CSR.V              = sregBuckWriteVal.V;
    }

    //
    // As we are in Voltage mode, clear the frequency target value
    //
    REG_RTC_BUCK_DSP_CSR2.V = 0x0;

    //
    // Capture that we have set the DSP Buck CSR
    //
    sregBuckShadow.bDspValid         = true;
    sregBuckShadow.bDspFrequencyMode = false;
    sregBuckShadow.ui32DspFrequency  = ETA_BUCK_AO_0P7V_MEM_0P9V_MIN_FREQ;       // Initialize with an arbitrary mid-point value
    sregBuckShadow.sregDspCsr.V      = sregBuckWriteVal.V;
    sregBuckShadow.sregDspCsr2.V     = 0x0;

    //
    // Wait for the write to complete.
    //
    EtaCspRtcFenceFast();

    //
    // Wait for DSP power domain to be stable
    //
    if (iWaitForStable == eBuckStableWait) {
        while(!EtaCspBuckStable());
    }

    //
    // Done
    //
    return (eEtaSuccess);
}


/***************************************************************************//**
 *
 *  EtaCspBuckMemVoltageSet - Set the target voltage for the MEM
 *  Buck
 *
 *  @param iMemTarget MEM Buck Voltage Target
 *  @param iWaitForStable Indicates whether to wait for PMIC
 *                        stability or higher level code will
 *                        handle this
 *  @return Return success/fail status
 *
 ******************************************************************************/
tEtaStatus
EtaCspBuckMemVoltageSet(tBuckMemTarget      iMemTarget,
                        tBuckWait4Stable    iWaitForStable)
{
    reg_rtc_pmic_mem_csr_t   sregBuckWriteVal;


    //
    // Check to ensure that the target is legal (ie: MEM cannot be set to 1.2V
    //      if AO is at 600mV)
    //
    if (!sregBuckShadow.bTargetsQualified) {
        if (!BuckTargetsValid(BuckGetAoVoltage(),
                              BuckGetM3Voltage(),
                              BuckGetM3Frequency(),
                              BuckGetDspVoltage(),
                              BuckGetDspFrequency(),
                              iMemTarget ) ) {
        return (eEtaBuckInvalidMemVoltage);
        }
    }

    //
    // Initialize all fields to 0's
    //
    sregBuckWriteVal.V  = 0x0;

    //
    // Set the Buck to use the external reference clock in all cases
    //
    sregBuckWriteVal.BF.SEL_REF_EXT = 1;

    //
    // Create the value to write to the MEM Buck CSR
    //
    switch(iMemTarget)
    {
        case eBuckMem900Mv:
        {
            sregBuckWriteVal.BF.OP_MODE     = 2;
            sregBuckWriteVal.BF.SEL_OSC     = 0;
            sregBuckWriteVal.BF.SEL_OSC_DIV = 2;
            sregBuckWriteVal.BF.SEL_PD      = 60;
            sregBuckWriteVal.BF.VREG_TRIM   = sregBuckShadow.ui8MemVregTrim;
            break;
        }

        case eBuckMem1100Mv:
        {
            sregBuckWriteVal.BF.OP_MODE     = 3;
            sregBuckWriteVal.BF.SEL_OSC     = 0;
            sregBuckWriteVal.BF.SEL_OSC_DIV = 1;
            sregBuckWriteVal.BF.SEL_PD      = 48;

            switch (sregBuckShadow.ui8MemVregTrim) {
                case 0: {
                    sregBuckWriteVal.BF.VREG_TRIM   = 5;
                    break;
                }
                case 1: {
                    sregBuckWriteVal.BF.VREG_TRIM   = 6;
                    break;
                }
                case 2: {
                    sregBuckWriteVal.BF.VREG_TRIM   = 7;
                    break;
                }
                case 3: {
                    sregBuckWriteVal.BF.VREG_TRIM   = 0;
                    break;
                }
                default: {
                    sregBuckWriteVal.BF.VREG_TRIM   = 4;
                    break;
                }
            }
            break;
        }

        default:
        {
            return (eEtaBuckInvalidMemVoltage);
        }
    }

    //
    // Write the new value first allowing time for value to stabilize before
    // strobing the write signal.
    //
    REG_RTC_PMIC_MEM_CSR.V      = sregBuckWriteVal.V;

    //
    // Finally, set the write bit.
    //
    sregBuckWriteVal.BF.WRITE   = 1;
    REG_RTC_PMIC_MEM_CSR.V      = sregBuckWriteVal.V;

    //
    // Capture that we have set the MEM Buck CSR
    //
    sregBuckShadow.bMemValid        = true;
    sregBuckShadow.sregMemCsr.V     = sregBuckWriteVal.V;

    //
    // Wait for the write to complete.
    //
    EtaCspRtcFenceFast();

    //
    // Wait for MEM power domain to be stable
    //
    if (iWaitForStable == eBuckStableWait) {
        while(!EtaCspBuckStable());
    }

    //
    // Done
    //
    return (eEtaSuccess);
}


/***************************************************************************//**
 *
 *  EtaCspBuckM3FrequencySet - Set the target frequency for the
 *  M3 Buck
 *
 *  @param ui32M3Frequency M3 Buck Frequency Target
 *  @param iWaitForStable Indicates whether to wait for PMIC
 *                        stability or higher level code will
 *                        handle this
 *  @return Return success/fail status
 *
 ******************************************************************************/
tEtaStatus
EtaCspBuckM3FrequencySet(uint32_t           ui32M3Frequency,
                         tBuckWait4Stable   iWaitForStable)
{
    reg_rtc_pmic_m3_csr_t   sregBuckWriteVal;
    reg_rtc_pmic_m3_csr2_t  sregBuckWriteVal2;
    uint8_t                 ui8NewFlashWS;
    uint8_t                 ui8CurrFlashWS;
    uint8_t                 ui8OscDiv;


    //
    // Check to ensure that the target is legal
    //  * M3 cannot be set to low frequencies if AO is > 600mV
    //  * M3 cannot be set to high frequencies if MEM is at 900mV
    //
    if (!sregBuckShadow.bTargetsQualified) {
        if (!BuckTargetsValid(BuckGetAoVoltage(),
                              eBuckM3FrequencyMode,
                              ui32M3Frequency,
                              BuckGetDspVoltage(),
                              BuckGetDspFrequency(),
                              BuckGetMemVoltage() ) ) {
        return (eEtaBuckInvalidM3Frequency);
        }
    }

    //
    // Initialize all fields to 0's
    //
    sregBuckWriteVal.V  = 0x0;
    sregBuckWriteVal2.V = 0x0;

    //
    // Extract the current # flash wait states.
    //
    ui8CurrFlashWS = REG_SOCCTRL_FLASH_CFG_STATUS.BF.NUM_WS;

    //
    // Set the Buck to use the external reference clock in all cases
    //
    sregBuckWriteVal.BF.SEL_REF_EXT = 1;

    //
    // Extract the current # flash wait states.
    //
    ui8CurrFlashWS = REG_SOCCTRL_FLASH_CFG_STATUS.BF.NUM_WS;

    //
    // Set the Buck to use the external reference clock in all cases
    //
    sregBuckWriteVal.BF.SEL_REF_EXT = 1;

    //
    // We set the trim in frequency mode also as it controls the limits the
    // voltage can range to when acquiring frequency. We set this to -1 (7)
    // to give us some voltage margin in frequency
    //
    sregBuckWriteVal.BF.VREG_TRIM = 7;  // FIXME - change to use offset from trim in flash info space when ready

    //
    // The Buck needs to monitor the voltage inside of the pad switch
    // when we are active and outside of the pad switch if we are going
    // to sleep/powerdown state. For frequency mode the switch must
    // be on, so always monitor inside of the switch.
    //
    sregBuckWriteVal.BF.SEL_VREG = 1;

    //
    // We are setting frequency mode, so switch to that. This bit goes active
    // immediately when written to the CSR
    //
    sregBuckWriteVal.BF.FMODE_RST_L_N = 1;

    //
    // In frequency mode we want to enable the min and max limits for the Buck
    // to be in safe modes.
    //
    sregBuckWriteVal.BF.OP_MODE = 4;

    //
    // Now create the frequency-specific variables
    //
    if (ui32M3Frequency <= 32000) {
        sregBuckWriteVal.BF.SEL_PD      = 64;
        ui8OscDiv                       = 2;                // FIXME - need to vary the trim based on the ATE-set trim written in flash
        ui8NewFlashWS                   = 1;                // FIXME - need to verify
    } else if (ui32M3Frequency <= 40000) {
        sregBuckWriteVal.BF.SEL_PD      = 60;
        ui8OscDiv                       = 2;                // FIXME - need to vary the trim based on the ATE-set trim written in flash
        ui8NewFlashWS                   = 1;                // FIXME - need to verify
    } else if (ui32M3Frequency <= 48000) {
        sregBuckWriteVal.BF.SEL_PD      = 58;
        ui8OscDiv                       = 1;                // FIXME - need to vary the trim based on the ATE-set trim written in flash
        ui8NewFlashWS                   = 2;                // FIXME - need to verify
    } else if (ui32M3Frequency <= 64000) {
        sregBuckWriteVal.BF.SEL_PD      = 54;
        ui8OscDiv                       = 1;                // FIXME - need to vary the trim based on the ATE-set trim written in flash
        ui8NewFlashWS                   = 2;                // FIXME - need to verify
    } else if (ui32M3Frequency <= 80000) {
        sregBuckWriteVal.BF.SEL_PD      = 50;
        ui8OscDiv                       = 1;                // FIXME - need to vary the trim based on the ATE-set trim written in flash
        ui8NewFlashWS                   = 3;                // FIXME - need to verify
    } else if (ui32M3Frequency <= 96000) {
        sregBuckWriteVal.BF.SEL_PD      = 48;
        ui8OscDiv                       = 1;                // FIXME - need to vary the trim based on the ATE-set trim written in flash
        ui8NewFlashWS                   = 3;                // FIXME - need to verify
    } else {
        sregBuckWriteVal.BF.SEL_PD      = 44;
        ui8OscDiv                       = 1;                // FIXME - need to vary the trim based on the ATE-set trim written in flash
        ui8NewFlashWS                   = 4;                // FIXME - need to verify
    }

    //
    // Capture the oscillator base clock settings and divide
    // value into the structure
    //
    sregBuckWriteVal.BF.SEL_OSC     = 0;                    // For all settings of frequency we will use the HFO/VHFO as the base
    sregBuckWriteVal.BF.SEL_OSC_DIV = ui8OscDiv;            // FIXME - need to vary the trim based on the ATE-set trim written in flash

    //
    // If we are increasing the # of wait states, do it first.
    //
    if(ui8NewFlashWS > ui8CurrFlashWS)
    {
        EtaCspFlashBrWsSet(ui8NewFlashWS);
    }

    //
    // Determine the frequency target value
    //
    // ui32FreqTarget = (m3 osc frequency / ref clock frequency) / 2
    //
    // Optimize that to:
    // ui32FreqTarget = (m3 osc freq / (ref clock base / ref clock div) / 2
    // ui32FreqTarget = (m3 osc freq * ref clock div) / (ref clock base * 2)
    // ui32FreqTarget = (m3 osc freq * ref clock div) / (4000 or 4096 or 128)
    //
    if (sregBuckShadow.sregHfClockSource == eBuckHfClockSourceHfo) {
        sregBuckWriteVal2.V = (ui32M3Frequency * (1 << ui8OscDiv)) / 4000;
    } else {
        sregBuckWriteVal2.V = (ui32M3Frequency * (1 << ui8OscDiv)) / 4096;
    }

    //
    // Adjust the calculated value using the offset
    //
    sregBuckWriteVal2.V += sregBuckShadow.i8M3FreqOffset;   // FIXME - this may need to become an equation relative to refclk

    //
    // Write the frequency target value.
    //
    REG_RTC_PMIC_M3_CSR2.V = sregBuckWriteVal2.V;

    //
    // Write the new value first allowing time for value to stabilize before
    // strobing the write signal.
    //
    REG_RTC_PMIC_M3_CSR.V = sregBuckWriteVal.V;

    //
    // Finally, set the write bit.
    //
    sregBuckWriteVal.BF.WRITE   = 1;
    REG_RTC_PMIC_M3_CSR.V       = sregBuckWriteVal.V;

    //
    // Capture that we have set the M3 Buck CSRs
    //
    sregBuckShadow.bM3Valid             = true;
    sregBuckShadow.bM3FrequencyMode     = true;
    sregBuckShadow.ui32M3Frequency      = ui32M3Frequency;
    sregBuckShadow.sregM3Csr.V          = sregBuckWriteVal.V;
    sregBuckShadow.sregM3Csr2.V         = sregBuckWriteVal2.V;

    //
    // Wait for the write to complete.
    //
    EtaCspRtcFenceFast();

    //
    // Wait for M3 power domain to be stable
    //
    if (iWaitForStable == eBuckStableWait) {
        while(!EtaCspBuckStable());
    }

    //
    // If we are decreasing the # of wait states, do it last.
    //
    if(ui8NewFlashWS < ui8CurrFlashWS)
    {
        EtaCspFlashBrWsSet(ui8NewFlashWS);
    }

    //
    // Done
    //
    return (eEtaSuccess);
}


/***************************************************************************//**
 *
 *  EtaCspBuckDspFrequencySet - Turn on the DSP Buck and set the
 *  target frequency
 *
 *  @param ui32DspFrequency DSP Buck Frequency Target
 *  @param iWaitForStable Indicates whether to wait for PMIC
 *                        stability or higher level code will
 *                        handle this
 *  @return Return success/fail status
 *
 ******************************************************************************/
tEtaStatus
EtaCspBuckDspFrequencySet(uint32_t          ui32DspFrequency,
                          tBuckWait4Stable  iWaitForStable)
{
    reg_rtc_buck_dsp_csr_t   sregBuckWriteVal;
    reg_rtc_buck_dsp_csr2_t  sregBuckWriteVal2;
    uint8_t                  ui8OscDiv;


    //
    // Confirm that the DSP is on before proceeding
    //
    if (EtaCspDspPowerStateGet() == eDspPowerOff) {
        return (eEtaBuckDspPoweredOff);
    }

    //
    // Check to ensure that the target is legal
    //  * DSP cannot be set to low frequencies if AO is > 600mV
    //  * DSP cannot be set to high frequencies if MEM is at 900mV
    //
    if (!sregBuckShadow.bTargetsQualified) {
        if (!BuckTargetsValid(BuckGetAoVoltage(),
                              BuckGetM3Voltage(),
                              BuckGetM3Frequency(),
                              eBuckDspFrequencyMode,
                              ui32DspFrequency,
                              BuckGetMemVoltage() ) ) {
        return (eEtaBuckInvalidDspFrequency);
        }
    }

    //
    // Initialize all fields to 0's
    //
    sregBuckWriteVal.V  = 0x0;

    //
    // Set the Buck to use the external reference clock in all cases
    //
    sregBuckWriteVal.BF.SEL_REF_EXT = 1;

    //
    // We set the trim in frequency mode also as it controls the limits the
    // voltage can range to when acquiring frequency. We set this to -1 (7)
    // to give us some voltage margin in frequency
    //
    sregBuckWriteVal.BF.VREG_TRIM = 7;  // FIXME - change to use offset from trim in flash info space when ready

    //
    // The Buck needs to monitor the voltage inside of the pad switch
    // when we are active and outside of the pad switch if we are going
    // to sleep/powerdown state. For frequency mode the switch must
    // be on, so always monitor inside of the switch.
    //
    sregBuckWriteVal.BF.SEL_VREG = 1;

    //
    // We are setting frequency mode, so switch to that. This bit goes active
    // immediately when written to the CSR
    //
    sregBuckWriteVal.BF.FMODE_RST_L_N = 1;

    //
    // In frequency mode we want to enable the min and max limits for the Buck
    // to be in safe modes.
    //
    sregBuckWriteVal.BF.OP_MODE = 4;

    //
    // Now create the frequency-specific variables
    //
    if (ui32DspFrequency <= 32000) {
        sregBuckWriteVal.BF.SEL_PD      = 64;
        ui8OscDiv                       = 2;                // FIXME - need to vary the trim based on the ATE-set trim written in flash
    } else if (ui32DspFrequency <= 40000) {
        sregBuckWriteVal.BF.SEL_PD      = 60;
        ui8OscDiv                       = 2;                // FIXME - need to vary the trim based on the ATE-set trim written in flash
    } else if (ui32DspFrequency <= 48000) {
        sregBuckWriteVal.BF.SEL_PD      = 58;
        ui8OscDiv                       = 1;                // FIXME - need to vary the trim based on the ATE-set trim written in flash
    } else if (ui32DspFrequency <= 64000) {
        sregBuckWriteVal.BF.SEL_PD      = 54;
        ui8OscDiv                       = 1;                // FIXME - need to vary the trim based on the ATE-set trim written in flash
    } else if (ui32DspFrequency <= 80000) {
        sregBuckWriteVal.BF.SEL_PD      = 50;
        ui8OscDiv                       = 1;                // FIXME - need to vary the trim based on the ATE-set trim written in flash
    } else if (ui32DspFrequency <= 96000) {
        sregBuckWriteVal.BF.SEL_PD      = 48;
        ui8OscDiv                       = 1;                // FIXME - need to vary the trim based on the ATE-set trim written in flash
    } else {
        sregBuckWriteVal.BF.SEL_PD      = 44;
        ui8OscDiv                       = 1;                // FIXME - need to vary the trim based on the ATE-set trim written in flash
    }

    //
    // Capture the oscillator base clock settings and divide
    // value into the structure
    //
    sregBuckWriteVal.BF.SEL_OSC     = 0;                    // For all settings of frequency we will use the HFO/VHFO as the base
    sregBuckWriteVal.BF.SEL_OSC_DIV = ui8OscDiv;            // FIXME - need to vary the trim based on the ATE-set trim written in flash

    //
    // Determine the frequency target value
    //
    // ui32FreqTarget = (m3 osc frequency / ref clock frequency) / 2
    //
    // Optimize that to:
    // ui32FreqTarget = (m3 osc freq / (ref clock base / ref clock div) / 2
    // ui32FreqTarget = (m3 osc freq * ref clock div) / (ref clock base * 2)
    // ui32FreqTarget = (m3 osc freq * ref clock div) / (4000 or 4096 or 128)
    //
    if (sregBuckShadow.sregHfClockSource == eBuckHfClockSourceHfo) {
        sregBuckWriteVal2.V = (ui32DspFrequency * (1 << ui8OscDiv)) / 4000;
    } else {
        sregBuckWriteVal2.V = (ui32DspFrequency * (1 << ui8OscDiv)) / 4096;
    }

    //
    // Adjust the calculated value using the offset
    //
    sregBuckWriteVal2.V += sregBuckShadow.i8DspFreqOffset;  // FIXME - this may need to become an equation relative to refclk

    //
    // Write the frequency target value.
    //
    REG_RTC_BUCK_DSP_CSR2.V = sregBuckWriteVal2.V;

    //
    // Write the new value first allowing time for value to stabilize before
    // strobing the write signal.
    //
    REG_RTC_BUCK_DSP_CSR.V = sregBuckWriteVal.V;

    //
    // Finally, set the write bit.
    //
    sregBuckWriteVal.BF.WRITE   = 1;
    REG_RTC_BUCK_DSP_CSR.V      = sregBuckWriteVal.V;

    //
    // Capture that we have set the DSP Buck CSRs
    //
    sregBuckShadow.bDspValid             = true;
    sregBuckShadow.bDspFrequencyMode     = true;
    sregBuckShadow.ui32DspFrequency      = ui32DspFrequency;
    sregBuckShadow.sregDspCsr.V          = sregBuckWriteVal.V;
    sregBuckShadow.sregDspCsr2.V         = sregBuckWriteVal2.V;

    //
    // Wait for the write to complete.
    //
    EtaCspRtcFenceFast();

    //
    // Wait for DSP power domain to be stable
    //
    if (iWaitForStable == eBuckStableWait) {
        while(!EtaCspBuckStable());
    }

    //
    // Done
    //
    return (eEtaSuccess);
}


/***************************************************************************//**
 *
 *  EtaCspBuckDspPrePowerDown - Prepare the DSP buck for
 *  powerdown.
 *
 *  @return Return success/fail status
 *
 ******************************************************************************/
tEtaStatus
EtaCspBuckDspPrePowerDown(void)
{
    //
    // Make sure the DSP is on before doing anything
    //
    if (EtaCspDspPowerStateGet() == eDspPowerOn) {
        //
        // Set a flag to indicate that we are going to powerdown the DSP Buck
        //
        sregBuckShadow.bEnterSleep  = true;

        //
        // Set to Voltage mode and a safe voltage
        //
        EtaCspBuckDspVoltageSet(eBuckDspVoltage700Mv, false);

        //
        // Clear the flag now so that others are not affected
        //
        sregBuckShadow.bEnterSleep  = false;
    }

    //
    // Done
    //
    return (eEtaSuccess);
}

/***************************************************************************//**
 *
 *  EtaCspBuckPrePowerDown - Prepare the bucks for powerdown.
 *  Applications should not call this directly, it is called
 *  during the powerdown sequence by the function
 *  EtaCspPwrSocShutdown().
 *
 *  @return Return success/fail status
 *
 ******************************************************************************/
tEtaStatus
EtaCspBuckPrePowerDown(void)
{
    tEtaStatus  eEtaStatus;

    // Set the voltages to a known good point and return
    EtaCspBuckDspPrePowerDown();

    //
    // Set a flag to indicate that we are going to powerdown
    //  Have to do this after DSP PrePowerDown since it also sets then clears it
    //
    sregBuckShadow.bEnterSleep  = true;

    //
    // Set the bucks - have to do this explicitly and not use SetAll. SetAll will
    // check if the new target matches current and will not do anything if so.
    // Unfortunately we need to change the SEL_VREG bit for the M3, so we need to
    // force the setting to happen.
    //
    if ((eEtaStatus = EtaCspBuckM3VoltageSet(eBuckM3Voltage700Mv, false)) != eEtaSuccess) {
        sregBuckShadow.bEnterSleep = false;
        return (eEtaStatus);
    }
    if ((eEtaStatus = EtaCspBuckMemVoltageSet(eBuckMem900Mv, false)) != eEtaSuccess) {
        return (eEtaStatus);
    }
    if ((eEtaStatus = EtaCspBuckAoVoltageSet(eBuckAo600Mv, false)) != eEtaSuccess) {
        return (eEtaStatus);
    }

    //
    // Clear the flag now so that others are not affected
    //
    sregBuckShadow.bEnterSleep = false;

    //
    // All done, ready for powerdown/sleep
    //
    return (eEtaSuccess);
}


/***************************************************************************//**
 *
 *  EtaCspBuckM3PreStall - Prepare the bucks for M3 stall. This
 *  will put them into a low-power safe state. The PostStall
 *  function must be called as soon as the stall is complete.
 *
 *  @return Return success/fail status
 *
 ******************************************************************************/
tEtaStatus
EtaCspBuckM3PreStall(void)
{
    // reg_rtc_pmic_ao_csr_t   sregAoCsr  = sregBuckShadow.sregAoCsr;
    reg_rtc_pmic_m3_csr_t   sregM3Csr  = sregBuckShadow.sregM3Csr;
    reg_rtc_pmic_mem_csr_t  sregMemCsr = sregBuckShadow.sregMemCsr;

    //
    // Before we change the buck, capture the current state
    //
    sregBuckShadow.sregM3VoltagePreStall    = BuckGetM3Voltage();
    sregBuckShadow.ui32M3FrequencyPreStall  = BuckGetM3Frequency();
    sregBuckShadow.bM3StallValid            = true;

    //
    // Now change the M3 Buck to voltage mode and set the voltage
    // dependent on the AO and MEM buck settings
    //
    if ((BuckGetMemVoltage() < eBuckMem1100Mv) && (BuckGetAoVoltage() < eBuckAo700Mv)) {
        EtaCspBuckM3VoltageSet(eBuckM3Voltage600Mv, false);
    } else {
        EtaCspBuckM3VoltageSet(eBuckM3Voltage700Mv, false);
    }

    //
    // Manually change the reference clocks to lower power
    // in low performance case
    //
    // DO NOT DO THIS FOR AO. This affects the HFO which thus causes potential
    // issues to devices like the UART. Just leave AO alone.
    // sregAoCsr.BF.WRITE          = 0x0;
    // sregAoCsr.BF.SEL_OSC        = 0x1;
    // sregAoCsr.BF.SEL_OSC_DIV    = 0x2;
    // REG_RTC_PMIC_AO_CSR.V       = sregAoCsr.V;

    sregMemCsr.BF.WRITE         = 0x0;
    sregMemCsr.BF.SEL_OSC       = 0x1;
    sregMemCsr.BF.SEL_OSC_DIV   = 0x1;
    REG_RTC_PMIC_MEM_CSR.V      = sregMemCsr.V;

    //
    // If no one else is using the MEM domain then we can move it to a lower
    // power state also
    //
    if (EtaCspDspPowerStateGet() == eDspPowerOff) {
        sregM3Csr.BF.WRITE          = 0x0;
        sregM3Csr.BF.SEL_OSC        = 0x1;
        sregM3Csr.BF.SEL_OSC_DIV    = 0x1;
        REG_RTC_PMIC_M3_CSR.V       = sregM3Csr.V;
    }

    return (eEtaSuccess);
}


/***************************************************************************//**
 *
 *  EtaCspBuckM3PostStall - Restore the bucks (AO, M3, MEM) to
 *  their pre-M3 Stall state.
 *
 *  @return Return success/fail status
 *
 ******************************************************************************/
tEtaStatus
EtaCspBuckM3PostStall(void)
{
    reg_rtc_pmic_ao_csr_t   sregAoCsr  = sregBuckShadow.sregAoCsr;
    reg_rtc_pmic_m3_csr_t   sregM3Csr  = sregBuckShadow.sregM3Csr;
    reg_rtc_pmic_mem_csr_t  sregMemCsr = sregBuckShadow.sregMemCsr;

    //
    // Returning from stall, we need to reset the Buck back to pre-stall settings
    // First, confirm that we have valid settings
    //
    if (!sregBuckShadow.bM3StallValid) {
        return (eEtaFailure);
    }

    //
    // Bring the buck reference clocks back up to their pre-stall state
    //
    sregM3Csr.BF.WRITE      = 0;
    REG_RTC_PMIC_M3_CSR.V   = sregM3Csr.V;

    sregAoCsr.BF.WRITE      = 0;
    REG_RTC_PMIC_AO_CSR.V   = sregAoCsr.V;

    sregMemCsr.BF.WRITE     = 0;
    REG_RTC_PMIC_MEM_CSR.V  = sregMemCsr.V;

    //
    // Clear the bit indicating we had valid settings
    //
    sregBuckShadow.bM3StallValid = false;

    //
    // Now determine if we were in voltage or frequency mode and handle accordingly
    //
    if (sregBuckShadow.sregM3VoltagePreStall == eBuckM3FrequencyMode) {
        return (EtaCspBuckM3FrequencySet(sregBuckShadow.ui32M3FrequencyPreStall, false));
    } else {
        return (EtaCspBuckM3VoltageSet(sregBuckShadow.sregM3VoltagePreStall, false));
    }
}


/***************************************************************************//**
 *
 *  EtaCspBuckDspPreStall - Prepare the DSP buck for stall
 *
 *  @return Return success/fail status
 *
 ******************************************************************************/
tEtaStatus
EtaCspBuckDspPreStall(void)
{
    //
    // Only need to set up the DSP here, leave AO and MEM alone.
    //
    // Safe off current state to allow restore after with another funciton?
    //

    //
    // Before we change the buck, capture the current state
    //
    sregBuckShadow.sregDspVoltagePreStall    = BuckGetDspVoltage();
    sregBuckShadow.ui32DspFrequencyPreStall  = BuckGetDspFrequency();
    sregBuckShadow.bDspStallValid            = true;

    //
    // Now change to voltage mode and return
    //
    return (EtaCspBuckDspVoltageSet(eBuckDspVoltage700Mv, false));
}


/***************************************************************************//**
 *
 *  EtaCspBuckDspPostStall - Prepare the DSP buck after stall
 *
 *  @return Return success/fail status
 *
 ******************************************************************************/
tEtaStatus
EtaCspBuckDspPostStall(void)
{
    //
    // Returning from stall, we need to reset the Buck back to pre-stall settings
    // First, confirm that we have valid settings
    //
    if (!sregBuckShadow.bDspStallValid) {
        return (eEtaFailure);
    }

    //
    // Clear the bit indicating we had valid settings
    //
    sregBuckShadow.bDspStallValid = false;

    //
    // Now determine if we were in voltage or frequency mode and handle accordingly
    //
    if (sregBuckShadow.sregDspVoltagePreStall == eBuckDspFrequencyMode) {
        return (EtaCspBuckDspFrequencySet(sregBuckShadow.ui32DspFrequencyPreStall, false));
    } else {
        return (EtaCspBuckDspVoltageSet(sregBuckShadow.sregDspVoltagePreStall, false));
    }
}


/***************************************************************************//**
 *
 *  EtaCspBuckInit - Set the AO, M3, and MEM Buck Regulators to
 *  known safe values. This will leave the DSP powered off.
 *
 *  @return Return the status.
 *
 *  When called, this function will check to see if the Bucks
 *  are in their POR states (should only happen on a
 *  cold-start). If not, the function will populate the
 *  structure which maintains the current state based on the
 *  state of the Bucks. If so, the Bucks will be set up with
 *  good known values of AO to 0.7V, M3 to 0.7V (voltage mode),
 *  and MEM to 0.9V. These are safe settings.
 *
 ******************************************************************************/
tEtaStatus
EtaCspBuckInit(void)
{
    uint32_t                ui32RC = 0;
    reg_rtc_pmic_ao_csr_t   sregAoCsr;

    //
    // Before we do anything, read the Buck trim values
    // from the Flash Info space. If the Info space is not valid,
    // rely on default of 0 for all trims
    //
    BuckGetFlashInfoAoTrim(&(sregBuckShadow.ui8AoVregTrim));
    BuckGetFlashInfoM3Trim(&(sregBuckShadow.ui8M3VregTrim));
    BuckGetFlashInfoDspTrim(&(sregBuckShadow.ui8DspVregTrim));
    BuckGetFlashInfoMemTrim(&(sregBuckShadow.ui8MemVregTrim));

    //
    // Regardless of how we got here, we are NOT going to sleep
    //
    sregBuckShadow.bEnterSleep  = false;

    //
    // Read the current state of the AO Buck CSR
    //
    sregAoCsr = REG_RTC_PMIC_AO_CSR;

    //
    // Before we check to see if the Bucks already contain valid data, let's
    // load the frequency target offsets from flash if they are there. Otherwise
    // we need to calculate them before we are done and store in the flash.
    //
    BuckGetFlashInfoFreqOffsets(&(sregBuckShadow.i8M3FreqOffset),
                                &(sregBuckShadow.i8DspFreqOffset) );


    //
    // If the AO Buck is in External reference mode then the Bucks have
    // been previously programmed. Therefore, populate the structure
    // with the current CSR values
    //
    if (sregAoCsr.BF.SEL_REF_EXT) {
        sregBuckShadow.sregAoCsr            = sregAoCsr;
        sregBuckShadow.bAoValid             = true;

        sregBuckShadow.sregM3Csr            = REG_RTC_PMIC_M3_CSR;
        sregBuckShadow.sregM3Csr2           = REG_RTC_PMIC_M3_CSR2;
        sregBuckShadow.bM3FrequencyMode     = (sregBuckShadow.sregM3Csr.BF.FMODE_RST_L_N == 1) ? true : false;
        sregBuckShadow.ui32M3Frequency      = ETA_BUCK_AO_0P7V_MEM_0P9V_MIN_FREQ;        // Initialize with an arbitrary mid-point value
        sregBuckShadow.bM3Valid             = true;
        sregBuckShadow.bM3StallValid        = false;

        // If we are coming back from a powerdown, we need to set the
        //      SEL_VREG bit again.
        if (sregBuckShadow.sregM3Csr.BF.SEL_VREG == 0) {
            sregBuckShadow.sregM3Csr.BF.SEL_VREG    = 1;
            REG_RTC_PMIC_M3_CSR.V                   = sregBuckShadow.sregM3Csr.V;
        }

        sregBuckShadow.sregDspCsr           = REG_RTC_BUCK_DSP_CSR;
        sregBuckShadow.sregDspCsr2          = REG_RTC_BUCK_DSP_CSR2;
        sregBuckShadow.bDspFrequencyMode    = (sregBuckShadow.sregDspCsr.BF.FMODE_RST_L_N == 1) ? true : false;
        sregBuckShadow.ui32DspFrequency     = ETA_BUCK_AO_0P7V_MEM_0P9V_MIN_FREQ;       // Initialize with an arbitrary mid-point value
        sregBuckShadow.bDspValid            = true;
        sregBuckShadow.bDspStallValid       = false;

        // If we get here and the DSP Buck is turned on, then we need to
        //      make sure the SEL_VREG is set. The only way this should ever
        //      happen is if BuckInit is called after the DSP Buck is turned
        //      on (which should not happen).
        if ((REG_RTC_PWR.BF.DSP_BUCK == 1) &&
            (sregBuckShadow.sregDspCsr.BF.SEL_VREG == 0)) {
            sregBuckShadow.sregDspCsr.BF.SEL_VREG   = 1;
            REG_RTC_BUCK_DSP_CSR.BF.SEL_VREG        = 1;
        }

        sregBuckShadow.sregMemCsr           = REG_RTC_PMIC_MEM_CSR;
        sregBuckShadow.bMemValid            = true;

        // All done, so just return now, no need to write any new values
        return (eEtaSuccess);
    } else {
        sregBuckShadow.sregAoCsr.V          = 0;
        sregBuckShadow.sregM3Csr.V          = 0;
        sregBuckShadow.sregM3Csr2.V         = 0;
        sregBuckShadow.sregDspCsr.V         = 0;
        sregBuckShadow.sregDspCsr2.V        = 0;
        sregBuckShadow.sregMemCsr.V         = 0;
        sregBuckShadow.bAoValid             = false;
        sregBuckShadow.bM3Valid             = false;
        sregBuckShadow.bDspValid            = true;     // At time 0, we leave it off, so technically it's in a valid state (we won't program it below as we do the others)
        sregBuckShadow.bMemValid            = false;
        sregBuckShadow.bM3FrequencyMode     = false;
        sregBuckShadow.bDspFrequencyMode    = false;
        sregBuckShadow.ui32M3Frequency      = ETA_BUCK_AO_0P7V_MEM_0P9V_MIN_FREQ;        // Initialize with an arbitrary mid-point value
        sregBuckShadow.ui32DspFrequency     = ETA_BUCK_AO_0P7V_MEM_0P9V_MIN_FREQ;        // Initialize with an arbitrary mid-point value
        sregBuckShadow.bM3StallValid        = false;
        sregBuckShadow.bDspStallValid       = false;
    }

    //
    // Clear this flag so that all Set* functions will check the targets
    //
    sregBuckShadow.bTargetsQualified = false;

    //
    // Preload the high frequency clock source indicator
    //
    sregBuckShadow.sregHfClockSource = (REG_RTC_OSC_CSR.BF.BUCK_HF_CLK_SRC) ? eBuckHfClockSourceVhfo : eBuckHfClockSourceHfo;

    //
    // Set up the default value for the FREF AO CSR here
    // such that future powerdown calls will be set correctly
    //
    REG_RTC_PMIC_FREF_AO_CSR.V = 0x00000000;

    //
    // Now we can set the Bucks to known good safe values
    //
    ui32RC = EtaCspBuckAoVoltageSet(eBuckAo700Mv, true);
    if (ui32RC != eEtaSuccess) {
        return (ui32RC);
    }

    ui32RC = EtaCspBuckM3VoltageSet(eBuckM3Voltage700Mv, false);
    if (ui32RC != eEtaSuccess) {
        return (ui32RC);
    }

    ui32RC = EtaCspBuckMemVoltageSet(eBuckMem900Mv, true);
    if (ui32RC != eEtaSuccess) {
        return (ui32RC);
    }

    //
    // Need to retune the HFO after making any changes to AO
    //
    EtaCspHfoTuneDefaultUpdate();

    // All done
    return (eEtaSuccess);
}


/*****************************************************************************
 *
 *  BuckNextState - Function to determine which Buck to change next
 *                  during a programming sequence
 *
 ******************************************************************************/
static tBuckState
BuckNextState(void)
{
    bool        bAoChanging     = !sregBuckShadow.bAoValid;
    bool        bM3Changing     = !sregBuckShadow.bM3Valid;
    bool        bDspChanging    = !sregBuckShadow.bDspValid;
    bool        bMemChanging    = !sregBuckShadow.bMemValid;

    bool        bAoUp           = sregBuckShadow.bAoUp;
    bool        bM3Up           = sregBuckShadow.bM3Up;
    bool        bDspUp          = sregBuckShadow.bDspUp;

    // Determine if we need to take intermediate steps... This could be due to
    // voltage or frequency stepping.
    bool        bM3InterV2V      = ((!sregBuckShadow.bM3FrequencyMode) && (sregBuckShadow.sregM3VoltageTarget != eBuckM3FrequencyMode)) &&
                                   (((BuckGetM3Voltage() == eBuckM3Voltage600Mv) && (sregBuckShadow.sregM3VoltageTarget == eBuckM3Voltage800Mv)) ||
                                    ((BuckGetM3Voltage() == eBuckM3Voltage800Mv) && (sregBuckShadow.sregM3VoltageTarget == eBuckM3Voltage600Mv)) );

    bool        bM3InterV2F      = ((!sregBuckShadow.bM3FrequencyMode) && (sregBuckShadow.sregM3VoltageTarget == eBuckM3FrequencyMode)) &&
                                   (((BuckGetM3Voltage() == eBuckM3Voltage600Mv) && (sregBuckShadow.ui32M3FrequencyTarget > ETA_BUCK_AO_0P6V_MEM_0P9V_MAX_FREQ)) ||
                                    ((BuckGetM3Voltage() == eBuckM3Voltage800Mv) && (sregBuckShadow.ui32M3FrequencyTarget < ETA_BUCK_AO_0P7V_MEM_0P9V_MIN_FREQ)) );

    bool        bM3InterF2F      = (( sregBuckShadow.bM3FrequencyMode) && (sregBuckShadow.sregM3VoltageTarget == eBuckM3FrequencyMode)) &&
                                   (((BuckGetM3Frequency() > ETA_BUCK_AO_0P7V_MEM_0P9V_MIN_FREQ) && (sregBuckShadow.ui32M3FrequencyTarget < ETA_BUCK_AO_0P7V_MEM_0P9V_MIN_FREQ)) ||
                                    ((BuckGetM3Frequency() < ETA_BUCK_AO_0P7V_MEM_0P9V_MIN_FREQ) && (sregBuckShadow.ui32M3FrequencyTarget > ETA_BUCK_AO_0P7V_MEM_0P9V_MIN_FREQ)) );

    bool        bM3InterF2V      = (( sregBuckShadow.bM3FrequencyMode) && (sregBuckShadow.sregM3VoltageTarget != eBuckM3FrequencyMode)) &&
                                   (((BuckGetM3Frequency() > ETA_BUCK_AO_0P7V_MEM_0P9V_MIN_FREQ) && (sregBuckShadow.sregM3VoltageTarget < eBuckM3Voltage700Mv)) ||
                                    ((BuckGetM3Frequency() < ETA_BUCK_AO_0P7V_MEM_0P9V_MIN_FREQ) && (sregBuckShadow.sregM3VoltageTarget > eBuckM3Voltage700Mv)) );

    bool        bDspInterV2V     = ((!sregBuckShadow.bDspFrequencyMode) && (sregBuckShadow.sregDspVoltageTarget != eBuckDspFrequencyMode)) &&
                                   (((BuckGetDspVoltage() == eBuckDspVoltage600Mv) && (sregBuckShadow.sregDspVoltageTarget == eBuckDspVoltage800Mv)) ||
                                    ((BuckGetDspVoltage() == eBuckDspVoltage800Mv) && (sregBuckShadow.sregDspVoltageTarget == eBuckDspVoltage600Mv)) );

    bool        bDspInterV2F     = ((!sregBuckShadow.bDspFrequencyMode) && (sregBuckShadow.sregDspVoltageTarget == eBuckDspFrequencyMode)) &&
                                   (((BuckGetDspVoltage() == eBuckDspVoltage600Mv) && (sregBuckShadow.ui32DspFrequencyTarget > ETA_BUCK_AO_0P6V_MEM_0P9V_MAX_FREQ)) ||
                                    ((BuckGetDspVoltage() == eBuckDspVoltage800Mv) && (sregBuckShadow.ui32DspFrequencyTarget < ETA_BUCK_AO_0P7V_MEM_0P9V_MIN_FREQ)) );

    bool        bDspInterF2F     = (( sregBuckShadow.bDspFrequencyMode) && (sregBuckShadow.sregDspVoltageTarget == eBuckDspFrequencyMode)) &&
                                   (((BuckGetDspFrequency() > ETA_BUCK_AO_0P7V_MEM_0P9V_MIN_FREQ) && (sregBuckShadow.ui32DspFrequencyTarget < ETA_BUCK_AO_0P7V_MEM_0P9V_MIN_FREQ)) ||
                                    ((BuckGetDspFrequency() < ETA_BUCK_AO_0P7V_MEM_0P9V_MIN_FREQ) && (sregBuckShadow.ui32DspFrequencyTarget > ETA_BUCK_AO_0P7V_MEM_0P9V_MIN_FREQ)) );

    bool        bDspInterF2V     = (( sregBuckShadow.bDspFrequencyMode) && (sregBuckShadow.sregDspVoltageTarget != eBuckDspFrequencyMode)) &&
                                   (((BuckGetDspFrequency() > ETA_BUCK_AO_0P7V_MEM_0P9V_MIN_FREQ) && (sregBuckShadow.sregDspVoltageTarget < eBuckDspVoltage700Mv)) ||
                                    ((BuckGetDspFrequency() < ETA_BUCK_AO_0P7V_MEM_0P9V_MIN_FREQ) && (sregBuckShadow.sregDspVoltageTarget > eBuckDspVoltage700Mv)) );


    //
    // Most complicated case is when all 3 (considering M3 / DSP as 1 domain)
    // domains have to change. Need to determine which has to go first based
    // on current and next targets, and some cases will require an intermediate
    // step to maintain safe ranges
    //
    if (bAoChanging && (bM3Changing || bDspChanging) && bMemChanging) {
        // M3 going from 0.6 -> 0.9 means we need to go to 0.7 first
        if ((bM3Changing) && (bM3Up) && (bM3InterV2V || bM3InterV2F || bM3InterF2V || bM3InterF2F)) {
            return (eBuckSetM3Intermediate);
        }
        // DSP going from 0.6 -> 0.9 means we need to go to 0.7 first
        if ((bDspChanging) && (bDspUp) && (bDspInterV2V || bDspInterV2F || bDspInterF2V || bDspInterF2F)) {
            return (eBuckSetDspIntermediate);
        }
        // M3 going from 0.9 to 0.6 means we need to go to 0.7 first
        if ((bM3Changing) && (!bM3Up) && (bM3InterV2V || bM3InterV2F || bM3InterF2V || bM3InterF2F)) {
            return (eBuckSetM3Intermediate);
        }
        // DSP going from 0.9 to 0.6 means we need to go to 0.7 first
        if ((bDspChanging) && (!bDspUp) && (bDspInterV2V || bDspInterV2F || bDspInterF2V || bDspInterF2F)) {
            return (eBuckSetDspIntermediate);
        }

        // M3 going from 0.7 to 0.9 or big frequency jump requiring AO to change, single step
        if ((bM3Changing) && (bM3Up) && (((!sregBuckShadow.bM3FrequencyMode) && (BuckGetM3Voltage() == eBuckM3Voltage700Mv)) ||
                                         (( sregBuckShadow.bM3FrequencyMode) && (BuckGetM3Frequency() >= ETA_BUCK_AO_0P7V_MEM_0P9V_MIN_FREQ)))) {
            return (eBuckSetAo);
        }
        // DSP going from 0.7 to 0.9 or big frequency jump requiring AO to change, single step
        if ((bDspChanging) && (bDspUp) && (((!sregBuckShadow.bDspFrequencyMode) && (BuckGetDspVoltage() == eBuckDspVoltage700Mv)) ||
                                           (( sregBuckShadow.bDspFrequencyMode) && (BuckGetDspFrequency() >= ETA_BUCK_AO_0P7V_MEM_0P9V_MIN_FREQ)))) {
            return (eBuckSetAo);
        }
        // M3 going from 0.6 to 0.7 (single step) or M3 in frequency mode without interdependence
        if ((bM3Changing) && (bM3Up)) {
            return (eBuckSetM3);
        }
        // DSP going from 0.6 to 0.7 (single step) or DSP in frequency mode without interdependence
        if ((bDspChanging) && (bDspUp)) {
            return (eBuckSetDsp);
        }

        // M3 going from 0.7 to 0.6, mem goes first
        if ((bM3Changing) && (!bM3Up) && (BuckGetM3Voltage() == eBuckM3Voltage700Mv)) {
            return (eBuckSetMem);
        }
        // DSP going from 0.7 to 0.6, mem goes first
        if ((bDspChanging) && (!bDspUp) && (BuckGetDspVoltage() == eBuckDspVoltage700Mv)) {
            return (eBuckSetMem);
        }
        // M3 going from 0.9 to 0.7(single step) or M3 in frequency mode without interdependence
        if ((bM3Changing) && (!bM3Up)) {
            return (eBuckSetM3);
        }
        // DSP going from 0.9 to 0.7 (single step) or DSP in frequency mode without interdependence
        if ((bDspChanging) && (!bDspUp)) {
            return (eBuckSetDsp);
        }

        // Should never get here, but if we do, return idle as nothing left to do.
        return eBuckIdle;

    } else if (bAoChanging && (bM3Changing || bDspChanging)) {
        if (bM3Up && bM3Changing) {
            return (eBuckSetM3);
        } else if (bDspUp && bDspChanging) {
            return (eBuckSetDsp);
        } else {
            return (eBuckSetAo);
        }

    } else if ((bM3Changing || bDspChanging) && bMemChanging) {
        if (!bM3Up && bM3Changing) {
            return (eBuckSetM3);
        } else if (!bDspUp && bDspChanging) {
            return (eBuckSetDsp);
        } else {
            return (eBuckSetMem);
        }

    } else if (bAoChanging && bMemChanging) {
        if (bAoUp) {
            return (eBuckSetAo);
        } else {
            return (eBuckSetMem);
        }

    } else if (bM3Changing) {
        return (eBuckSetM3);

    } else if (bDspChanging) {
        return (eBuckSetDsp);

    } else if (bAoChanging) {
        return (eBuckSetAo);

    } else {
        return (eBuckSetMem);
    }
}


/***************************************************************************//**
 *
 *  EtaCspBuckAllSet - Set the AO, M3, DSP, and MEM Buck
 *  Regulators to specified values putting the M3 and DSP into
 *  either voltage or frequency mode. The DSP will only be set
 *  if a valid target is received.
 *
 *  @param iAoTarget AO Voltage Target
 *  @param iM3Target M3 Target (Specify Voltage Target or
 *                   Frequency Mode)
 *  @param ui32M3Frequency
 *  @param iDspTarget DSP Target (Specify Voltage Target or
 *                   Frequency Mode)
 *  @param ui32DspFrequency If DSP in frequency mode, specify
 *                          target frequency, otherwise ignored
 *  @param iMemTarget MEM Voltage Target
 *  @param iAllAtOnce If true the call to SetAll will perform
 *                    programming of all Bucks before returning
 *                    to the calling code.
 *  @param iWaitForStable If true the call to SetAll will not
 *                        return until the 200mS stability
 *                        counter of the Bucks has counted out.
 *  @return Return the status.
 *
 *  One should call this function to initiate the sequence of
 *  changing the Buck voltages / frequencies. Once this is
 *  called, it may return a status of eEtaBuckCallBackLater.
 *  When this is received, the calling code should call back the
 *  function EtaCspBuckAllSetWait until a return code of
 *  eEtaSuccess is received. At that point, all Bucks will be in
 *  their requested state.
 *
 *  If the user calls this function before receiving the
 *  Success status that implies that the previous sequence
 *  should be aborted and new targets will be used for further
 *  programming.
 *
 *  Calling this function with the iAllAtOnce flag set to true
 *  will cause all Buck programming to happen before a return to
 *  the calling program. In this case, the calling program will
 *  simply wait until all programming is complete. This paradigm
 *  is simpler for the calling code but will take longer to
 *  regain control.
 *
 ******************************************************************************/
tEtaStatus
EtaCspBuckAllSet(tBuckAoTarget          iAoTarget,
                 tBuckM3Target          iM3Target,
                 uint32_t               ui32M3Frequency,
                 tBuckDspTarget         iDspTarget,
                 uint32_t               ui32DspFrequency,
                 tBuckMemTarget         iMemTarget,
                 tBuckAllAtOnce         iAllAtOnce,
                 tBuckWait4Stable       iWaitForStable)
{
    bool            bNoChange = true;
    tBuckState      sregNextState;

    //
    // Initialize this to false on any call to SetAll in case the
    // user preempts an on-going set of transitions. In that case we
    // need to re-check the targets
    //
    sregBuckShadow.bTargetsQualified = false;

    //
    // If we are waiting for a prior change to complete and this function
    // is called again, previous transitions will be aborted and new
    // targets will be used. If invalid new targets are sent, this function
    // will abort and potentially leave the device in a bad state.
    //
    // First check to ensure that the targets are legal (summary):
    //  * AO target must be less than M3 target
    //  * AO target must be less than DSP target (if enabled)
    //  * M3 target must be less than MEM target
    //  * DSP target must be less than MEM target (if enabled)
    //  * AO and MEM cannot be set to 0.6V and 1.2V respectively
    //
    if (!BuckTargetsValid(iAoTarget,
                          iM3Target,
                          ui32M3Frequency,
                          iDspTarget,
                          ui32DspFrequency,
                          iMemTarget)) {
        return (eEtaFailure);
    }

    //
    // We have validated the target so let's invalidate any current voltage
    // which needs to change and capture transition indicators
    //
    if (iAoTarget != BuckGetAoVoltage()) {
        sregBuckShadow.bAoUp                        = (iAoTarget > BuckGetAoVoltage()) ? true : false;
        sregBuckShadow.sregAoVoltageTarget          = iAoTarget;
        sregBuckShadow.bAoValid                     = false;
        bNoChange                                   = false;
    }

    if ((iM3Target != eBuckM3FrequencyMode) && (!sregBuckShadow.bM3FrequencyMode)) {
        if (iM3Target != BuckGetM3Voltage()) {
            sregBuckShadow.bM3Up                    = (iM3Target > BuckGetM3Voltage()) ? true : false;
            sregBuckShadow.sregM3VoltageTarget      = iM3Target;
            sregBuckShadow.ui32M3FrequencyTarget    = 0;
            sregBuckShadow.bM3Valid                 = false;
            bNoChange                               = false;
        }
    } else if ((iM3Target != eBuckM3FrequencyMode) && (sregBuckShadow.bM3FrequencyMode)) {
        sregBuckShadow.bM3Up                        = (iM3Target > BuckVoltageFromFrequency(sregBuckShadow.ui32M3Frequency)) ? true : false;
        sregBuckShadow.sregM3VoltageTarget          = iM3Target;
        sregBuckShadow.ui32M3FrequencyTarget        = 0;
        sregBuckShadow.bM3Valid                     = false;
        bNoChange                                   = false;
    } else if ((iM3Target == eBuckM3FrequencyMode) && (sregBuckShadow.bM3FrequencyMode)) {
        if (ui32M3Frequency != BuckGetM3Frequency()) {
            sregBuckShadow.bM3Up                    = (ui32M3Frequency > BuckGetM3Frequency()) ? true : false;
            sregBuckShadow.sregM3VoltageTarget      = iM3Target;
            sregBuckShadow.ui32M3FrequencyTarget    = ui32M3Frequency;
            sregBuckShadow.bM3Valid                 = false;
            bNoChange                               = false;
        }
//    } else if ((iM3Target == eBuckM3FrequencyMode) && (!sregBuckShadow.bM3FrequencyMode)) {   // Doesn't need an explicit check, this is the exhaustive case
    } else {
        sregBuckShadow.bM3Up                        = (ui32M3Frequency > BuckFrequencyFromVoltage(BuckGetM3Voltage())) ? true : false;
        sregBuckShadow.sregM3VoltageTarget          = iM3Target;
        sregBuckShadow.ui32M3FrequencyTarget        = ui32M3Frequency;
        sregBuckShadow.bM3Valid                     = false;
        bNoChange                                   = false;
    }

    if (iDspTarget != eBuckDspOff) {
        if ((iDspTarget != eBuckDspFrequencyMode) && (!sregBuckShadow.bDspFrequencyMode)) {
            if (iDspTarget != BuckGetDspVoltage()) {
                sregBuckShadow.bDspUp                   = (iDspTarget > BuckGetDspVoltage()) ? true : false;
                sregBuckShadow.sregDspVoltageTarget     = iDspTarget;
                sregBuckShadow.ui32DspFrequencyTarget   = 0;
                sregBuckShadow.bDspValid                = false;
                bNoChange                               = false;
            }
        } else if ((iDspTarget != eBuckDspFrequencyMode) && (sregBuckShadow.bDspFrequencyMode)) {
            sregBuckShadow.bDspUp                       = (iDspTarget > BuckVoltageFromFrequency(sregBuckShadow.ui32DspFrequency)) ? true : false;
            sregBuckShadow.sregDspVoltageTarget         = iDspTarget;
            sregBuckShadow.ui32DspFrequencyTarget       = 0;
            sregBuckShadow.bDspValid                    = false;
            bNoChange                                   = false;
        } else if ((iDspTarget == eBuckDspFrequencyMode) && (sregBuckShadow.bDspFrequencyMode)) {
            if (ui32DspFrequency != BuckGetDspFrequency()) {
                sregBuckShadow.bDspUp                   = (ui32DspFrequency > BuckGetDspFrequency()) ? true : false;
                sregBuckShadow.sregDspVoltageTarget     = iDspTarget;
                sregBuckShadow.ui32DspFrequencyTarget   = ui32DspFrequency;
                sregBuckShadow.bDspValid                = false;
                bNoChange                               = false;
            }
//        } else if ((iDspTarget == eBuckDspFrequencyMode) && (!sregBuckShadow.bDspFrequencyMode)) {    // Doesn't need an explicit check, this is the exhaustive case
        } else {
            sregBuckShadow.bDspUp                       = (ui32DspFrequency > BuckFrequencyFromVoltage(BuckGetDspVoltage())) ? true : false;
            sregBuckShadow.sregDspVoltageTarget         = iDspTarget;
            sregBuckShadow.ui32DspFrequencyTarget       = ui32DspFrequency;
            sregBuckShadow.bDspValid                    = false;
            bNoChange                                   = false;
        }
    } else {
        // If SetAll is called with DSP indicated off, capture that state
        // The DSP Buck is turned on or off outside of the Buck functions
        // in the DSP CSP code.
        sregBuckShadow.bDspUp                       = false;
        sregBuckShadow.sregDspVoltageTarget         = iDspTarget;
        sregBuckShadow.ui32DspFrequencyTarget       = 0;
        sregBuckShadow.bDspValid                    = true;
    }

    if (iMemTarget != BuckGetMemVoltage()) {
        sregBuckShadow.bMemUp                       = (iMemTarget > BuckGetMemVoltage()) ? true : false;
        sregBuckShadow.sregMemVoltageTarget         = iMemTarget;
        sregBuckShadow.bMemValid                    = false;
        bNoChange                                   = false;
    }

    //
    // If there are no changes to the current state just exit
    //
    if (bNoChange) {
        // Do this just in case we were not stable upon entry
        if (iWaitForStable == eBuckStableWait) {
            while (!EtaCspBuckStable());
        }
        return (eEtaSuccess);
    }

    //
    // Now set the Buck voltages in the correct order
    //
    sregNextState = BuckNextState();


    switch (sregNextState) {
        case eBuckSetAo: {
            EtaCspBuckAoVoltageSet(iAoTarget, true);             // FIXME - consider doing this conditionally only when AO->0.6V
            break;
        }
        default: // pick one
        case eBuckSetM3: {
            if (iM3Target == eBuckM3FrequencyMode) {
                EtaCspBuckM3FrequencySet(ui32M3Frequency, false);
            } else {
                EtaCspBuckM3VoltageSet(iM3Target, false);
            }
            break;
        }
        case eBuckSetM3Intermediate: {
            if (iM3Target == eBuckM3FrequencyMode) {
                EtaCspBuckM3FrequencySet(ETA_BUCK_AO_0P7V_MEM_0P9V_MIN_FREQ, false);
            } else {
                EtaCspBuckM3VoltageSet(eBuckM3Voltage700Mv, false);
            }
            // Invalidate the M3 to force the second transition later
            sregBuckShadow.bM3Valid = false;
            break;
        }
        case eBuckSetDsp: {
            if (iDspTarget == eBuckDspFrequencyMode) {
                EtaCspBuckDspFrequencySet(ui32DspFrequency, false);
            } else {
                EtaCspBuckDspVoltageSet(iDspTarget, false);
            }
            break;
        }
        case eBuckSetDspIntermediate: {
            if (iDspTarget == eBuckDspFrequencyMode) {
                EtaCspBuckDspFrequencySet(ETA_BUCK_AO_0P7V_MEM_0P9V_MIN_FREQ, false);
            } else {
                EtaCspBuckDspVoltageSet(eBuckDspVoltage700Mv, false);
            }
            // Invalidate the DSP to force the second transition later
            sregBuckShadow.bDspValid = false;
            break;
        }
        case eBuckSetMem: {
            EtaCspBuckMemVoltageSet(iMemTarget, false);
            break;
        }
    }


    //
    // If the AllAtOnce flag is set, go perform the other programming
    // and wait for stability, then all done.
    //
    if (iAllAtOnce == eBuckSetAllBucks) {
        // Set the qualified flag here to prevent re-checking the
        // targets in the Finish function
        sregBuckShadow.bTargetsQualified = true;

        EtaCspBuckAllSetFinish();

        if (iWaitForStable == eBuckStableWait) {
            while (!EtaCspBuckStable());
        }

        return (eEtaSuccess);
    }

    //
    // Check to see if any more Bucks need to be changed
    //
    if ((sregBuckShadow.bAoValid  == true) &&
        (sregBuckShadow.bM3Valid  == true) &&
        (sregBuckShadow.bDspValid == true) &&
        (sregBuckShadow.bMemValid == true) ){

        if (iWaitForStable == eBuckStableWait) {
            while (!EtaCspBuckStable());
        }

        return (eEtaSuccess);
    } else {
        // Set the qualified flag here to prevent re-checking the
        // targets in the Wait function
        sregBuckShadow.bTargetsQualified = true;
        return (eEtaBuckCallBackLater);
    }
}


/***************************************************************************//**
 *
 *  EtaCspBuckAllSetWait - Call this function after calling
 *  EtaCapBuckSetAll and receiving a return code of
 *  eEtaBuckCallBackLater. This function will continue the
 *  sequencing of programming the Buck converters. When called,
 *  it first checks to see if all programming is complete. If so,
 *  it will then check the iWaitForStable parameter. If the parameter
 *  is true, the function will wait for the Bucks to become stable
 *  before returning. If false, the function will return immediately.
 *  When all are done with the transition this function will
 *  return eEtaSuccess.
 *
 *  @param iWaitForStable If true the call to SetAllWait will wait
 *                        on the last call when all Buck CSRs have
 *                        been programmed until the 200mS stability
 *                        counter of the Bucks has counted out.
 *
 *  @return Return the status.
 *
 ******************************************************************************/
tEtaStatus
EtaCspBuckAllSetWait(tBuckWait4Stable iWaitForStable)
{
    tBuckState      sregNextState;

    //
    // Determine if there are more to updates to make. If not, just return success
    //
    if (sregBuckShadow.bAoValid && sregBuckShadow.bM3Valid &&
        sregBuckShadow.bDspValid && sregBuckShadow.bMemValid) {

        // Clear this flag so that future function calls will validate targets
        sregBuckShadow.bTargetsQualified = false;

        if (iWaitForStable == eBuckStableWait) {
            while (!EtaCspBuckStable());
        }

        return (eEtaSuccess);
    }

    //
    // Determine next state
    //
    sregNextState = BuckNextState();


    switch (sregNextState) {
        case eBuckSetAo: {
            EtaCspBuckAoVoltageSet(sregBuckShadow.sregAoVoltageTarget, true);            // FIXME - consider doing this conditionally only when AO->0.6V
            break;
        }
        default: // pick one
        case eBuckSetM3: {
            if (sregBuckShadow.sregM3VoltageTarget == eBuckM3FrequencyMode) {
                EtaCspBuckM3FrequencySet(sregBuckShadow.ui32M3FrequencyTarget, false);
            } else {
                EtaCspBuckM3VoltageSet(sregBuckShadow.sregM3VoltageTarget, false);
            }
            break;
        }
        case eBuckSetM3Intermediate: {
            if (sregBuckShadow.sregM3VoltageTarget == eBuckM3FrequencyMode) {
                EtaCspBuckM3FrequencySet(ETA_BUCK_AO_0P7V_MEM_0P9V_MIN_FREQ, false);
            } else {
                EtaCspBuckM3VoltageSet(eBuckM3Voltage700Mv, false);
            }
            // Invalidate the M3 to force the second transition later
            sregBuckShadow.bM3Valid = false;
            break;
        }
        case eBuckSetDsp: {
            if (sregBuckShadow.sregDspVoltageTarget == eBuckDspFrequencyMode) {
                EtaCspBuckDspFrequencySet(sregBuckShadow.ui32DspFrequencyTarget, false);
            } else {
                EtaCspBuckDspVoltageSet(sregBuckShadow.sregDspVoltageTarget, false);
            }
            break;
        }
        case eBuckSetDspIntermediate: {
            if (sregBuckShadow.sregDspVoltageTarget == eBuckDspFrequencyMode) {
                EtaCspBuckDspFrequencySet(ETA_BUCK_AO_0P7V_MEM_0P9V_MIN_FREQ, false);
            } else {
                EtaCspBuckDspVoltageSet(eBuckDspVoltage700Mv, false);
            }
            // Invalidate the DSP to force the second transition later
            sregBuckShadow.bDspValid = false;
            break;
        }
        case eBuckSetMem: {
            EtaCspBuckMemVoltageSet(sregBuckShadow.sregMemVoltageTarget, false);
            break;
        }
    }

    //
    // Tell the calling function to come back until done
    //
    return (eEtaBuckCallBackLater);
}


/*******************************************************************************
 *
 *  EtaCspBuckAllSetFinish - This function is only called by
 *  EtaCspBuckAllSet when the iAllAtOnce input is set. This
 *  function will step through programming all Bucks until all
 *  changes are complete. It will then wait for the stability
 *  counter to expire indicating the entire system is stable.
 *
 *  @return Return the status.
 *
 ******************************************************************************/
static tEtaStatus
EtaCspBuckAllSetFinish(void)
{
    tBuckState      sregNextState;
    bool            bPendingChange;

    //
    // Determine if further programming steps are needed
    //
    bPendingChange = !sregBuckShadow.bAoValid  || !sregBuckShadow.bM3Valid ||
                     !sregBuckShadow.bDspValid || !sregBuckShadow.bMemValid;

    //
    // Loop through any next programming steps
    //
    while (bPendingChange) {
        //
        // Determine next state
        //
        sregNextState = BuckNextState();


        switch (sregNextState) {
            case eBuckSetAo: {
                EtaCspBuckAoVoltageSet(sregBuckShadow.sregAoVoltageTarget, true);       // FIXME - consider doing this conditionally only when AO->0.6V
                break;
            }
            default: // pick one
            case eBuckSetM3: {
                if (sregBuckShadow.sregM3VoltageTarget == eBuckM3FrequencyMode) {
                    EtaCspBuckM3FrequencySet(sregBuckShadow.ui32M3FrequencyTarget, false);
                } else {
                    EtaCspBuckM3VoltageSet(sregBuckShadow.sregM3VoltageTarget, false);
                }
                break;
            }
            case eBuckSetM3Intermediate: {
                if (sregBuckShadow.sregM3VoltageTarget == eBuckM3FrequencyMode) {
                    EtaCspBuckM3FrequencySet(ETA_BUCK_AO_0P7V_MEM_0P9V_MIN_FREQ, false);
                } else {
                    EtaCspBuckM3VoltageSet(eBuckM3Voltage700Mv, false);
                }
                // Invalidate the M3 to force the second transition later
                sregBuckShadow.bM3Valid = false;
                break;
            }
            case eBuckSetDsp: {
                if (sregBuckShadow.sregDspVoltageTarget == eBuckDspFrequencyMode) {
                    EtaCspBuckDspFrequencySet(sregBuckShadow.ui32DspFrequencyTarget, false);
                } else {
                    EtaCspBuckDspVoltageSet(sregBuckShadow.sregDspVoltageTarget, false);
                }
                break;
            }
            case eBuckSetDspIntermediate: {
                if (sregBuckShadow.sregDspVoltageTarget == eBuckDspFrequencyMode) {
                    EtaCspBuckDspFrequencySet(ETA_BUCK_AO_0P7V_MEM_0P9V_MIN_FREQ, false);
                } else {
                    EtaCspBuckDspVoltageSet(eBuckDspVoltage700Mv, false);
                }
                // Invalidate the DSP to force the second transition later
                sregBuckShadow.bDspValid = false;
                break;
            }
            case eBuckSetMem: {
                EtaCspBuckMemVoltageSet(sregBuckShadow.sregMemVoltageTarget, false);
                break;
            }
        }

        //
        // Are we done yet?
        //
        bPendingChange = !sregBuckShadow.bAoValid  || !sregBuckShadow.bM3Valid ||
                         !sregBuckShadow.bDspValid || !sregBuckShadow.bMemValid;

    }

    //
    // Now that we are done clear this bit so that future function calls
    // will correctly qualify the targets
    //
    sregBuckShadow.bTargetsQualified = false;

    //
    // All programming complete, so wait for all Bucks to be stable
    //
    while (!EtaCspBuckStable());

    //
    // Do a retune of the HFO also
    //
    // EtaCspHfoTuneDefaultUpdate();

    //
    // All done, so let's finish up
    //
    return (eEtaSuccess);
}


/***************************************************************************//**
 *
 *  EtaCspBuckStable - Report when the Bucks have reached a
 *  stable state after programming.
 *
 *  @return Return true if the Bucks are stable.
 *
 ******************************************************************************/
bool
EtaCspBuckStable(void)
{
    return ((bool)(REG_RTC_PMIC_M3_CSR.BF.STABLE));
}


/***************************************************************************//**
 *
 *  EtaCspBuckHfClkSrcSelect - Select the Oscillator source
 *  of the high frequency reference clock (HFO or VHFO)
 *
 *  @param iSelect - 1 == VHFO, 0 == HFO
 ******************************************************************************/
void
EtaCspBuckHfClkSrcSelect(tBuckHfClkSrc iSource)
{
    //
    // Capture the state of the clock source to allow correct calculation
    // of the frequency target when in frequency mode.
    //
    sregBuckShadow.sregHfClockSource = iSource;

    //
    // Write the control bit appropriately
    //
    if(iSource == eBuckHfClockSourceVhfo)
    {
        REG_RTC_OSC_CSR.BF.BUCK_HF_CLK_SRC = 1;
    }
    else
    {
        REG_RTC_OSC_CSR.BF.BUCK_HF_CLK_SRC = 0;
    }
}

