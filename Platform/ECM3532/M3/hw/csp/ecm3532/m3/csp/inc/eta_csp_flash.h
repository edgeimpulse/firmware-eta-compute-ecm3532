/***************************************************************************//**
 *
 * @file eta_csp_flash.h
 *
 * @brief This file contains eta_csp_flash module definitions and constants.
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
 * @addtogroup ecm3532flash-m3 Flash
 * @ingroup ecm3532csp-m3
 * @{
 ******************************************************************************/

#ifndef __ETA_CSP_FLASH_H__
#define __ETA_CSP_FLASH_H__

#include <stdint.h>
#include "eta_status.h"
#include "eta_chip.h"

#ifdef __cplusplus
extern "C" {
#endif

//
// General defines for the flash macro
//
#define ETA_CSP_FLASH_MAX               (0x01080000)
#define ETA_CSP_FLASH_BASE              (0x01000000)
#define ETA_CSP_FLASH_SIZE              (ETA_CSP_FLASH_MAX  - ETA_CSP_FLASH_BASE)
#define ETA_CSP_FLASH_PAGE_SIZE_BYTES   (4096)
#define ETA_CSP_FLASH_PAGE_SIZE_WORDS   (ETA_CSP_FLASH_PAGE_SIZE_BYTES >> 2)
#define ETA_CSP_FLASH_PAGE_SIZE_MASK    (0xfffff000)
#define ETA_CSP_FLASH_NUM_PAGES         ((ETA_CSP_FLASH_SIZE) / ETA_CSP_FLASH_PAGE_SIZE_BYTES)

//
// Flash Info Space defines
//
#define FLASH_INFO_MAGIC0_ADDRESS                   (0x0000)
#define FLASH_INFO_MAGIC0_OFFSET                    (FLASH_INFO_MAGIC0_ADDRESS >> 4)
#define FLASH_INFO_MAGIC0_INDEX                     ((FLASH_INFO_MAGIC0_ADDRESS & 0xC) >> 2)

#define FLASH_INFO_MAGIC1_ADDRESS                   (0x0004)
#define FLASH_INFO_MAGIC1_OFFSET                    (FLASH_INFO_MAGIC1_ADDRESS >> 4)
#define FLASH_INFO_MAGIC1_INDEX                     ((FLASH_INFO_MAGIC1_ADDRESS & 0xC) >> 2)

#define FLASH_INFO_MAGIC1_UNLOCK_ETA_MASK           (0x80000000)

#define FLASH_INFO_ETA_FLASH_RD_ENABLE_ADDRESS      (0x0008)
#define FLASH_INFO_ETA_FLASH_RD_ENABLE_OFFSET       (FLASH_INFO_ETA_FLASH_RD_ENABLE_ADDRESS >> 4)
#define FLASH_INFO_ETA_FLASH_RD_ENABLE_INDEX        ((FLASH_INFO_ETA_FLASH_RD_ENABLE_ADDRESS & 0xC) >> 2)

#define FLASH_INFO_ETA_FLASH_WRT_ENABLE_ADDRESS     (0x000C)
#define FLASH_INFO_ETA_FLASH_WRT_ENABLE_OFFSET      (FLASH_INFO_ETA_FLASH_WRT_ENABLE_ADDRESS >> 4)
#define FLASH_INFO_ETA_FLASH_WRT_ENABLE_INDEX       ((FLASH_INFO_ETA_FLASH_WRT_ENABLE_ADDRESS & 0xC) >> 2)

#define FLASH_INFO_MAGIC2_ADDRESS                   (0x0400)
#define FLASH_INFO_MAGIC2_OFFSET                    (FLASH_INFO_MAGIC2_ADDRESS >> 4)
#define FLASH_INFO_MAGIC2_INDEX                     ((FLASH_INFO_MAGIC2_ADDRESS & 0xC) >> 2)

#define FLASH_INFO_MAGIC3_ADDRESS                   (0x0404)
#define FLASH_INFO_MAGIC3_OFFSET                    (FLASH_INFO_MAGIC3_ADDRESS >> 4)
#define FLASH_INFO_MAGIC3_INDEX                     ((FLASH_INFO_MAGIC3_ADDRESS & 0xC) >> 2)

#define FLASH_INFO_MAGIC3_NOT_SECURE_BOOT_MASK      (0x08000000)
#define FLASH_INFO_MAGIC3_DBG_PRESERVE_FLASH_MASK   (0x10000000)
#define FLASH_INFO_MAGIC3_DBG_PRESERVE_SRAM_MASK    (0x20000000)
#define FLASH_INFO_MAGIC3_DBG_ENABLE_MASK           (0x40000000)
#define FLASH_INFO_MAGIC3_UNLOC_CUSTOMER_MASK       (0x80000000)

//
// Manufacturing Data
//

// Device ID = ascii "3532"
#define FLASH_INFO_ETA_DATA0_ADDRESS                (0x0200)
#define FLASH_INFO_ETA_DATA0_OFFSET                 (FLASH_INFO_ETA_DATA0_ADDRESS >> 4)
#define FLASH_INFO_ETA_DATA0_INDEX                  ((FLASH_INFO_ETA_DATA0_ADDRESS & 0xC) >> 2)

// { wafer_number[15:0], revision_number[15:0] }
#define FLASH_INFO_ETA_DATA1_ADDRESS                (0x0204)
#define FLASH_INFO_ETA_DATA1_OFFSET                 (FLASH_INFO_ETA_DATA1_ADDRESS >> 4)
#define FLASH_INFO_ETA_DATA1_INDEX                  ((FLASH_INFO_ETA_DATA1_ADDRESS & 0xC) >> 2)

// Assembly lot data0
#define FLASH_INFO_ETA_DATA2_ADDRESS                (0x0208)
#define FLASH_INFO_ETA_DATA2_OFFSET                 (FLASH_INFO_ETA_DATA2_ADDRESS >> 4)
#define FLASH_INFO_ETA_DATA2_INDEX                  ((FLASH_INFO_ETA_DATA2_ADDRESS & 0xC) >> 2)

// Assembly lot data1
#define FLASH_INFO_ETA_DATA3_ADDRESS                (0x020c)
#define FLASH_INFO_ETA_DATA3_OFFSET                 (FLASH_INFO_ETA_DATA3_ADDRESS >> 4)
#define FLASH_INFO_ETA_DATA3_INDEX                  ((FLASH_INFO_ETA_DATA3_ADDRESS & 0xC) >> 2)

// Assembly Unit Number
#define FLASH_INFO_ETA_DATA4_ADDRESS                (0x0210)
#define FLASH_INFO_ETA_DATA4_OFFSET                 (FLASH_INFO_ETA_DATA4_ADDRESS >> 4)
#define FLASH_INFO_ETA_DATA4_INDEX                  ((FLASH_INFO_ETA_DATA4_ADDRESS & 0xC) >> 2)

// TestID [31:0]
#define FLASH_INFO_ETA_DATA16_ADDRESS                (0x0240)
#define FLASH_INFO_ETA_DATA16_OFFSET                 (FLASH_INFO_ETA_DATA16_ADDRESS >> 4)
#define FLASH_INFO_ETA_DATA16_INDEX                  ((FLASH_INFO_ETA_DATA16_ADDRESS & 0xC) >> 2)

// Assembly ID[3:0]
#define FLASH_INFO_ETA_DATA32_ADDRESS                (0x0280)
#define FLASH_INFO_ETA_DATA32_OFFSET                 (FLASH_INFO_ETA_DATA32_ADDRESS >> 4)
#define FLASH_INFO_ETA_DATA32_INDEX                  ((FLASH_INFO_ETA_DATA32_ADDRESS & 0xC) >> 2)

// Assembly ID[7:4]
#define FLASH_INFO_ETA_DATA33_ADDRESS                (0x0284)
#define FLASH_INFO_ETA_DATA33_OFFSET                 (FLASH_INFO_ETA_DATA33_ADDRESS >> 4)
#define FLASH_INFO_ETA_DATA33_INDEX                  ((FLASH_INFO_ETA_DATA33_ADDRESS & 0xC) >> 2)

// Package Type
#define FLASH_INFO_ETA_DATA34_ADDRESS                (0x0288)
#define FLASH_INFO_ETA_DATA34_OFFSET                 (FLASH_INFO_ETA_DATA34_ADDRESS >> 4)
#define FLASH_INFO_ETA_DATA34_INDEX                  ((FLASH_INFO_ETA_DATA34_ADDRESS & 0xC) >> 2)

// Temperature Sensor recorded Frequency #1
#define FLASH_INFO_ETA_TSENSE_FREQUENCYA_ADDRESS     (0x02C0)
#define FLASH_INFO_ETA_TSENSE_FREQUENCYA_OFFSET      (FLASH_INFO_ETA_TSENSE_FREQUENCYA_ADDRESS >> 4)
#define FLASH_INFO_ETA_TSENSE_FREQUENCYA_INDEX       ((FLASH_INFO_ETA_TSENSE_FREQUENCYA_ADDRESS & 0xC) >> 2)

// Temperature Sensor recorded Temperature #1
#define FLASH_INFO_ETA_TSENSE_TEMPERATUREA_ADDRESS   (0x02C4)
#define FLASH_INFO_ETA_TSENSE_TEMPERATUREA_OFFSET    (FLASH_INFO_ETA_TSENSE_TEMPERATUREA_ADDRESS >> 4)
#define FLASH_INFO_ETA_TSENSE_TEMPERATUREA_INDEX     ((FLASH_INFO_ETA_TSENSE_TEMPERATUREA_ADDRESS & 0xC) >> 2)

// Temperature Sensor recorded Frequency #2
#define FLASH_INFO_ETA_TSENSE_FREQUENCYB_ADDRESS     (0x02C8)
#define FLASH_INFO_ETA_TSENSE_FREQUENCYB_OFFSET      (FLASH_INFO_ETA_TSENSE_FREQUENCYB_ADDRESS >> 4)
#define FLASH_INFO_ETA_TSENSE_FREQUENCYB_INDEX       ((FLASH_INFO_ETA_TSENSE_FREQUENCYB_ADDRESS & 0xC) >> 2)

// Temperature Sensor recorded Temperature #2
#define FLASH_INFO_ETA_TSENSE_TEMPERATUREB_ADDRESS   (0x02CC)
#define FLASH_INFO_ETA_TSENSE_TEMPERATUREB_OFFSET    (FLASH_INFO_ETA_TSENSE_TEMPERATUREB_ADDRESS >> 4)
#define FLASH_INFO_ETA_TSENSE_TEMPERATUREB_INDEX     ((FLASH_INFO_ETA_TSENSE_TEMPERATUREB_ADDRESS & 0xC) >> 2)

// ATE Lock
#define FLASH_INFO_ETA_ATE_LOCK_ADDRESS              (0x02d0)
#define FLASH_INFO_ETA_ATE_LOCK_OFFSET               (FLASH_INFO_ETA_ATE_LOCK_ADDRESS >> 4)
#define FLASH_INFO_ETA_ATE_LOCK_INDEX                ((FLASH_INFO_ETA_ATE_LOCK_ADDRESS & 0xC) >> 2)

// Temperature Sensor recorded Frequency #3 (RESERVED FOR FUTURE USE)
#define FLASH_INFO_ETA_TSENSE_FREQUENCYC_ADDRESS     (0x02E0)
#define FLASH_INFO_ETA_TSENSE_FREQUENCYC_OFFSET      (FLASH_INFO_ETA_TSENSE_FREQUENCYC_ADDRESS >> 4)
#define FLASH_INFO_ETA_TSENSE_FREQUENCYC_INDEX       ((FLASH_INFO_ETA_TSENSE_FREQUENCYC_ADDRESS & 0xC) >> 2)

// Temperature Sensor recorded Temperature #3  (RESERVED FOR FUTURE USE)
#define FLASH_INFO_ETA_TSENSE_TEMPERATUREC_ADDRESS   (0x02E0)
#define FLASH_INFO_ETA_TSENSE_TEMPERATUREC_OFFSET    (FLASH_INFO_ETA_TSENSE_TEMPERATUREC_ADDRESS >> 4)
#define FLASH_INFO_ETA_TSENSE_TEMPERATUREC_INDEX     ((FLASH_INFO_ETA_TSENSE_TEMPERATUREC_ADDRESS & 0xC) >> 2)


//
// Info Space Analog Trims
//

// Magic for 4 for Analog trims
#define FLASH_INFO_MAGIC4_ADDRESS                   (0x0030)
#define FLASH_INFO_MAGIC4_OFFSET                    (FLASH_INFO_MAGIC4_ADDRESS >> 4)
#define FLASH_INFO_MAGIC4_INDEX                     ((FLASH_INFO_MAGIC4_ADDRESS & 0xC) >> 2)

#define FLASH_INFO_MAGIC5_ADDRESS                   (0x0034)
#define FLASH_INFO_MAGIC5_OFFSET                    (FLASH_INFO_MAGIC5_ADDRESS >> 4)
#define FLASH_INFO_MAGIC5_INDEX                     ((FLASH_INFO_MAGIC5_ADDRESS & 0xC) >> 2)

#define FLASH_INFO_OSC_KEY_ADDRESS                  (0x0038)
#define FLASH_INFO_OSC_KEY_OFFSET                   (FLASH_INFO_OSC_KEY_ADDRESS >> 4)
#define FLASH_INFO_OSC_KEY_INDEX                    ((FLASH_INFO_OSC_KEY_ADDRESS & 0xC) >> 2)

#define FLASH_INFO_OSC_TRIM_ADDRESS                 (0x003C)
#define FLASH_INFO_OSC_TRIM_OFFSET                  (FLASH_INFO_OSC_TRIM_ADDRESS >> 4)
#define FLASH_INFO_OSC_TRIM_INDEX                   ((FLASH_INFO_OSC_TRIM_ADDRESS & 0xC) >> 2)

#define FLASH_INFO_AO_PMIC_KEY_ADDRESS              (0x0040)
#define FLASH_INFO_AO_PMIC_KEY_OFFSET               (FLASH_INFO_AO_PMIC_KEY_ADDRESS >> 4)
#define FLASH_INFO_AO_PMIC_KEY_INDEX                ((FLASH_INFO_AO_PMIC_KEY_ADDRESS & 0xC) >> 2)

#define FLASH_INFO_AO_PMIC_TRIM_ADDRESS             (0x0044)
#define FLASH_INFO_AO_PMIC_TRIM_OFFSET              (FLASH_INFO_AO_PMIC_TRIM_ADDRESS >> 4)
#define FLASH_INFO_AO_PMIC_TRIM_INDEX               ((FLASH_INFO_AO_PMIC_TRIM_ADDRESS & 0xC) >> 2)

#define FLASH_INFO_M3_PMIC_KEY_ADDRESS              (0x0048)
#define FLASH_INFO_M3_PMIC_KEY_OFFSET               (FLASH_INFO_M3_PMIC_KEY_ADDRESS >> 4)
#define FLASH_INFO_M3_PMIC_KEY_INDEX                ((FLASH_INFO_M3_PMIC_KEY_ADDRESS & 0xC) >> 2)

#define FLASH_INFO_M3_PMIC_TRIM_ADDRESS             (0x004c)
#define FLASH_INFO_M3_PMIC_TRIM_OFFSET              (FLASH_INFO_M3_PMIC_TRIM_ADDRESS >> 4)
#define FLASH_INFO_M3_PMIC_TRIM_INDEX               ((FLASH_INFO_M3_PMIC_TRIM_ADDRESS & 0xC) >> 2)

#define FLASH_INFO_MEM_PMIC_KEY_ADDRESS             (0x0050)
#define FLASH_INFO_MEM_PMIC_KEY_OFFSET              (FLASH_INFO_MEM_PMIC_KEY_ADDRESS >> 4)
#define FLASH_INFO_MEM_PMIC_KEY_INDEX               ((FLASH_INFO_MEM_PMIC_KEY_ADDRESS & 0xC) >> 2)

#define FLASH_INFO_MEM_PMIC_TRIM_ADDRESS            (0x0054)
#define FLASH_INFO_MEM_PMIC_TRIM_OFFSET             (FLASH_INFO_MEM_PMIC_TRIM_ADDRESS >> 4)
#define FLASH_INFO_MEM_PMIC_TRIM_INDEX              ((FLASH_INFO_MEM_PMIC_TRIM_ADDRESS & 0xC) >> 2)

// Removed from trim space
// #define FLASH_INFO_VHFO_KEY_ADDRESS                 (0x0058)
// #define FLASH_INFO_VHFO_KEY_OFFSET                  (FLASH_INFO_VHFO_KEY_ADDRESS >> 4)
// #define FLASH_INFO_VHFO_KEY_INDEX                   ((FLASH_INFO_VHFO_KEY_ADDRESS & 0xC) >> 2)
//
// #define FLASH_INFO_VHFO_TRIM_ADDRESS                (0x005C)
// #define FLASH_INFO_VHFO_TRIM_OFFSET                 (FLASH_INFO_VHFO_TRIM_ADDRESS >> 4)
// #define FLASH_INFO_VHFO_TRIM_INDEX                  ((FLASH_INFO_VHFO_TRIM_ADDRESS & 0xC) >> 2)

#define FLASH_INFO_DSP_PMIC_KEY_ADDRESS             (0x0060)
#define FLASH_INFO_DSP_PMIC_KEY_OFFSET              (FLASH_INFO_DSP_PMIC_KEY_ADDRESS >> 4)
#define FLASH_INFO_DSP_PMIC_KEY_INDEX               ((FLASH_INFO_DSP_PMIC_KEY_ADDRESS & 0xC) >> 2)

#define FLASH_INFO_DSP_PMIC_TRIM_ADDRESS            (0x0060)
#define FLASH_INFO_DSP_PMIC_TRIM_OFFSET             (FLASH_INFO_DSP_PMIC_TRIM_ADDRESS >> 4)
#define FLASH_INFO_DSP_PMIC_TRIM_INDEX              ((FLASH_INFO_DSP_PMIC_TRIM_ADDRESS & 0xC) >> 2)

#define FLASH_INFO_BUCK_FREQ_OFFSET_KEY_ADDRESS     (0x0078)
#define FLASH_INFO_BUCK_FREQ_OFFSET_KEY_OFFSET      (FLASH_INFO_BUCK_FREQ_OFFSET_KEY_ADDRESS >> 4)
#define FLASH_INFO_BUCK_FREQ_OFFSET_KEY_INDEX       ((FLASH_INFO_BUCK_FREQ_OFFSET_KEY_ADDRESS & 0xC) >> 2)

#define FLASH_INFO_BUCK_FREQ_OFFSET_TRIM_ADDRESS    (0x007C)
#define FLASH_INFO_BUCK_FREQ_OFFSET_TRIM_OFFSET     (FLASH_INFO_BUCK_FREQ_OFFSET_TRIM_ADDRESS >> 4)
#define FLASH_INFO_BUCK_FREQ_OFFSET_TRIM_INDEX      ((FLASH_INFO_BUCK_FREQ_OFFSET_TRIM_ADDRESS & 0xC) >> 2)

//
// Customer-specific section of Info Space
//
#define FLASH_INFO_CUST_MAGIC0_ADDRESS              (0x0400)
#define FLASH_INFO_CUST_MAGIC0_OFFSET               (FLASH_INFO_CUST_MAGIC0_ADDRESS >> 4)
#define FLASH_INFO_CUST_MAGIC0_INDEX                ((FLASH_INFO_CUST_MAGIC0_ADDRESS & 0xC) >> 2)

#define FLASH_INFO_CUST_MAGIC1_ADDRESS              (0x0404)
#define FLASH_INFO_CUST_MAGIC1_OFFSET               (FLASH_INFO_CUST_MAGIC1_ADDRESS >> 4)
#define FLASH_INFO_CUST_MAGIC1_INDEX                ((FLASH_INFO_CUST_MAGIC1_ADDRESS & 0xC) >> 2)

#define FLASH_INFO_CUST_FLASH_RD_ENABLE_ADDRESS     (0x0408)
#define FLASH_INFO_CUST_FLASH_RD_ENABLE_OFFSET      (FLASH_INFO_CUST_FLASH_RD_ENABLE_ADDRESS >> 4)
#define FLASH_INFO_CUST_FLASH_RD_ENABLE_INDEX       ((FLASH_INFO_CUST_FLASH_RD_ENABLE_ADDRESS & 0xC) >> 2)

#define FLASH_INFO_CUST_FLASH_WRT_ENABLE_ADDRESS    (0x040C)
#define FLASH_INFO_CUST_FLASH_WRT_ENABLE_OFFSET     (FLASH_INFO_CUST_FLASH_WRT_ENABLE_ADDRESS >> 4)
#define FLASH_INFO_CUST_FLASH_WRT_ENABLE_INDEX      ((FLASH_INFO_CUST_FLASH_WRT_ENABLE_ADDRESS & 0xC) >> 2)

#define FLASH_INFO_CUST_VHFO_KEY_ADDRESS            (0x0410)
#define FLASH_INFO_CUST_VHFO_KEY_OFFSET             (FLASH_INFO_CUST_VHFO_KEY_ADDRESS >> 4)
#define FLASH_INFO_CUST_VHFO_KEY_INDEX              ((FLASH_INFO_CUST_VHFO_KEY_ADDRESS & 0xC) >> 2)

#define FLASH_INFO_CUST_VHFO_TRIM_ADDRESS           (0x0414)
#define FLASH_INFO_CUST_VHFO_TRIM_OFFSET            (FLASH_INFO_CUST_VHFO_TRIM_ADDRESS >> 4)
#define FLASH_INFO_CUST_VHFO_TRIM_INDEX             ((FLASH_INFO_CUST_VHFO_TRIM_ADDRESS & 0xC) >> 2)

#define FLASH_INFO_CUST_XTAL_KEY_ADDRESS            (0x0418)
#define FLASH_INFO_CUST_XTAL_KEY_OFFSET             (FLASH_INFO_CUST_XTAL_KEY_ADDRESS >> 4)
#define FLASH_INFO_CUST_XTAL_KEY_INDEX              ((FLASH_INFO_CUST_XTAL_KEY_ADDRESS & 0xC) >> 2)

#define FLASH_INFO_CUST_XTAL_TRIM_ADDRESS           (0x041C)
#define FLASH_INFO_CUST_XTAL_TRIM_OFFSET            (FLASH_INFO_CUST_XTAL_TRIM_ADDRESS >> 4)
#define FLASH_INFO_CUST_XTAL_TRIM_INDEX             ((FLASH_INFO_CUST_XTAL_TRIM_ADDRESS & 0xC) >> 2)


//
// Create funciton pointers to FLASH helper function in BOOTROM.
//
typedef void (*tpfFlashBrWsHelper)(uint32_t);
typedef uint32_t (*tpfFlashBrLoadHelper)(uint32_t);
typedef void (*tpfFlashBrStoreHelper)(uint32_t, uint32_t);
typedef uint32_t (*tpfFlashBrVersionHelper)(void);
typedef uint32_t (*tpfFlashBrErase)(uint32_t, uint32_t, uint32_t, uint32_t,
                                    uint32_t, uint32_t, uint32_t);
typedef uint32_t (*tpfFlashBrProgram)(uint32_t, uint32_t *, uint32_t, uint32_t,
                                      uint32_t, uint32_t, uint32_t, uint32_t,
                                      uint32_t);
typedef uint32_t (*tpfFlashBrRead)(uint32_t, uint32_t, uint32_t *);

typedef void (*tpfFlashBrDebrick)(void);
typedef void (*tpfFlashBrCopyAndGo)(uint32_t *,uint32_t *,uint32_t ,uint32_t *);



//
// Create the function pointers to reach into the bootrom helpers.
//
extern tpfFlashBrWsHelper pfnFlashBrWsHelper;
extern tpfFlashBrLoadHelper pfnFlashBrLoadHelper;
extern tpfFlashBrStoreHelper pfnFlashBrStoreHelper;
extern tpfFlashBrVersionHelper pfnFlashBrVersionHelper;

extern tpfFlashBrErase pfnFlashBrErase;
extern tpfFlashBrProgram pfnFlashBrProgram;
extern tpfFlashBrRead pfnFlashBrRead;
extern tpfFlashBrRead pfnFlashBrReadBkpt;

extern tpfFlashBrDebrick pfnFlashBrDebrick;
extern tpfFlashBrErase pfnFlashBrEraseBkpt;
extern tpfFlashBrProgram pfnFlashBrProgramBkpt;
extern tpfFlashBrCopyAndGo pfnFlashBrCopyAndGo;

#define ETA_CSP_FLASH_TNVS_COUNT   (0x6)
#define ETA_CSP_FLASH_TNVH_COUNT   (0x6)
#define ETA_CSP_FLASH_TNVH1_COUNT  (0x6E)
#define ETA_CSP_FLASH_TPGS_COUNT   (0xB)
#define ETA_CSP_FLASH_TPROG_COUNT  (0xC)
#define ETA_CSP_FLASH_TRCV_COUNT   (0xB)
#define ETA_CSP_FLASH_TRE_COUNT    (0x186A0)
#define ETA_CSP_FLASH_TERASE_COUNT (0x16000)
#define ETA_CSP_FLASH_TME_COUNT    (0x16000)


#define ETA_CSP_FLASH_MASS_ERASE() pfnFlashBrErase(0x01000000, 1, 0,          \
                                                   ETA_CSP_FLASH_TNVS_COUNT,  \
                                                   ETA_CSP_FLASH_TME_COUNT,   \
                                                   ETA_CSP_FLASH_TNVH1_COUNT, \
                                                   ETA_CSP_FLASH_TRCV_COUNT)
#define ETA_CSP_FLASH_MASS_ERASE_BKPT() pfnFlashBrEraseBkpt(0x01000000, 1, 0,          \
                                                   ETA_CSP_FLASH_TNVS_COUNT,  \
                                                   ETA_CSP_FLASH_TME_COUNT,   \
                                                   ETA_CSP_FLASH_TNVH1_COUNT, \
                                                   ETA_CSP_FLASH_TRCV_COUNT);


#define ETA_CSP_FLASH_PAGE_ERASE(ui32Address)                             \
    pfnFlashBrErase((ui32Address), 0, 0, ETA_CSP_FLASH_TNVS_COUNT,        \
                    ETA_CSP_FLASH_TERASE_COUNT, ETA_CSP_FLASH_TNVH_COUNT, \
                    ETA_CSP_FLASH_TRCV_COUNT);

#define ETA_CSP_FLASH_PAGE_ERASE_BKPT(ui32Address)                             \
    pfnFlashBrEraseBkpt((ui32Address), 0, 0, ETA_CSP_FLASH_TNVS_COUNT,        \
                    ETA_CSP_FLASH_TERASE_COUNT, ETA_CSP_FLASH_TNVH_COUNT, \
                    ETA_CSP_FLASH_TRCV_COUNT);

// ETA_CSP_FLASH_PROGRAM deprecated, see EtaCspFlashProgramFast, below.
#define ETA_CSP_FLASH_PROGRAM(ui32Address, pui32Src, ui32Count) \
    EtaCspFlashProgram(ui32Address, pui32Src, ui32Count,0);

#define ETA_CSP_FLASH_PROGRAM_BKPT(ui32Address, pui32Src, ui32Count) \
    EtaCspFlashProgram(ui32Address, pui32Src, ui32Count,1);

#define ETA_CSP_FLASH_INFO_ERASE() pfnFlashBrErase(0x01000000, 0, 1,           \
                                                   ETA_CSP_FLASH_TNVS_COUNT,   \
                                                   ETA_CSP_FLASH_TERASE_COUNT, \
                                                   ETA_CSP_FLASH_TNVH_COUNT,   \
                                                   ETA_CSP_FLASH_TRCV_COUNT);

#define ETA_CSP_FLASH_INFO_ERASE_BKPT() pfnFlashBrEraseBkpt(0x01000000, 0, 1,           \
                                                   ETA_CSP_FLASH_TNVS_COUNT,   \
                                                   ETA_CSP_FLASH_TERASE_COUNT, \
                                                   ETA_CSP_FLASH_TNVH_COUNT,   \
                                                   ETA_CSP_FLASH_TRCV_COUNT);

#define ETA_CSP_FLASH_INFO_PROGRAM(ui32Address, pui32Src, ui32Count) \
    EtaCspFlashInfoProgram(ui32Address, pui32Src, ui32Count,0)

#define ETA_CSP_FLASH_INFO_PROGRAM_BKPT(ui32Address, pui32Src, ui32Count) \
    EtaCspFlashInfoProgram(ui32Address, pui32Src, ui32Count,1)

#define ETA_CSP_FLASH_READ(ui32Address, ui32Info, pui32Result) \
    pfnFlashBrRead(ui32Address, ui32Info, pui32Result)

#define ETA_CSP_FLASH_READ_BKPT(ui32Address, ui32Info, pui32Result) \
    pfnFlashBrReadBkpt(ui32Address, ui32Info, pui32Result)

/***************************************************************************//**
 *
 *  EtaCspFlashCopyAndGo - Copy Src to Dst then branch to it.
 *
 *  @param pui32Src   - pointer to unsigned 32-bit source vector.
 *  @param pui32Dst   - pointer to unsigned 32-bit destination vector.
 *  @param ui32Count  - number of 32-bit words to copy.
 *  @param pui32Start - pointer to vector address for starting.
 *
 ******************************************************************************/
#define ETA_CSP_FLASH_COPY_AND_GO(pui32Src, pui32Dst, ui32Count,pui32Start) \
    pfnFlashBrCopyAndGo(pui32Src, pui32Dst, ui32Count,pui32Start)


/***************************************************************************//**
 *
 *  EtaCspFlashDebrick - Force a complete erase of the flash
 *  data and info spaces.
 *
 *  @return - does not return except via POR
 *
 ******************************************************************************/
#define ETA_CSP_FLASH_DEBRICK() \
    pfnFlashBrDebrick()

//
// Function prototype for function pointer initializer.
//
extern tEtaStatus EtaCspFlashInit(void);


//
// Perform a mass erase.
//
extern void EtaCspFlashMassErase(void);

//
// Perform a page erase.
//
extern void EtaCspFlashPageErase(uint32_t ui32Address);

//
// Erase the info space.
//
extern void EtaCspFlashInfoErase(void);

//
// Program flash.  Deprecated. see EtaCspFlashProgramFast, below.
//
extern tEtaStatus EtaCspFlashProgram(uint32_t ui32Address,  uint8_t *pui8Src,
					 uint32_t ui32Count, bool bBkpt);


//
// Program Flash with some restrictions that enhance speed.
//
extern tEtaStatus EtaCspFlashProgramFast(uint32_t ui32Address,
		                         uint32_t *pui32Src);

//
// Program the info space.
//
extern tEtaStatus EtaCspFlashInfoProgram(uint32_t ui32Address, uint8_t *pui8Src,
                                         uint32_t ui32Count, bool bBkpt);

//
// Reads exactly four words from main flash using only the Flash controller.
//
extern void EtaCspFlashRead(uint32_t ui32Address, uint32_t *pui32Result);

//
// Reads exactly four words from flash INFO page.
//
extern void EtaCspFlashInfoRead(uint32_t ui32Address, uint32_t *pui32Result);

//
// Reads N number of 8-bit bytes from flash INFO page.
//
extern void EtaCspFlashInfoGet(uint32_t ui32Address, // starting bye address
                               uint8_t *pui8Result,
                               uint32_t ui32Count);  // number of bytes

//
// Returns a 32-bit version number of the BOOTROM code.
//
extern uint32_t EtaCspFlashVersionGet(void);

//
// Returns a 32-bit value of the supplied address.
//
extern uint32_t EtaCspFlashBrLoad(uint32_t ui32Address);

//
// Stores a 32-bit value to the supplied address.
//
extern void EtaCspFlashBrStore(uint32_t ui32Address, uint32_t ui32Value);

//
// Sets the 3-bit flash wait state selector.
//
extern void EtaCspFlashBrWsSet(uint32_t ui32Select);

//
// Test PC and see if we are running in flash.
//
extern bool EtaCspFlashRunningInFlash(void);

//
// Test Eta Bytes in INFO page make sure they are all erased.
//
bool EtaCspFlashEtaInfoBytesAreErased(void);

//
// Test Eta Bytes in INFO page to see if they are ATE locked.
//
bool EtaCspFlashEtaInfoBytesAreAteLocked(void);

#ifdef __cplusplus
}
#endif

#endif // __ETA_CSP_FLASH_H__

