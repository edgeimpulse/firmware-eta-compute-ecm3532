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

#ifndef H_DSP_MSG_
#define H_DSP_MSG_

#include "dsp_config.h"

#if CONFIG_MAX_DSP_LOCAL_QUEUE_SIZE
#define MAX_DSP_LOCAL_QUEUE_SIZE CONFIG_MAX_DSP_LOCAL_QUEUE_SIZE 
#else
#define MAX_DSP_LOCAL_QUEUE_SIZE 8 
#endif

#if CONFIG_MAX_DSP_LOCAL_MSG_DATA_SIZE
#define MAX_DSP_LOCAL_MSG_DATA_SIZE CONFIG_MAX_DSP_LOCAL_MSG_DATA_SIZE
#else
#define MAX_DSP_LOCAL_MSG_DATA_SIZE 2
#endif


typedef struct dspLocalMsg {
	uint8_t  taskId; 
	uint8_t  msgId; //   particualr message identifier. Create the enum
	uint16_t  data [MAX_DSP_LOCAL_MSG_DATA_SIZE]; // Can pass a combo of address and data.
}tdspLocalMsg;

typedef struct msgQueue_t {
	uint8_t readIndex;
	uint8_t writeIndex;
    tdspLocalMsg msgArray[MAX_DSP_LOCAL_QUEUE_SIZE];
}tMsgQueue;

int MsgQueueInit(tMsgQueue* queue);
int MsgQueueAdd(tMsgQueue* queue, tdspLocalMsg* msg);
int MsgQueueRemove(tMsgQueue* queue, tdspLocalMsg* msg);
uint8_t IsMsgQueueEmpty(tMsgQueue* queue);


#endif//# H_DSP_MSG_
