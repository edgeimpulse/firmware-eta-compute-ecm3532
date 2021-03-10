
#include "config.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "helper_func.h"
#include "shmem.h"
#include "bget.h"


SemaphoreHandle_t xShMemMutex;

//Statistics
void* startSharedMem = NULL;
uint32_t sizeSharedMem = -1;

//Public functions
__attribute__((section(".initSection"))) int SharedMemInit(void* startAddress, uint32_t size)
{

    xShMemMutex = xSemaphoreCreateMutex();

    if (xShMemMutex == NULL) {
        //TBD: Add trace message of error
        return -1;
    }

    bpool(startAddress, size);
    startSharedMem = startAddress;
    sizeSharedMem = size;
    return 0;
}

void SharedMemDeinit(void)
{

    vSemaphoreDelete(xShMemMutex);

    startSharedMem = NULL;
    sizeSharedMem = -1;
}

void* SharedMemAlloc(uint32_t size)
{
    void* mem = NULL;
    if (size > sizeSharedMem) {
        // TBD: Add trace message of error
        return mem;
    }
    //Thread safe : renentrant

    xSemaphoreTake(xShMemMutex, portMAX_DELAY);

    mem = bget(size);

    xSemaphoreGive(xShMemMutex);

    return mem;
}
void  SharedMemFree(void* mem)
{
    //Thread safe : renentrant

    xSemaphoreTake(xShMemMutex, portMAX_DELAY);

    brel(mem);

    xSemaphoreGive(xShMemMutex);

}
uint16_t SharedMemGetOffset(void* mem)
{
    return ((uint32_t)mem - (uint32_t)startSharedMem);
}

void* SharedMemGetAddress(uint16_t offset)
{
    return (void *)((uint32_t)offset + (uint32_t)startSharedMem);
}

//TBD add statistics functions later

