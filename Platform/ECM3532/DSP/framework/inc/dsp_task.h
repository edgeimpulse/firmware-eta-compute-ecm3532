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
#ifndef H_DSP_TASK_
#define H_DSP_TASK_
#include "dsp_msg.h"

typedef void (*DspTaskHandler)(tdspLocalMsg* msg, void* pArg);
int8_t DspTaskCreate(DspTaskHandler handler, void* pArg, uint8_t* taskId);
int8_t DspTaskDelete(uint8_t taskId);
#define  DSpYield()   do{ return;}while(0);
void bindTasktoRPCModule(uint16_t  moduleId, uint8_t  taskId);
void DspTaskSchedule(void);


#endif//# H_DSP_TASK_
