/***************************************************************************//**
 *
 * @file eta_csp_hfo.c
 *
 * @brief This file contains eta_csp_hfo module implementations.
 *
 *   - start_hfo
 *   - stop_hfo
 *   - initial tunning of HFO against 32KHz
 *   - update tunning of HFO against 32KHz
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
 * @addtogroup ecm3532hfo-m3 High Frequency Oscillator (HFO)
 * @ingroup ecm3532csp-m3
 * @{
 ******************************************************************************/

#include <stdint.h>
#include "eta_status.h"
#include "eta_chip.h"
#include "eta_csp.h"
#include "eta_csp_hfo.h"
#include "eta_chip.h"
#include "memio.h"

/***************************************************************************//**
 *
 *  FreqGet - Get frequency after averaging over N samples.
 *
 *  @param ui32NumSamples is the number of samples to average over.
 *  @return Return the computed frequency.
 *
 ******************************************************************************/
static uint32_t
FreqGet(uint32_t ui32NumSamples)
{
    uint32_t ui32StartFlag, ui32Count, ui32I;

    ui32Count = 0;

    // Wait for start of measurement period
    ui32StartFlag = REG_GPIO_HFO_FREQ.BF.FLAG;
    while(ui32StartFlag == REG_GPIO_HFO_FREQ.BF.FLAG) { /* wait */ };

    for( ui32I=0; ui32I<ui32NumSamples; ui32I++) {
        // Wait for measurement period to complete
        while(ui32StartFlag != REG_GPIO_HFO_FREQ.BF.FLAG) { /* wait */ };
        ui32StartFlag = !ui32StartFlag;

        // Collect frequency measurement
        ui32Count += MEMIO16(REG_GPIO_HFO_FREQ_ADDR) & BM_GPIO_HFO_FREQ_COUNT;
    }

    // Return the frequency.
    return(ui32Count / ui32NumSamples);
}

/***************************************************************************//**
 *
 *  TrimSet - Set the HFO trim.
 *
 *  @param ui8Trim is the desired trim setting.
 *
 ******************************************************************************/
static void
TrimSet(uint8_t ui8Trim, uint32_t ui32Stop)
{
    if (ui32Stop) EtaCspHfoStop();
    REG_RTC_OSC_CSR.BF.HFO_TRIM = ui8Trim;
    if (ui32Stop) EtaCspHfoStart();
    MEMIO8(REG_RTC_DEBUG0_READ_ONLY_ADDR);  // BARRIER
}

/***************************************************************************//**
 *
 *  TrimGet - Get the current HFO trim value.
 *
 *  @return The HFO trim value.
 *
 ******************************************************************************/
static uint8_t
TrimGet(void)
{
    //
    // Get and return the current HFO trim value.
    //
    return(REG_RTC_OSC_CSR.BF.HFO_TRIM);
}

/***************************************************************************//**
 *
 *  HfoBinCoarseTune - Coarse tune HFO and return its frequency.
 *
 *  @param ui32Window The +/- tolerance for success.
 *  @param ui32NumSamples How many samples to average for getting frequency.
 *  @param ui32Timeout Number of calibration cycles after which the algorithm
 *                     gives up.
 *  @return The HFO frequency.
 *
 ******************************************************************************/
static uint32_t
HfoBinCoarseTune(uint32_t ui32Window,
                 uint32_t ui32NumSamples,
                 uint32_t ui32Timeout)
{
    uint32_t ui32Count     = 0;
    uint32_t ui32HfoFreq   = 0;
    uint8_t ui8TrimMax     = 0xFF >> 2;
    uint8_t ui8TrimMin     = 0;
    uint8_t ui8TrimMean    = 0x80 >> 2;
    uint32_t ui32UpWindow  = (244 + ui32Window);
    uint32_t ui32LowWindow = (244 - ui32Window);

    //
    // Write new trim and get new frequency.
    //
    TrimSet((ui8TrimMean << 2), 0);
    ui32HfoFreq = FreqGet(ui32NumSamples);

    //
    // Use a binary search strategy to set trim until the frequency within
    // ui32Window.
    //
    while((ui32HfoFreq > ui32UpWindow) | (ui32HfoFreq < ui32LowWindow))
    {
        if(ui32HfoFreq > ui32UpWindow)
        {
            ui8TrimMin = ui8TrimMean;
        }
        else
        {
            ui8TrimMax = ui8TrimMean;
        }
        ui8TrimMean = ((ui8TrimMax + ui8TrimMin) >> 1); // div by 2

        //
        // Write new trim and get new frequency.
        //
        TrimSet((ui8TrimMean << 2), 0);
        ui32HfoFreq = FreqGet(ui32NumSamples);

        //
        // Stop if maximum number of loop iterations.
        //
        if(++ui32Count >= ui32Timeout)
        {
            break;
        }
    }

    //
    // Return the HFO frequency.
    //
    return(ui32HfoFreq);
}

/***************************************************************************//**
 *
 *  HfoLinFineTune - Fine tune HFO and return its frequency.
 *
 *  @param ui32Window The +/- tolerance for success.
 *  @param ui32NumSamples How many samples to average for getting frequency.
 *  @param ui32Timeout Number of calibration cycles after which the algorithm
 *                     gives up.
 *  @param ui32Check Clock gates the HFO before each trim adjustment
 *                     gives up.
 *  @return Return its frequency.
 *
 ******************************************************************************/
static uint32_t
HfoLinFineTune(uint32_t ui32Window, uint32_t ui32NumSamples,
               uint32_t ui32Timeout, uint32_t ui32Check)
{
    uint32_t ui32Count = 0;
    uint8_t  ui8Trim = TrimGet();
    uint32_t ui32HfoFreq = FreqGet(ui32NumSamples);
    uint32_t ui32UpWindow = (244 + ui32Window);
    uint32_t ui32LowWindow = (244 - ui32Window);

    // We don't want to be within values between 80 and 128
    if ((ui8Trim < 128) && (ui8Trim > 80))
    {
        ui8Trim += 48;
        TrimSet(ui8Trim, ui32Check);
        ui32HfoFreq = FreqGet(ui32NumSamples);
    }

    //
    // Adjust trim in single steps until frequency is within ui32Window.
    //
    while((ui32HfoFreq > ui32UpWindow) | (ui32HfoFreq < ui32LowWindow))
    {
        //
        // Be sure to check for overflow/underflow.
        //
        if(ui32HfoFreq > 244)
        {
            if(ui8Trim != 0xFF)
            {
                ui8Trim += ((ui8Trim & 0x3) == 0x1) + 1;
            }
        }
        else
        {
            if(ui8Trim != 0x00)
            {
                ui8Trim -= ((ui8Trim & 0x3) == 0x2) + 1;
            }
        }

        // We don't want to be within values between 80 and 128
        if ((ui8Trim < 128) && (ui8Trim > 80))
        {
            ui8Trim += 48;
        }

        //
        // Write new trim and get new frequency.
        //
        TrimSet(ui8Trim, ui32Check);
        ui32HfoFreq = FreqGet(ui32NumSamples);

        //
        // Stop if maximum number of loop iterations.
        //
        if(++ui32Count >= ui32Timeout)
        {
            break;
        }
    }

    //
    // Return the HFO frequency.
    //
    return(ui32HfoFreq);
}

/***************************************************************************//**
 *
 *  EtaCspHfoStart - Start the HFO.
 *
 ******************************************************************************/
void
EtaCspHfoStart(void)
{
    //
    // Disable clock gate the HFO to the periph block.
    //
    REG_RTC_OSC_CSR.BF.HFO_CLK_GATE = 0;
}

/***************************************************************************//**
 *
 *  EtaCspHfoStop - Stop the HFO.
 *
 ******************************************************************************/
void
EtaCspHfoStop(void)
{
    //
    // Clock gate the HFO to the periph block.
    //
    REG_RTC_OSC_CSR.BF.HFO_CLK_GATE = 1;
}

/***************************************************************************//**
 *
 *  EtaCspHfoTuneCheck - Tuning algorithm to be used periodically for checking
 *                       the tuning.  Uses fine tuning only.
 *
 *  @param ui32Window The +/- tolerance for success.
 *  @param ui32NumSamples How many samples to average for getting frequency.
 *  @param ui32Timeout Number of calibration cycles after which the algorithm
 *                     gives up.
 *  @return Return the HFO frequency.
 *
 ******************************************************************************/
uint32_t
EtaCspHfoTuneCheck(uint32_t ui32Window, uint32_t ui32NumSamples,
                   uint32_t ui32Timeout)
{
    return HfoLinFineTune(ui32Window, ui32NumSamples, ui32Timeout, 1);
}

/***************************************************************************//**
 *
 *  EtaCspHfoTuneInit - Tuning algorithm used for cold start (POR or RESET_N).
 *                      Uses coarse and fine tuning.  Use this one at POR,
 *                      RESET_N or anytime the AO BUCK voltage is changed.
 *
 *  @param ui32CoarseWindow The +/- tolerance for coarse tuning success.
 *  @param ui32FineWindow The +/- tolerance for fine tuning success.
 *  @param ui32CoarseSamples How many samples to average for getting frequency
 *                           for coarse tuning.
 *  @param ui32FineSamples How many samples to average for getting frequency for
 *                         fine tuning.
 *  @param ui32CoarseTimeout Number of calibration cycles after which the
 *                           algorithm gives up for coarse tuning.
 *  @param ui32FineTimeout Number of calibration cycles after which the
 *                         algorithm gives up for coarse tuning.
 *  @return Return the frequency.
 *
 ******************************************************************************/
uint32_t
EtaCspHfoTuneInit(uint32_t ui32CoarseWindow, uint32_t ui32FineWindow,
                  uint32_t ui32CoarseSamples, uint32_t ui32FineSamples,
                  uint32_t ui32CoarseTimeout, uint32_t ui32FineTimeout)
{
    uint32_t ui32Freq;

    //
    // First thing we clock gate the HFO.
    //
    EtaCspHfoStop();

    //
    // Coarse tune the HFO using a binary search algorithm.
    //
    HfoBinCoarseTune(ui32CoarseWindow, ui32CoarseSamples, ui32CoarseTimeout);

    //
    // Fine tune the HFO using a linear search algorithm with more samples.
    //
    HfoLinFineTune(ui32FineWindow, ui32FineSamples, ui32FineTimeout, 0);

    //
    // Check fine tuning using a large number of samples.
    //
    ui32Freq = HfoLinFineTune(ui32FineWindow, 128, 2, 0);

    //
    // Now that we are done calibrating, ungate the HFO.
    //
    EtaCspHfoStart();

    //
    // Check for voltage droop after ungating the HFO
    //
    ui32Freq = EtaCspHfoTuneCheck(ui32FineWindow, ui32FineSamples, ui32FineTimeout);

    //
    // Return the frequency.
    //
    return(ui32Freq);
}

/***************************************************************************//**
 *
 *  EtaCspHfoTuneUpdate - Tuning algorithm used for warm starts. Only uses fine
 *  tuning.  Can be used infrequently to retune the HFO over time/temperature.
 *
 *  @param ui32Window The +/- tolerance for success
 *  @param ui32NumSamples How many samples to average for getting frequency
 *  @param ui32Timeout Number of calibration cycles after which the algorithm
 *                     gives up.
 *  @return Return the frequency.
 *
 ******************************************************************************/
uint32_t
EtaCspHfoTuneUpdate(uint32_t ui32Window, uint32_t ui32NumSamples,
                    uint32_t ui32Timeout)
{
    uint32_t ui32Freq;

    //
    // Check HFO tuning
    //
    HfoLinFineTune(ui32Window, ui32NumSamples, ui32Timeout, 1);

    //
    // Check fine tuning using a large number of samples.
    //
    ui32Freq = HfoLinFineTune(ui32Window, 128, 2, 1);

    //
    // Return the frequency.
    //
    return(ui32Freq);
}

/***************************************************************************//**
 *
 *  EtaCspHfoTuneDefaultInit - Tune based on the defaults.
 *
 *  @return Return the frequency.
 *
 ******************************************************************************/
uint32_t
EtaCspHfoTuneDefaultInit(void)
{
    return(EtaCspHfoTuneInit(1, 1, 1, 32, 8, 8));
}

/***************************************************************************//**
 *
 *  EtaCspHfoTuneDefaultUpdate - Update the tune.
 *
 *  @return Return the frequency.
 *
 ******************************************************************************/
uint32_t
EtaCspHfoTuneDefaultUpdate(void)
{
    return(EtaCspHfoTuneUpdate(1, 32, 156));
}

