#include "module_common.h"
#include "m3Interface.h"
#include "eta_csp_mailbox.h"

volatile tM3DSPSharedMemory chess_storage(IOMEM:DSP_IOMEM_WIN_ADDR(APP_MEM_WIN) + (SHM_OFFSET_0 / 2)) PhysicalSharedMemory;

void initializeSharedMemory(void)
{
    EtaCspAhbWindowSet(APP_MEM_WIN, M3_SRAM_WIN3_ADDR);
    PhysicalSharedMemory.topMemoryBoundaryPattern = 0xBEEF;
}


//convert shared memory offset to DSP memory address.
volatile void* getSharedMemoryAddress(uint16_t offset)
{
    uint16_t baseAddress = 0;
    baseAddress = (uint16_t) & (PhysicalSharedMemory.byteArray[0]);
    baseAddress += offset / 2;
    return (volatile void*)(baseAddress);
}











