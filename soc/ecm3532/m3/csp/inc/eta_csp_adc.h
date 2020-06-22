/***************************************************************************//**
 *
 * @file eta_csp_adc.h
 *
 * @brief This file contains eta_csp_adc module definitions.
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
 * @addtogroup ecm3532adc-m3 Analog-to-Digital Converter (ADC)
 * @ingroup ecm3532csp-m3
 * @{
 ******************************************************************************/

#ifndef __ETA_CSP_ADC_H__
#define __ETA_CSP_ADC_H__

#include <stdint.h>
#include "eta_status.h"
#include "eta_chip.h"
#include "reg.h"
#include "eta_csp_common_adc.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ADC_CLK_HZ       1000000U
#define ADC_CLK_DIV_1HZ  ADC_CLK_HZ
#define ADC_CLK_DIV_1KHZ (ADC_CLK_HZ / 1000U)

#define ADC_MIN_SAMPLE_TIME_US 4U
#define ETA_CSP_ADC_FIFO_SIZE (16)

//
//! ADC grab delay in clocks typedef.
//
typedef enum
{
    eAdcGrabDelayNone = 0,
    eAdcGrabDelay1Clk = 1,
    eAdcGrabDelay2Clk = 2,
    eAdcGrabDelay3Clk = 3,
}
tAdcGrabDelay;

//
//! ADC FIFO interrupt watermark typedef.
//
typedef enum
{
    eAdcFifoIntLevel1  = 0,
    eAdcFifoIntLevel2  = 1,
    eAdcFifoIntLevel3  = 2,
    eAdcFifoIntLevel4  = 3,
    eAdcFifoIntLevel5  = 4,
    eAdcFifoIntLevel6  = 5,
    eAdcFifoIntLevel7  = 6,
    eAdcFifoIntLevel8  = 7,
    eAdcFifoIntLevel9  = 8,
    eAdcFifoIntLevel10 = 9,
    eAdcFifoIntLevel11 = 10,
    eAdcFifoIntLevel12 = 11,
    eAdcFifoIntLevel13 = 12,
    eAdcFifoIntLevel14 = 13,
    eAdcFifoIntLevel15 = 14,
    eAdcFifoIntLevel16 = 15,
}
tAdcFifoIntLevel;

//
//! ADC channel number typedef.
//
typedef enum
{
    eAdcChannel0 = 0,
    eAdcChannel1 = 1,
    eAdcChannel2 = 2,
    eAdcChannel3 = 3,
}
tAdcChannel;

//
//! ADC channel type typedef.
//
typedef enum
{
    eAdcChannelTypeSingleEnded  = 0,
    eAdcChannelTypeDifferential = 1,
}
tAdcChannelType;

extern void EtaCspAdcSoftReset(void);
extern void EtaCspAdcDisable(void);
extern void EtaCspAdcEnable(void);
extern void EtaCspAdcStartPeriodicSampling(void);
extern void EtaCspAdcStopPeriodicSampling(void);
extern void EtaCspAdcTrigger(void);
//
// Initialize the ADC.
//
extern tEtaStatus EtaCspAdcInit(uint32_t ui32ClkDiv, uint8_t ui8SampleTimeUs,
                                tAdcGrabDelay iGrabDelay, bit_t iDspDmaEn,
                                bit_t iPeriodicEn, bit_t iIntEn);

//
// ADC FIFO
//
extern void EtaCspAdcFifoInit(tAdcFifoIntLevel iIntLevel, bit_t iClear,
                              bit_t iIntEn, bit_t iFifoEn);
extern void EtaCspAdcFifoDisable(void);
extern void EtaCspAdcFifoEnable(void);
extern void EtaCspAdcFifoIntDisable(void);
extern void EtaCspAdcFifoIntEnable(void);
extern void EtaCspAdcFifoIntLevelSet(tAdcFifoIntLevel iIntLevel);
extern uint32_t EtaCspAdcFifoIntLevelGet(void);
extern void EtaCspAdcFifoClear(void);
extern uint32_t EtaCspAdcFifoUsageGet(void);
extern bool EtaCspAdcFifoLowGet(void);
extern bool EtaCspAdcFifoFullGet(void);
extern bool EtaCspAdcFifoEmptyGet(void);
extern tEtaStatus EtaCspAdcFifoDataRead(uint16_t *pui16SampleData,
                                        uint32_t ui32NumSamples,
                                        uint32_t *pui32NumRead);

extern void EtaCspAdcSampleIntEnable(void);
extern void EtaCspAdcSampleIntDisable(void);
extern void EtaCspAdcSampleIntClear(void);
extern uint16_t EtaCspAdcSampleDataRead(void);

extern void EtaCspAdcCalModeEnable(void);
extern void EtaCspAdcCalModeDisable(void);
extern void EtaCspAdcChanSelect(tAdcChannel iChan, tAdcChannelType iChanType);
//
// ADC Interrupt
//
extern bool EtaCspAdcIntReadyGet(void);
extern bool EtaCspAdcIntFifoGet(void);
extern void EtaCspAdcIntClear(void);

#ifdef __cplusplus
}
#endif

#endif // __ETA_CSP_ADC_H__

