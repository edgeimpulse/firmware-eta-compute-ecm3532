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

#ifndef H_M3INTERFACE_
#define H_M3INTERFACE_
#include "reg.h"
#include "workQ_common.h"
#include "dsp_fw_cfg.h"


//extern volatile tM3DSPSharedMemory chess_storage(IOMEM) PhysicalSharedMemory;
extern volatile tM3DSPSharedMemory chess_storage(IOMEM:DSP_IOMEM_WIN_ADDR(APP_MEM_WIN) + (SHM_OFFSET_0 / 2)) PhysicalSharedMemory;
void initializeSharedMemory(void);
volatile void* getSharedMemoryAddress(uint16_t offset);

#endif  //H_M3INTERFACE_
