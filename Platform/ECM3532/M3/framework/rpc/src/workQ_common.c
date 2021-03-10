//#include <stdint.h>
#include "config.h"
#include "FreeRTOS.h" //Include it for M3 build but not for DSP.
#include "semphr.h"
#include "workQ_common.h"

#ifdef INC_FREERTOS_H
//Needed to make the functions reentrant from multiple M3 tasks
SemaphoreHandle_t xWorkQueueMutex;
#endif

__attribute__((section(".initSection")))
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
#ifdef INC_FREERTOS_H
    xWorkQueueMutex = xSemaphoreCreateMutex();

    if (xWorkQueueMutex == NULL) {
        //TBD: Add trace message of error
        return -1;
    }
#endif
    return 0;

}
int WorkQueueAdd(tWorkQueue* queue, tWorkDescriptor* work)
{
    // while adding we detect an alost full condition and assert
    // do not write in case of full
    uint16_t next;
#ifdef INC_FREERTOS_H
    xSemaphoreTake(xWorkQueueMutex, portMAX_DELAY);
#endif
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
#ifdef INC_FREERTOS_H
    xSemaphoreGive(xWorkQueueMutex);
#endif
    return 0;  // return success to indicate successful push.
}


int WorkQueueRemove(tWorkQueue* queue, tWorkDescriptor* work)
{
    uint16_t next;
#ifdef INC_FREERTOS_H
    xSemaphoreTake(xWorkQueueMutex, portMAX_DELAY);
#endif

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

#ifdef INC_FREERTOS_H
    xSemaphoreGive(xWorkQueueMutex);
#endif
    return 0;  // return success to indicate successful push.
}
