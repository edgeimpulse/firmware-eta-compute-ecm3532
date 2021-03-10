//////////////////////////////////////////////////////////////////////////
// The confidential and proprietary information contained in this file may
// only be used by a person authorized under and to the extent permitted
// by a subsisting licensing agreement from Eta Compute Inc.
//
//        (C) Copyright 2019 Eta Compute Inc
//                All Rights Reserved
//
// This entire notice must be reproduced on all copies of this file
// and copies of this file may only be made by a person if such person is
// permitted to do so under the terms of a subsisting license agreement
// from Eta Compute Inc.
//////////////////////////////////////////////////////////////////////////
//TBD: in future can we make it part of the global config and get rid of this file
#ifndef H_DSP_FW_CFG_
#define H_DSP_FW_CFG_
#include "config.h"
#include "dsp_config.h"

#if CONFIG_DSP_MAX_TASKS
#define DSP_MAX_TASKS CONFIG_DSP_MAX_TASKS
#else
#define DSP_MAX_TASKS 4
#endif

#if CONFIG_MAX_TIMERS
#define MAX_TIMERS CONFIG_MAX_TIMERS
#else
#define MAX_TIMERS 10
#endif

#define MAX_TIMER_MS_COUNT 1000

#define DSP_IOMEM_WIN_ADDR(w) ((unsigned long)w * 0x2000UL)
#define DSP_IOMEM_WIN3_ADDR 0x6000UL
#define DSP_IOMEM_WIN2_ADDR 0x4000UL
#define DSP_IOMEM_WIN1_ADDR 0x2000UL
#define DSP_IOMEM_WIN0_ADDR 0x0000UL
#if 0
#define M3_SRAM_WIN_ADDR(w) (0x10010000 + ((unsigned long)w * 0x4000UL))
#define M3_SRAM_WIN3_ADDR 0x1001C000
#define M3_SRAM_WIN2_ADDR 0x10018000
#define M3_SRAM_WIN1_ADDR 0x10014000
#define M3_SRAM_WIN0_ADDR 0x10010000

#define SHM_OFFSET_0 (CONFIG_SHM_START - M3_SRAM_WIN3_ADDR)
#define APP_MEM_WIN	3
#endif

#ifndef CONFIG_M3_RAM_START
    #define CONFIG_M3_RAM_START 0x10000000
#endif 

#ifndef CONFIG_M3_RAM_SIZE
   #define CONFIG_M3_RAM_SIZE 0x40000
#endif

#ifndef CONFIG_SHM_LENGTH
  #define CONFIG_SHM_LENGTH 0x4000 //16K mapping
#endif
//always mapped 16K area
#ifdef CONFIG_SHM_START 
    #undef CONFIG_SHM_START
#endif
#define CONFIG_SHM_START (CONFIG_M3_RAM_START + CONFIG_M3_RAM_SIZE - CONFIG_SHM_LENGTH)
#define M3_SRAM_WIN3_ADDR  CONFIG_SHM_START
#define SHM_OFFSET_0 (CONFIG_SHM_START - M3_SRAM_WIN3_ADDR)

#define APP_MEM_WIN	3
#endif //H_DSP_FW_CFG_
