#include "eta_csp_io.h"
#include "reg.h"
#include "dsp_malloc.h"
#include "dsp_config.h"

#define LOG  etaPrintf


void  testXHeapOnly(void )
{
	void * ptr1 = 0;
	void * ptr2 = 0;
        void * ptr3 = 0;
	LOG("### XMEM Malloc test starts ##### \r\n");
	Dsp_Xmem_heap_Init();
	Dsp_Xmem_heap_Info();
	ptr1 = Dsp_Xmem_Alloc(1024);
	if (!ptr1)
		LOG("Malloc failed !! \r\n");
	LOG("ptr1 = 0x%x\r\n",ptr1);
	Dsp_Xmem_heap_Info();
         *((uint16_t *)ptr1) = 0x1234;
        LOG("*ptr1 = 0x%x\r\n",  *((uint16_t *)ptr1)); 
        LOG("Done read write\r\n");
	ptr2 = Dsp_Xmem_Alloc(CONFIG_XMEM_HEAP_SIZE_BYTES);
	if (!ptr2)
		LOG("Malloc failed !! \r\n");
	LOG("ptr2 = 0x%x\r\n",ptr2);
        ptr2 = Dsp_Xmem_Alloc(2048);
	if (!ptr2)
		LOG("Malloc failed !! \r\n");
	LOG("ptr2 = 0x%x\r\n",ptr2);
	Dsp_Xmem_heap_Info();

	LOG("Freeing ptr1\r\n");
	Dsp_Xmem_Free(ptr1);
        LOG (" Free done \r\n");
	Dsp_Xmem_heap_Info();
        ptr1 = Dsp_Xmem_Alloc(1024);
	if (!ptr1)
		LOG("Malloc failed !! \r\n");
	LOG("ptr1 = 0x%x\r\n",ptr1);
	Dsp_Xmem_heap_Info();
        ptr3 = Dsp_Xmem_Alloc(500);
	if (!ptr3)
		LOG("Malloc failed !! \r\n");
	LOG("ptr3 = 0x%x\r\n",ptr3);
        Dsp_Xmem_heap_Info();
        Dsp_Xmem_Free(ptr2);
        Dsp_Xmem_heap_Info();
        Dsp_Xmem_Free(ptr1);
        Dsp_Xmem_heap_Info();
        ptr2 = Dsp_Xmem_Alloc(3072);
	if (!ptr2)
		LOG("Malloc failed !! \r\n");
	LOG("ptr2 = 0x%x\r\n",ptr2);
	Dsp_Xmem_heap_Info();
        LOG("### XMEM Malloc test ends ##### \r\n");
		
}

void  testYHeapOnly(void )
{
	void _YMEM * ptr1 = 0;
	void _YMEM * ptr2 = 0;
        void _YMEM* ptr3 = 0;
	LOG("### YMEM Malloc test starts ##### \r\n");
	Dsp_Ymem_heap_Init();
	Dsp_Ymem_heap_Info();

	ptr1 = Dsp_Ymem_Alloc(1024);
	if (!ptr1)
		LOG("Malloc failed !! \r\n");
	LOG("ptr1 = 0x%x\r\n",ptr1);
        LOG(" Doing read write \r\n");
        *((uint16_t *)ptr1) = 0x1234;
        LOG("*ptr1 = 0x%x \r\n",  *((uint16_t *)ptr1)); 
        LOG(" Done read write\r \n");    
	Dsp_Ymem_heap_Info();
	ptr2 = Dsp_Ymem_Alloc(CONFIG_YMEM_HEAP_SIZE_BYTES);
	if (!ptr2)
		LOG("Malloc failed !! \r\n");
	LOG("ptr2 = 0x%x\r\n",ptr2);
        ptr2 = Dsp_Ymem_Alloc(2048);
	if (!ptr2)
		LOG("Malloc failed !! \r\n");
	LOG("ptr2 = 0x%x\r\n",ptr2);
	Dsp_Ymem_heap_Info();

	LOG("Freeing ptr1\r\n");
	Dsp_Ymem_Free(ptr1);
        LOG (" Free done \r\n");
	Dsp_Ymem_heap_Info();
        ptr1 = Dsp_Ymem_Alloc(1024);
	if (!ptr1)
		LOG("Malloc failed !! \r\n");
	LOG("ptr1 = 0x%x\r\n",ptr1);
	Dsp_Ymem_heap_Info();
        ptr3 = Dsp_Ymem_Alloc(500);
	if (!ptr3)
		LOG("Malloc failed !! \r\n");
	LOG("ptr3 = 0x%x\r\n",ptr3);
        Dsp_Ymem_heap_Info();
        Dsp_Ymem_Free(ptr2);
        Dsp_Ymem_heap_Info();
        Dsp_Ymem_Free(ptr1);
        Dsp_Ymem_heap_Info();
        ptr2 = Dsp_Ymem_Alloc(3072);
	if (!ptr2)
		LOG("Malloc failed !! \r\n");
	LOG("ptr2 = 0x%x\r\n",ptr2);
	Dsp_Ymem_heap_Info();
        LOG("### YMEM Malloc test ends ##### \r\n");
		
}
int main(void)
{
    EtaCspIoPrintfViaDspUart(true); //enable prints.	
    testXHeapOnly();
    testYHeapOnly();
}

