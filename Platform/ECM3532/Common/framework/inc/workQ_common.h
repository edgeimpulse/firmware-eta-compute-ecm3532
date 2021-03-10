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

#ifndef H_WORKQ_COMMON_
#define H_WORKQ_COMMON_
#include "config.h"
//TBD: Needs to go to config.h
#define MAX_WORK_QUEUE_SIZE 2
/* reserved 32 for tM3DSPSharedMemory */
#define SHARED_MEMORY_ALLOCATOR_SIZE (CONFIG_SHM_LENGTH - 32)

#ifdef __GNUC__
#include "stdint.h"
#define chess_storage(x)
#else
#include "reg.h"
#endif

typedef struct workDescriptor_t {
    uint16_t  moduleId; //8bit moduleId   sensor, executor
    uint16_t  operation; //example sensor's open, ioctl, read etc.   FFT,CONVOLUTION
    uint16_t argumentPointerOffset; //memory offset where additional arguments for commands are located.
}tWorkDescriptor;

typedef struct workQueue_t {
    uint16_t readIndex;
    uint16_t writeIndex;
    tWorkDescriptor workArray[MAX_WORK_QUEUE_SIZE];
} tWorkQueue;

typedef struct sharedMemory_t {
    uint16_t   topMemoryBoundaryPattern;  //For debugging purpose, it will be filled with DEADBEAF
    uint16_t  reserved;
    uint16_t byteArray[SHARED_MEMORY_ALLOCATOR_SIZE / 2];
    tWorkQueue workQueue;
    uint16_t bottomMemoryBoundaryPattern;  //For debugging purpose, it will be filled with DEADBEAF
} tM3DSPSharedMemory;

int WorkQueueInit(tWorkQueue* queue);   //Shared queue between m3 and dsp will be initialized by M3
int WorkQueueAdd(tWorkQueue* queue, tWorkDescriptor* work);  //New workTask will be added by M3 into queue

#ifdef __GNUC__
int WorkQueueRemove(tWorkQueue chess_storage(IOMEM)* queue, tWorkDescriptor* work); //DSP will takeout task from queue and process.
#else
int WorkQueueRemove(volatile tWorkQueue chess_storage(IOMEM)* queue, tWorkDescriptor* work); //DSP will takeout task from queue and process.
#endif

uint8_t IsWorkQueueEmpty(volatile tWorkQueue chess_storage(IOMEM)* queue);


#endif//# H_WORKQ_COMMON_
