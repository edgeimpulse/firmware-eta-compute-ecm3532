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
#include "reg.h"
#include "dsp_fw_cfg.h"
#include "dsp_support.h"
#include "dsp_msg.h"

int MsgQueueInit(tMsgQueue* queue)
{
	uint8_t index = 0;
    uint8_t dataIndex = 0;
	queue->readIndex = queue->writeIndex = 0;
	//Follow a away which can work for all compilers even chess. later can be chanaged. So no memcpy
	for (index = 0; index < MAX_DSP_LOCAL_QUEUE_SIZE; index++) {
		queue->msgArray[index].taskId = 0;
		queue->msgArray[index].msgId = 0;
        for (dataIndex = 0; dataIndex < MAX_DSP_LOCAL_MSG_DATA_SIZE; dataIndex++) {
            queue->msgArray[index].data[dataIndex] = 0;
        }
	}    
	return 0;
}
int MsgQueueAdd(tMsgQueue* queue, tdspLocalMsg* msg)
{
	// while adding we detect an alost full condition and assert
	// do not write in case of full
	uint8_t next;
    uint8_t index = 0;
// Ensure lock
    disable_interrupt();
	next = queue->writeIndex + 1;  // next is where writeIndex will point to after this write.
	if (next >= MAX_DSP_LOCAL_QUEUE_SIZE) //wrap around
		next = 0;

	if (next == queue->readIndex) { // if the writeIndex + 1 == readIndex, circular buffer is full
		return -1; // We can assert too
	}
	// Load data and then move
	queue->msgArray[queue->writeIndex].taskId = msg->taskId;
	queue->msgArray[queue->writeIndex].msgId = msg->msgId;
    for (index = 0; index < MAX_DSP_LOCAL_MSG_DATA_SIZE; index++) {
        queue->msgArray[queue->writeIndex].data[index] = msg->data[index];
    }

	queue->writeIndex = next;             // writeIndex to next data offset.
    enable_interrupt();
	return 0;  // return success to indicate successful push.
}


int MsgQueueRemove(tMsgQueue* queue, tdspLocalMsg* msg)
{
	uint8_t next;
    uint8_t index = 0;
    disable_interrupt();
	if (queue->writeIndex == queue->readIndex) {  // if the writeIndex == readIndex, we don't have any data
		return -1;
	}
	next = queue->readIndex + 1;  // next is where readIndex will point to after this read.
	if (next >= MAX_DSP_LOCAL_QUEUE_SIZE)
		next = 0;
	//Read data and move
    msg->taskId = queue->msgArray[queue->readIndex].taskId;
    msg->msgId = queue->msgArray[queue->readIndex].msgId;
    for (index = 0; index < MAX_DSP_LOCAL_MSG_DATA_SIZE; index++) {
        msg->data[index] = queue->msgArray[queue->readIndex].data[index];
    }
    queue->readIndex = next;              // readIndex to next offset.
    enable_interrupt();
	return 0;  // return success to indicate successful push.
}
uint8_t IsMsgQueueEmpty(tMsgQueue* queue)
{
    uint8_t result = 0;
    disable_interrupt();
    if (queue->writeIndex == queue->readIndex)
        result = 1;    
    enable_interrupt();
    return result;
}

