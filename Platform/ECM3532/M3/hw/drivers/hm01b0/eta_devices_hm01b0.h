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
tHm01b0Pins;

/*******************************************************************************
 *
 * Global definitions for the registers.
 *
 ******************************************************************************/

// Register address
// Read only registers
#define HM01B0_MODEL_ID_H  0x0000
#define HM01B0_MODEL_ID_L  0x0001
#define HM01B0_FRAME_COUNT 0x0005
#define HM01B0_PIXEL_ORDER 0x0006

// R&W registers
// Sensor mode control
#define HM01B0_MODE_SELECT     0x0100
#define HM01B0_IMG_ORIENTATION 0x0101
#define HM01B0_SW_RESET        0x0103
#define HM01B0_GRP_PARAM_HOLD  0x0104

// Sensor exposure gain control
#define HM01B0_INTEGRATION_H  0x0202
#define HM01B0_INTEGRATION_L  0x0203
#define HM01B0_ANALOG_GAIN    0x0205
#define HM01B0_DIGITAL_GAIN_H 0x020E
#define HM01B0_DIGITAL_GAIN_L 0x020F

// Frame timing control
#define HM01B0_FRAME_LEN_LINES_H        0x0340
#define HM01B0_HM01B0_FRAME_LEN_LINES_L 0x0341
#define HM01B0_LINE_LEN_PCK_H           0x0342
#define HM01B0_LINE_LEN_PCK_L           0x0343

// Binning mode control
#define HM01B0_READOUT_X    0x0383
#define HM01B0_READOUT_Y    0x0387
#define HM01B0_BINNING_MODE 0x0390

// Test pattern control
#define HM01B0_TEST_PATTERN_MODE 0x0601

// Black level control
#define HM01B0_BLC_CFG  0x1000
#define HM01B0_BLC_TGT  0x1003
#define HM01B0_BLI_EN   0x1006
#define HM01B0_BLC2_TGT 0x1007

// Sensor reserved
#define HM01B0_DPC_CTRL        0x1008
#define HM01B0_SINGLE_THR_HOT  0x100B
#define HM01B0_SINGLE_THR_COLD 0x100C

// VSYNC,HSYNC and pixel shift register
#define HM01B0_VSYNC_HSYNC_PIXEL_SHIFT_EN 0x1012

// Automatic exposure gain control
#define HM01B0_AE_CTRL         0x2100
#define HM01B0_AE_TARGET_MEAN  0x2101
#define HM01B0_AE_MIN_MEAN     0x2102
#define HM01B0_CONVERGE_IN_TH  0x2103
#define HM01B0_CONVERGE_OUT_TH 0x2104
#define HM01B0_MAX_INTG_H      0x2105
#define HM01B0_MAX_INTG_L      0x2106
#define HM01B0_MIN_INTG        0x2107
#define HM01B0_MAX_AGAIN_FULL  0x2108
#define HM01B0_MAX_AGAIN_BIN2  0x2109
#define HM01B0_MIN_AGAIN       0x210A
#define HM01B0_MAX_DGAIN       0x210B
#define HM01B0_MIN_DGAIN       0x210C
#define HM01B0_DAMPING_FACTOR  0x210D
#define HM01B0_FS_CTRL         0x210E
#define HM01B0_FS_60HZ_H       0x210F
#define HM01B0_FS_60HZ_L       0x2110
#define HM01B0_FS_50HZ_H       0x2111
#define HM01B0_FS_50HZ_L       0x2112
#define HM01B0_FS_HYST_TH      0x2113

// Motion detection control
#define HM01B0_MD_CTRL         0x2150
#define HM01B0_I2C_CLEAR       0x2153
#define HM01B0_WMEAN_DIFF_TH_H 0x2155
#define HM01B0_WMEAN_DIFF_TH_M 0x2156
#define HM01B0_WMEAN_DIFF_TH_L 0x2157
#define HM01B0_MD_THH          0x2158
#define HM01B0_MD_THM1         0x2159
#define HM01B0_MD_THM2         0x215A
#define HM01B0_MD_THL          0x215B

// Sensor timing control
#define HM01B0_QVGA_WIN_EN            0x3010
#define HM01B0_SIX_BIT_MODE_EN        0x3011
#define HM01B0_PMU_AUTOSLEEP_FRAMECNT 0x3020
#define HM01B0_ADVANCE_VSYNC          0x3022
#define HM01B0_ADVANCE_HSYNC          0x3023
#define HM01B0_EARLY_GAIN             0x3035

// IO and clock control
#define HM01B0_BIT_CONTROL               0x3059
#define HM01B0_OSC_CLK_DIV               0x3060
#define HM01B0_ANA_Register_11           0x3061
#define HM01B0_IO_DRIVE_STR              0x3062
#define HM01B0_IO_DRIVE_STR2             0x3063
#define HM01B0_ANA_Register_14           0x3064
#define HM01B0_OUTPUT_PIN_STATUS_CONTROL 0x3065
#define HM01B0_ANA_Register_17           0x3067
#define HM01B0_PCLK_POLARITY             0x3068

/*
 * Useful value of Himax registers
 */
#define HIMAX_RESET       0x01
#define PCLK_RISING_EDGE  0x00
#define PCLK_FALLING_EDGE 0x01
#define AE_CTRL_ENABLE    0x00
#define AE_CTRL_DISABLE   0x01

typedef enum
{
    eHm01b0ModeStandby    = 0x0,
    eHm01b0ModeStreaming1 = 0x1, // I2C triggered streaming enable
    eHm01b0ModeStreaming2 = 0x3, // Output N frames
    eHm01b0ModeStreaming3 = 0x5 // Hardware Trigger
}
tHm01b0Mode;

/*******************************************************************************
 *
 * External function definitions.
 *
 ******************************************************************************/
void EtaDevicesHm01b0Init(void);

uint32_t EtaDevicesHm01b0OneFrameReadBlocking(int8_t *pui8Frame);

#ifdef __cplusplus
}

#endif

#endif // __ETA_DEVICES_HM01B0__
