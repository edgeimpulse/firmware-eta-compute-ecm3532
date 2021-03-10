#ifndef H_SHMEM_
#define H_SHMEM_
#include <stdint.h>
int SharedMemInit(void* startAddress, uint32_t size);
void SharedMemDeinit(void);
void* SharedMemAlloc(uint32_t size);
void  SharedMemFree(void* mem);
uint16_t SharedMemGetOffset(void* mem); // offset in bytes
void* SharedMemGetAddress(uint16_t offset);
#endif//# H_SHMEM_
