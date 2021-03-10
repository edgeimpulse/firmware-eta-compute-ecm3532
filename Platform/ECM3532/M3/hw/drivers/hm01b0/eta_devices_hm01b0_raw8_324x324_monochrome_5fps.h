/***************************************************************************//**
 *
 * @file eta_devices_hm01b0_raw8_qvga_5fps.h
 *
 * @brief TODO
 *
 * Copyright (C) ${year} Eta Compute, Inc
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

#ifndef __ETA_DEVICES_HM01B0_RAW8_324x324_MONOCHROME_5FPS__
#define __ETA_DEVICES_HM01B0_RAW8_324x324_MONOCHROME_5FPS__

// Chao:
// I set register 0x3010 bit 0 to 0
// so that I use the full pixel array 324x324
// But I only download the 256x256 in the middle of row or column

#define HM01B0_PIXEL_X_NUM (324)
#define HM01B0_PIXEL_Y_NUM (324)

//
// HM01B0_RAW8_QVGA_8bits_lsb
//
const uint16_t HM01B0_INIT[][2] __attribute__((section(".initSection")))  =
{
// ---------------------------------------------------
// Initial
// ---------------------------------------------------
    {0x0103, 0x00}, // software reset-> was 0x22
    {0x0100, 0x00}, // power up

// ---------------------------------------------------
// Analog
// ---------------------------------------------------
    {0x1003, 0x00}, // BLC target :8  at 8 bit mode
    {0x1007, 0x08}, // BLI target :8  at 8 bit mode
    {0x3044, 0x0A}, // Increase CDS time for settling
    {0x3045, 0x00}, // Make symetric for cds_tg and rst_tg
    {0x3047, 0x0A}, // Increase CDS time for settling
    {0x3050, 0xC0}, // Make negative offset up to 4x
    {0x3051, 0x42}, //
    {0x3052, 0x50}, //
    {0x3053, 0x00}, //
    {0x3054, 0x03}, // tuning sf sig clamping as lowest
    {0x3055, 0xF7}, // tuning dsun
    {0x3056, 0xF8}, // increase adc nonoverlap clk
    {0x3057, 0x29}, // increase adc pwr for missing code
    {0x3058, 0x1F}, // turn on dsun
    {0x3059, 0x1E}, //
    {0x3064, 0x00}, // trigger sync
    {0x3065, 0x04}, // pad pull 0

// ---------------------------------------------------
// Digital function
// ---------------------------------------------------

// BLC
    {0x1000, 0x43}, // BLC_on, IIR
    {0x1001, 0x40}, // [6] : BLC dithering en
    {0x1002, 0x32}, ////  blc_darkpixel_thd

// Dgain
    {0x0350, 0x7F}, // Dgain Control

// BLI
    {0x1006, 0x01}, // [0] : bli enable

// DPC
    {0x1008, 0x00}, // [2:0] : DPC option 0: DPC off   1 : mono   3 : bayer1   5
                    // : bayer2
    {0x1009, 0xA0}, // cluster hot pixel th
    {0x100A, 0x60}, // cluster cold pixel th
    {0x100B, 0x90}, // single hot pixel th
    {0x100C, 0x40}, // single cold pixel th

//
 // advance VSYNC by 12 row
    { 0x3022, 12},
    {0x1012, 0x01}, // Sync. shift  enable
    //{0x1012, 0x00}, // Sync. shift disable

// ROI Statistic
    {0x2000, 0x07}, // [0] : AE stat en	[1] : MD LROI stat en	[2] : MD GROI
                    // stat en	[3] : RGB stat ratio en	[4] : IIR selection (1
                    // -> 16, 0 -> 8)
    {0x2003, 0x00}, // MD GROI 0 y start HB
    {0x2004, 0x1C}, // MD GROI 0 y start LB
    {0x2007, 0x00}, // MD GROI 1 y start HB
    {0x2008, 0x58}, // MD GROI 1 y start LB
    {0x200B, 0x00}, // MD GROI 2 y start HB
    {0x200C, 0x7A}, // MD GROI 2 y start LB
    {0x200F, 0x00}, // MD GROI 3 y start HB
    {0x2010, 0xB8}, // MD GROI 3 y start LB

    {0x2013, 0x00}, // MD LRIO y start HB
    {0x2014, 0x58}, // MD LROI y start LB
    {0x2017, 0x00}, // MD LROI y end HB
    {0x2018, 0x9B}, // MD LROI y end LB

// AE
    {0x2100, 0x01}, // [0]: AE control enable
    {0x2101, 0x50}, // AE target mean min .
    {0x2102, 0x0A}, // AE target mean min .
    {0x2103, 0x03},
    {0x2104, 0x05}, // converge out th
    {0x2105, 0x02}, // max INTG Hb
    {0x2106, 0x14}, // max INTG Lb
    {0x2107, 0x02},
    {0x2108, 0x03}, // max AGain in full
    {0x2109, 0x03}, // max AGain in bin2
    {0x210A, 0x00},
    {0x210B, 0x80}, // max DGain
    {0x210C, 0x40},
    {0x210D, 0x20},
    {0x210E, 0x03},
    {0x210F, 0x00}, // FS 60Hz Hb
    {0x2110, 0x85}, // FS 60Hz Lb
    {0x2111, 0x00}, // Fs 50Hz Hb
    {0x2112, 0xA0}, // FS 50Hz Lb

// MD
    {0x2150, 0x03}, // [0] : MD LROI en	[1] : MD GROI en

// ---------------------------------------------------
// frame rate : 5 FPS
// ---------------------------------------------------
    // 4MHz * 1 * 10^6 (1000000) / (6000208)  (15958 * 376)
    {0x0340, 0x01}, // smia frame length Hb         15958
    {0x0341, 0x04}, // smia frame length Lb

    {0x0342, 0x01}, // smia line length Hb
    {0x0343, 0x78}, // smia line length Lb          376

// ---------------------------------------------------
// Resolution : QVGA 324x244
// ---------------------------------------------------
    {0x3010, 0x00}, // [0] : window mode	0 : full frame 324x324	1 : QVGA
    {0x0383, 0x01},
    {0x0387, 0x01},
    {0x0390, 0x00},

// ---------------------------------------------------
// bit width Selection
// ---------------------------------------------------
    {0x3011, 0x70}, // [0] : 6 bit mode enable
    {0x3059, 0x02}, // [7]: Self OSC En, [6]: 4bit mode, [5]: serial mode,
                    // [4:0]: keep value as 0x02
    {0x3060, 0x2A}, // [5]: gated_clock, [4]: msb first,
                    // [3:2]: vt_reg_div->div by 4 / 8 / 1 / 2
                    // [1;0]: vt_sys_div->div by 8 / 4 / 2 / 1

// ---------------------------------------------------
// CMU update
// ---------------------------------------------------
    {0x0104, 0x01}, // was 0100
#ifdef CONFIG_STREAMING_MODE
    {0x0100, 0x1},
#endif

    // {0x0601, 0x1}, // test pattern
};

#endif // __ETA_DEVICES_HM01B0_RAW8_QVGA_5FPS__
