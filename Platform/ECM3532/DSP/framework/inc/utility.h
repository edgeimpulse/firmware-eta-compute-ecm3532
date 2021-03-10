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

#ifndef H_UTILITY_
#define H_UTILITY_


#define TRUE  1
#define FALSE 0
//Provides approximate ms delay.
#define MAX(a,b) ((a) > (b) ? (a):(b))
#define MIN(a,b) ((a) < (b) ? (a):(b))

#include "reg.h"
#include "dsp_config.h"

#ifdef CONFIG_DEBUG_PRINTS  //This will be automatically defined through dsp_config.h
#define ENABLE_TRACE
#endif

#ifdef ENABLE_TRACE
#define TRACE etaPrintf
#else
#define TRACE(...)
#endif

void io_mem_write(uint16_t chess_storage(IOMEM)* wrAddress, uint16_t val);
void io_mem_read(uint16_t chess_storage(IOMEM)* rdAddress, uint16_t* val);

extern void memcpy_io_mem_to_xmem(void* pvDest, void chess_storage(IOMEM)* pvSource, uint16_t xBytes);
extern void memcpy_xmem_to_iomem(void chess_storage(IOMEM)* pvDest, void* pvSource, uint16_t xBytes);
extern void memcpy_xmem_to_xmem(void* pvDest, void* pvSource, uint16_t xBytes);
void delayMs(volatile unsigned long int loopCount);
void dma_xmem_to_iomem(volatile void *  ioMem_dest, void* xmem_src, uint16_t xBytes);

#endif  //H_UTILITY_
