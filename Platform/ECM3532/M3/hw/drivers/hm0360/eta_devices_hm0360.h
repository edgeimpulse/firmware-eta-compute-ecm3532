/***************************************************************************//**
 *
 * @file eta_devices_hm01b0.h
 *
 * @brief Sensor driver for the Himax HM01B0 camera.
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
 * This is part of revision ${version} of the Tensai Software Development Kit.
 *
 ******************************************************************************/

/***************************************************************************//**
 * @addtogroup etadevices-hm01b0 Himax HM01B0 QVGA Camera
 * @ingroup etadevices
 * @{
 ******************************************************************************/

#ifndef __ETA_DEVICES_HM01B0__
#define __ETA_DEVICES_HM01B0__

#include <stdint.h>
#include "eta_csp_inc.h"

#ifdef __cplusplus
extern "C" {
#endif

//
// The DEBUG pin.
//
#define DEBUG_PIN eGpioBit24

/*******************************************************************************
 *
 * Hm01b0 Pin assignments.
 *
 ******************************************************************************/
typedef struct
{
    tGpioBit iD0;
    uint32_t ui32Hsync;
    uint32_t ui32Vsync;
    uint32_t ui32Pclk;
}
tHm0360Pins;

typedef enum
{
    //! Standby
    eHm0360ModeStandby    = 0x0,

    //! Continuous streaming
    eHm0360ModeStreaming1 = 0x1,

    //! Automatic wake up sleep cycles
    eHm0360ModeStreaming2 = 0x2,

    //! Snapshot with N frames output
    eHm0360ModeStreaming3 = 0x3,

    //! Hardware Trigger: Continuous streaming
    eHm0360ModeStreaming4 = 0x4,

    //! Hardware Trigger: Snapshot with N frames output
    eHm0360ModeStreaming5 = 0x5,

    //! Hardware Trigger: Automatic wake up sleep cycles
    eHm0360ModeStreaming6 = 0x6
}
tHm0360Mode;

/*******************************************************************************
 *
 * Global definitions for the registers.
 *
 ******************************************************************************/

#define HM0360_MODEL_ID_H           (0x0000)
#define HM0360_MODEL_ID_L           (0x0001)
#define HM0360_SILICON_REV          (0x0002)
#define HM0360_FRAME_COUNT_H        (0x0005)
#define HM0360_FRAME_COUNT_L        (0x0006)
#define HM0360_PIXEL_ORDER          (0x0007)

// R&W registers
// Sensor mode control
#define HM0360_MODE_SELECT          (0x0100)
#define HM0360_IMG_ORIENTATION      (0x0101)
#define HM0360_EMBEDDED_LINE_ENABLE (0x0102)
#define HM0360_SW_RESET             (0x0103)
#define HM0360_COMMAND_UPDATE       (0x0104)

// Sensor exposure gain control
#define HM0360_INTEGRATION_H        (0x0202)
#define HM0360_INTEGRATION_L        (0x0203)
#define HM0360_ANALOG_GAIN          (0x0205)
#define HM0360_DIGITAL_GAIN_H       (0x020E)
#define HM0360_DIGITAL_GAIN_L       (0x020F)

// Clock control registers
#define HM0360_PLL1CFG              (0x0300)
#define HM0360_PLL2CFG              (0x0301)
#define HM0360_PLL3CFG              (0x0302)

// Frame timing control
#define HM0360_FRAME_LEN_LINES_H    (0x0340)
#define HM0360_FRAME_LEN_LINES_L    (0x0341)
#define HM0360_LINE_LEN_PCK_H       (0x0342)
#define HM0360_LINE_LEN_PCK_L       (0x0343)

// Monochrome programming registers
#define HM0360_MONO_MODE            (0x0370)
#define HM0360_MONO_MODE_ISP        (0x0371)
#define HM0360_MONO_MODE_SEL        (0x0372)

// Subsampling/Binning mode control
#define HM0360_H_SUB                (0x0380)
#define HM0360_V_SUB                (0x0381)
#define HM0360_BINNING_MODE         (0x0382)

// Test pattern control
#define HM0360_TEST_PATTERN_MODE    (0x0601)
#define HM0360_TEST_DATA_BLUE_H     (0x0602)
#define HM0360_TEST_DATA_BLUE_L     (0x0603)
#define HM0360_TEST_DATA_GB_H       (0x0604)
#define HM0360_TEST_DATA_GB_L       (0x0605)
#define HM0360_TEST_DATA_GR_H       (0x0606)
#define HM0360_TEST_DATA_GR_L       (0x0607)
#define HM0360_TEST_DATA_RED_H      (0x0608)
#define HM0360_TEST_DATA_RED_L      (0x0609)

// frame formatting controls
#define HM0360_FRAME_VSYNC_HSYNC    (0x1014)

// analog and other settings
#define HM0360_ANA_REG_03           (0x310E)
#define HM0360_ANA_REG_04           (0x310F)
#define HM0360_ANA_REG_05           (0x3110)
#define HM0360_ANA_REG_06           (0x3111)
#define HM0360_ANA_REG_07           (0x3112)

#define HM0360_ANA_PLL1CFG          (0x3500)


/*******************************************************************************
 *
 * External function definitions.
 *
 ******************************************************************************/
void EtaDevicesHm0360Init(void);

uint32_t EtaDevicesHm0360OneFrameReadBlocking(uint8_t *pui8Frame);

#ifdef __cplusplus
}

#endif

#endif // __ETA_DEVICES_HM01B0__
