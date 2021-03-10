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
#include "workQ_common.h"
int WorkQueueInit(tWorkQueue* queue)
{
    uint8_t index = 0;
    queue->readIndex = queue->writeIndex = 0;
    //Follow a away which can work for all compilers even chess. later can be chanaged. So no memcpy
    for (index = 0; index < MAX_WORK_QUEUE_SIZE; index++) {
        queue->workArray[index].moduleId = 0;
        queue->workArray[index].operation = 0;
        queue->workArray[index].argumentPointerOffset = 0;
    }

    return 0;

}
int WorkQueueAdd(tWorkQueue* queue, tWorkDescriptor* work)
{
    // while adding we detect an alost full condition and assert
    // do not write in case of full
    uint16_t next;
    next = queue->writeIndex + 1;  // next is where writeIndex will point to after this write.
    if (next >= MAX_WORK_QUEUE_SIZE) //wrap around
        next = 0;

    if (next == queue->readIndex) { // if the writeIndex + 1 == readIndex, circular buffer is full
        return -1; // We can assert too
    }
    // Load data and then move
    queue->workArray[queue->writeIndex].moduleId = work->moduleId;
    queue->workArray[queue->writeIndex].operation = work->operation;
    queue->workArray[queue->writeIndex].argumentPointerOffset = work->argumentPointerOffset;

    queue->writeIndex = next;             // writeIndex to next data offset.
    return 0;  // return success to indicate successful push.
}


int WorkQueueRemove(volatile tWorkQueue chess_storage(IOMEM)* queue, tWorkDescriptor* work)
{
    uint16_t next;
    if (queue->writeIndex == queue->readIndex) {  // if the writeIndex == readIndex, we don't have any data
        return -1;
    }

    next = queue->readIndex + 1;  // next is where readIndex will point to after this read.
    if (next >= MAX_WORK_QUEUE_SIZE)
        next = 0;
    //Read data and move
    work->moduleId = queue->workArray[queue->readIndex].moduleId;
    work->operation = queue->workArray[queue->readIndex].operation;
    work->argumentPointerOffset = queue->workArray[queue->readIndex].argumentPointerOffset;

    queue->readIndex = next;              // readIndex to next offset.
    return 0;  // return success to indicate successful push.
}

uint8_t IsWorkQueueEmpty(volatile tWorkQueue chess_storage(IOMEM)* queue)
{

    return (queue->writeIndex == queue->readIndex);
     
}
