/***************************************************************************//**
 *
 * @file eta_csp_adc.c
 *
 * @brief This file contains eta_csp_adc module implementations.
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
 * @addtogroup ecm3532adc-m3 Analog-to-Digital Converter (ADC)
 * @ingroup ecm3532csp-m3
 * @{
 ******************************************************************************/

#include <stdint.h>
#include "eta_status.h"
#include "eta_chip.h"
#include "eta_csp_adc.h"
#include "reg.h"
#include "memio.h"

/***************************************************************************//**
 *
 *  EtaCspAdcInit - Initialize the ADC.
 *
 *  @param ui32ClkDiv is the clock divide.
 *  @param ui8SampleTimeUs the sample time in microseconds.  Must not be less
 *                         than ADC_MIN_SAMPLE_TIME_US.
 *  @param iGrabDelay grab delay
 *  @param iDspDmaEn enable the DSP DMA
 *  @param iPeriodicEn periodic mode
 *  @param iIntEn enable the interrupt
 *  @return Return the status.
 *
 ******************************************************************************/
tEtaStatus
EtaCspAdcInit(uint32_t ui32ClkDiv, uint8_t ui8SampleTimeUs,
              tAdcGrabDelay iGrabDelay, bool iDspDmaEn, bool iPeriodicEn,
              bool iIntEn)
{
    tEtaStatus iStatus = eEtaSuccess;

    if((ui32ClkDiv >
        REG_FMAX(ADC_CLOCK_DIV,
                 VALUE)) || (ui8SampleTimeUs < ADC_MIN_SAMPLE_TIME_US))
    {
        iStatus = eEtaParameterOutOfRange;
    }
    else
    {
        REG_W1(ADC_CLOCK_DIV, VALUE, ui32ClkDiv);

        REG_M5(ADC_CNTRL, CAPTURE_CNT, ui8SampleTimeUs,
               GRAB_DELAY, iGrabDelay,
               DMA, iDspDmaEn,
               PERIODIC, iPeriodicEn,
               INT_EN, iIntEn);
    }

    //
    // Return the status.
    //
    return(iStatus);
}

/***************************************************************************//**
 *
 *  EtaCspAdcSoftReset - Perform a soft reset on the ADC.
 *
 ******************************************************************************/
void
EtaCspAdcSoftReset(void)
{
    REG_S1(ADC_CNTRL, SFTRST);
    REG_C1(ADC_CNTRL, MODE);
}

/***************************************************************************//**
 *
 *  EtaCspAdcDisable - Disable the ADC.
 *
 ******************************************************************************/
void
EtaCspAdcDisable(void)
{
    REG_C1(ADC_CNTRL, MODE);
}

/***************************************************************************//**
 *
 *  EtaCspAdcEnable - Enable the ADC.
 *
 ******************************************************************************/
void
EtaCspAdcEnable(void)
{
    REG_S1(ADC_CNTRL, MODE);
}

/***************************************************************************//**
 *
 *  EtaCspAdcFifoInit - Initialize the FIFO.
 *
 *  @param iIntLevel the interrupt level.
 *  @param iClear is the clear bit.
 *  @param iIntEn is the interrupt enable bit.
 *  @param iFifoEn is the FIFO enable bit.
 *
 ******************************************************************************/
void
EtaCspAdcFifoInit(tAdcFifoIntLevel iIntLevel, bool iClear, bool iIntEn,
                  bool iFifoEn)
{
    REG_W4(ADC_SFIFO_CFG, FULL, iIntLevel, CLR, iClear, INT_EN, iIntEn, ENABLE,
           iFifoEn);
}

/***************************************************************************//**
 *
 *  EtaCspAdcFifoDisable - Disable the FIFO.
 *
 ******************************************************************************/
void
EtaCspAdcFifoDisable(void)
{
    REG_C1(ADC_SFIFO_CFG, ENABLE);
}

/***************************************************************************//**
 *
 *  EtaCspAdcFifoEnable - Enable the FIFO.
 *
 ******************************************************************************/
void
EtaCspAdcFifoEnable(void)
{
    REG_S1(ADC_SFIFO_CFG, ENABLE);
}

/***************************************************************************//**
 *
 *  EtaCspAdcFifoIntDisable - Disable the FIFO interrupt.
 *
 ******************************************************************************/
void
EtaCspAdcFifoIntDisable(void)
{
    REG_C1(ADC_SFIFO_CFG, INT_EN);
}

/***************************************************************************//**
 *
 *  EtaCspAdcFifoIntEnable - Enable the FIFO interrupt.
 *
 ******************************************************************************/
void
EtaCspAdcFifoIntEnable()
{
    REG_S1(ADC_SFIFO_CFG, INT_EN);
}

/***************************************************************************//**
 *
 *  EtaCspAdcFifoIntEnable - Enable the FIFO interrupt.
 *
 *  @param iIntLevel is the desired level to interrupt on.
 *
 ******************************************************************************/
extern void EtaCspAdcFifoIntLevelSet(tAdcFifoIntLevel iIntLevel)
{
    REG_M1(ADC_SFIFO_CFG, FULL, iIntLevel);
}

/***************************************************************************//**
 *
 *  EtaCspAdcFifoClear - Clear the FIFO.
 *
 ******************************************************************************/
void
EtaCspAdcFifoClear(void)
{
    REG_S1(ADC_SFIFO_CFG, CLR);
}

/***************************************************************************//**
 *
 *  EtaCspAdcFifoIntLevelGet - Get the FIFO interrupt level.
 *
 *  @return Return the FIFO interrupt level.
 *
 ******************************************************************************/
uint32_t
EtaCspAdcFifoIntLevelGet(void)
{
    return(REG_R(ADC_SFIFO_CFG, FULL));
}

///***************************************************************************//**
// *
// *  EtaCspAdcFifoDataRead - Read the FIFO data.
// *
// *  @param pui16SampleData pointer to store the data.
// *  @param ui32NumSamples is the number of samples to read.
// *  @param pui32NumRead is a pointer to store the number of samples read.
// *  @return Return the status.
// *
// ******************************************************************************/
//tEtaStatus
//EtaCspAdcFifoDataRead(uint16_t *pui16SampleData,
//                      uint32_t ui32NumSamples,
//                      uint32_t *pui32NumRead)
//{
//    reg_adc_sfifo_t uFifo;
//    tEtaStatus iStatus = eEtaSuccess;
//
//    *pui32NumRead = 0;
//
//    //
//    // Initiate first four non-blocking reads of the ADC_SFIFO.
//    //
//    REG_S1(GPIO_SPEC_READ_STAT, SPEC_A);
//    MEMIO32(REG_ADC_SFIFO_ADDR | 0x400000);
//
//    while((ui32NumSamples > 0) && (iStatus == eEtaSuccess))
//    {
//        //
//        // Wait for speculative read values to be ready.
//        //
//        while(REG_R(GPIO_SPEC_READ_STAT, SPEC_A) != 0x1)
//        {
//        }
//        uFifo = *(volatile reg_adc_sfifo_t *)REG_GPIO_SPEC_READ_A_ADDR;
//        REG_W1(GPIO_SPEC_READ_STAT, SPEC_A, 1);
//        MEMIO32(REG_ADC_SFIFO_ADDR | 0x400000);
//
//        //
//        // Check for various errors.
//        //
//        if(uFifo.BF.LOST == TRUE)
//        {
//            iStatus = eEtaAdcFifoOverflow;
//            break;
//        }
//        else if(uFifo.BF.VALID0 == 0)
//        {
//            iStatus = eEtaAdcSampleInvalid;
//            break;
//        }
//        else if(uFifo.BF.OFLOW0)
//        {
//            iStatus = eEtaAdcSampleOverflow;
//        }
//        else if(uFifo.BF.VALID1 == 0)
//        {
//            ui32NumSamples = 1;
//        }
//        else if(uFifo.BF.OFLOW1)
//        {
//            //mei FIXME  ui32NumSamples = 1;
//            iStatus = eEtaAdcSampleOverflow;
//        }
//
//        if(ui32NumSamples == 1)
//        {
//            *pui16SampleData++ = uFifo.BF.SAMP0;
//            ui32NumSamples -= 1;
//            *pui32NumRead += 1;
//        }
//        else
//        {
//            *pui16SampleData++ = uFifo.BF.SAMP0;
//            *pui16SampleData++ = uFifo.BF.SAMP1;
//            ui32NumSamples -= 2;
//            *pui32NumRead += 2;
//        }
//    }
//
//    //
//    // Clear ADC interrupt status.
//    //
//    REG_S1(ADC_INT_STAT, READY);
//
//    //
//    // Fence the interrupt clearing write.
//    //
//    EtaCspAdcFenceFast();
//
//    //
//    // Return the status.
//    //
//    return(iStatus);
//}

/***************************************************************************//**
 *
 *  EtaCspAdcFifoDataRead - Read the FIFO data.
 *
 *  @param pui16SampleData pointer to store the data.
 *  @param ui32NumSamples is the desired number of samples to read.
 *  @param pui32NumRead is a pointer to store the number of valid amples read.
 *  @return Return the status.
 *
 *  @Note pui32NumRead is the number of valid data read back, before an invalid 
 *        encoutner and regardless of overflow status.  
 *
 ******************************************************************************/
tEtaStatus
EtaCspAdcFifoDataRead(uint16_t *pui16SampleData,
                      uint32_t ui32NumSamples,
                      uint32_t *pui32NumRead)
{
    tEtaStatus iStatus = eEtaSuccess;
    uint16_t ui16ReadSample; 
    uint32_t ui32ReadSample; 

    *pui32NumRead = 0;

    while((ui32NumSamples > 0) && (iStatus == eEtaSuccess))
    {
        if(ui32NumSamples == 1)
        {
            ui16ReadSample = MEMIO16(REG_ADC_SFIFO_ADDR);
            //
            // Check for various errors.
            //
            if(ui16ReadSample & BM_ADC_SFIFO_LOST)
            {
                iStatus = eEtaAdcFifoOverflow;
                break;
            }
            else if((ui16ReadSample & BM_ADC_SFIFO_VALID0) == FALSE)
            {
                iStatus = eEtaAdcSampleInvalid;
                break;
            }
            else if(ui16ReadSample & BM_ADC_SFIFO_OFLOW0)
            {
                iStatus = eEtaAdcSampleOverflow;
                //break;
            }
            *pui16SampleData++ = (ui16ReadSample & BM_ADC_SFIFO_SAMP0) >> BP_ADC_SFIFO_SAMP0; 
            ui32NumSamples -= 1;
            *pui32NumRead += 1;
        }
        else
        {
            ui32ReadSample = MEMIO32(REG_ADC_SFIFO_ADDR);
            //
            // Check for various errors.
            //
            if(ui32ReadSample & BM_ADC_SFIFO_LOST)
            {
                iStatus = eEtaAdcFifoOverflow;
                break;
            }
            else if((ui32ReadSample & BM_ADC_SFIFO_VALID0) == FALSE)
            {
                iStatus = eEtaAdcSampleInvalid;
                break;
            }
            else if(ui32ReadSample & BM_ADC_SFIFO_OFLOW0)
            {
                iStatus = eEtaAdcSampleOverflow;
                //break;
            }
            *pui16SampleData++ = (ui32ReadSample & BM_ADC_SFIFO_SAMP0) >> BP_ADC_SFIFO_SAMP0; 
            ui32NumSamples -- ;
            *pui32NumRead += 1;

            if((ui32ReadSample & BM_ADC_SFIFO_VALID1) == FALSE)
            {
                iStatus = eEtaAdcSampleInvalid;
                break;
            }
            else if(ui32ReadSample & BM_ADC_SFIFO_OFLOW1)
            {
                iStatus = eEtaAdcSampleOverflow;
                //break;
            }
            *pui16SampleData++ = (ui32ReadSample & BM_ADC_SFIFO_SAMP1) >> BP_ADC_SFIFO_SAMP1; 
            ui32NumSamples -- ;
            *pui32NumRead += 1;
        }
    }

    //
    // Clear ADC interrupt status.
    //
    REG_S1(ADC_INT_STAT, READY);

    //
    // Fence the interrupt clearing write.
    //
    EtaCspAdcFenceFast();

    //
    // Return the status.
    //
    return(iStatus);
}

/***************************************************************************//**
 *
 *  EtaCspAdcFifoStatusGet - Get the FIFO status.
 *
 *  @return Return the FIFO status.
 *
 ******************************************************************************/
reg_adc_sfifo_stat_t
EtaCspAdcFifoStatusGet(void)
{
    return(REG_ADC_SFIFO_STAT);
}

/***************************************************************************//**
 *
 *  EtaCspAdcFifoUsageGet - Get the FIFO usage.
 *
 *  @return Return the FIFO usage.
 *
 ******************************************************************************/
uint32_t
EtaCspAdcFifoUsageGet(void)
{
    reg_adc_sfifo_stat_t uStatus = EtaCspAdcFifoStatusGet();

    return(uStatus.BF.SIZE | (uStatus.BF.FULL << 4));
}

/***************************************************************************//**
 *
 *  EtaCspAdcFifoLowGet - Get the FIFO low status.
 *
 *  @return Return the FIFO low status.
 *
 ******************************************************************************/
bool
EtaCspAdcFifoLowGet(void)
{
    reg_adc_sfifo_stat_t uStatus = EtaCspAdcFifoStatusGet();

    return(uStatus.BF.LOW);
}

/***************************************************************************//**
 *
 *  EtaCspAdcFifoFullGet - Get the FIFO full status.
 *
 *  @return Return the FIFO full status.
 *
 ******************************************************************************/
bool
EtaCspAdcFifoFullGet(void)
{
    reg_adc_sfifo_stat_t uStatus = EtaCspAdcFifoStatusGet();

    return(uStatus.BF.FULL);
}

/***************************************************************************//**
 *
 *  EtaCspAdcFifoEmptyGet - Get the FIFO empty status.
 *
 *  @return Return the FIFO empty status.
 *
 ******************************************************************************/
bool
EtaCspAdcFifoEmptyGet(void)
{
    reg_adc_sfifo_stat_t uStatus = EtaCspAdcFifoStatusGet();

    return(uStatus.BF.EMPTY);
}

/***************************************************************************//**
 *
 *  EtaCspAdcSampleIntEnable - Enable the sample interrupt.
 *
 ******************************************************************************/
void
EtaCspAdcSampleIntEnable(void)
{
    REG_S1(ADC_CNTRL, INT_EN);
}

/***************************************************************************//**
 *
 *  EtaCspAdcSampleIntDisable - Disable the sample interrupt.
 *
 ******************************************************************************/
void
EtaCspAdcSampleIntDisable(void)
{
    REG_C1(ADC_CNTRL, INT_EN);
}

/***************************************************************************//**
 *
 *  EtaCspAdcCalModeEnable - Enable calibration mode.
 *
 ******************************************************************************/
void
EtaCspAdcCalModeEnable(void)
{
    REG_S1(ADC_CNTRL, CALIBRATE);
}

/***************************************************************************//**
 *
 *  EtaCspAdcCalModeDisable - Disable calibration mode.
 *
 ******************************************************************************/
void
EtaCspAdcCalModeDisable(void)
{
    REG_C1(ADC_CNTRL, CALIBRATE);
}

/***************************************************************************//**
 *
 *  EtaCspAdcChanSelect - Select the ADC channel.
 *
 *  @param iChan is the channel number.
 *  @param iChanType is the channel type.
 *
 ******************************************************************************/
void
EtaCspAdcChanSelect(tAdcChannel iChan, tAdcChannelType iChanType)
{
    REG_W2(ADC_SRC, SEL, iChan, DIFF, iChanType);
}

/***************************************************************************//**
 *
 *  EtaCspAdcIntStatusGet - Get the sample interrupt and the FIFO interrupt.
 *
 *  @return Return the ADC Int state.
 *
 ******************************************************************************/
reg_adc_int_stat_t
EtaCspAdcIntStatusGet(void)
{
    return(REG_ADC_INT_STAT);
}

/***************************************************************************//**
 *
 *  EtaCspAdcIntReadyGet - Get the sample interrupt.
 *
 *  @return Return the ADC Int READY state.
 *
 ******************************************************************************/
bool
EtaCspAdcIntReadyGet(void)
{
    reg_adc_int_stat_t uStatus = EtaCspAdcIntStatusGet();

    return(uStatus.BF.READY);
}

/***************************************************************************//**
 *
 *  EtaCspAdcIntFifoGet - Get the ADC FIFO interrupt.
 *
 *  @return Return the ADC Int FIFO state.
 *
 ******************************************************************************/
bool
EtaCspAdcIntFifoGet(void)
{
    reg_adc_int_stat_t uStatus = EtaCspAdcIntStatusGet();

    return(uStatus.BF.SFIFO);
}

/***************************************************************************//**
 *
 *  EtaCspAdcFifoIntClear - Clear the fifo interrupt.
 *
 ******************************************************************************/
void
EtaCspAdcFifoIntClear(void)
{
    REG_S1(ADC_INT_STAT, SFIFO);

    EtaCspAdcFenceFast();
}

/***************************************************************************//**
 *
 *  EtaCspAdcIntClear - Clear the sample interrupt and the FIFO interrupt.
 *
 ******************************************************************************/
void
EtaCspAdcIntClear(void)
{
    REG_W2(ADC_INT_STAT, READY, 1, SFIFO, 1);
}

/***************************************************************************//**
 *
 *  EtaCspAdcSampleIntClear - Clear the sample interrupt.
 *
 ******************************************************************************/
void
EtaCspAdcSampleIntClear(void)
{
    REG_S1(ADC_INT_STAT, READY);
}

/***************************************************************************//**
 *
 *  EtaCspAdcSampleDataRead - Read the sample data regardless of overflow bit.
 *
 *  @return Return the sample data.
 *  @note   This function doesn't provide overflow bit information.
 *
 ******************************************************************************/
uint16_t
EtaCspAdcSampleDataRead(void)
{
    uint16_t ui16SampleData = REG_R(ADC_SAMPLE, DATA);

    //
    // Clear ADC interrupt status.
    //
    REG_S1(ADC_INT_STAT, READY);

    //
    // Fence the interrupt clearing write.
    //
    EtaCspAdcFenceFast();

    //
    // Return the sample data.
    //
    return(ui16SampleData);
}

/***************************************************************************//**
 *
 *  EtaCspAdcSampleDataReadWithOflow - Read the sample data, with Overflow bit 
 *    returned as a status.
 *
 *  @param  pui16Read is a pointer to store the data read.
 *  @return Return eEtaSuccess if there is no overflow; else eEtaAdcSampleOverflow.
 *
 ******************************************************************************/
tEtaStatus
EtaCspAdcSampleDataReadWithOflow(uint16_t *pui16Read)
{
    reg_adc_sample_t ui16Sample;
    ui16Sample = *(volatile reg_adc_sample_t *)REG_ADC_SAMPLE_ADDR;

    *pui16Read = ui16Sample.BF.DATA;

    //
    // Clear ADC interrupt status.
    //
    REG_S1(ADC_INT_STAT, READY);

    //
    // Fence the interrupt clearing write.
    //
    EtaCspAdcFenceFast();

    //
    // Return the status.
    //
    if(ui16Sample.BF.OFLOW){
       return eEtaAdcSampleOverflow; 
    }
    return eEtaSuccess;
}

/***************************************************************************//**
 *
 *  EtaCspAdcStartPeriodicSampling - start ADC periodic conversions.
 *
 ******************************************************************************/
void
EtaCspAdcStartPeriodicSampling(void)
{
    REG_M1(ADC_CNTRL,PERIODIC,1); 
}

/***************************************************************************//**
 *
 *  EtaCspAdcStopPeriodicSampling - stop ADC periodic conversions.
 *
 ******************************************************************************/
void
EtaCspAdcStopPeriodicSampling(void)
{
    REG_C1(ADC_CNTRL, PERIODIC);
}

/***************************************************************************//**
 *
 *  EtaCspAdcTrigger - trigger one discrete ADC conversion.
 *
 ******************************************************************************/
void
EtaCspAdcTrigger(void)
{
    REG_W1(ADC_START, NOW, 1);
}

