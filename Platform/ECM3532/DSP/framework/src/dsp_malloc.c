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
#include <stdlib.h>
#include <limits.h>
#include "dsp_config.h"
#include "dsp_fw_cfg.h"
#include "reg.h"
#include "eta_csp_io.h"
#include "dsp_config.h"
#include "dsp_malloc.h"

#define USED 1
#define LOG  etaPrintf

#define NUMBER_OFBYTES_IN_INT            2 // This you need to change
//#define CONFIG_XMEM_HEAP_SIZE_BYTES CONFIG_XMEM_HEAP_SIZE_BYTES *4 // it will come from DSP config file
unsigned int  xHeap[CONFIG_XMEM_HEAP_SIZE_BYTES / NUMBER_OFBYTES_IN_INT] = { 0 };
unsigned int* XHeapCurrent = 0;
unsigned long XHeapFree = 0;

unsigned int  _YMEM yHeap[CONFIG_YMEM_HEAP_SIZE_BYTES / NUMBER_OFBYTES_IN_INT] = { 0 };
unsigned int  _YMEM  *YHeapCurrent = 0;
unsigned long _YMEM YHeapFree = 0;


void Dsp_Xmem_heap_Init(void) {
    XHeapCurrent = (void*)&xHeap[0];
    XHeapFree = CONFIG_XMEM_HEAP_SIZE_BYTES;
}

void Dsp_Ymem_heap_Init(void) {
    YHeapCurrent = (void _YMEM*)&yHeap[0];
    YHeapFree = CONFIG_YMEM_HEAP_SIZE_BYTES;
}

void* Dsp_Xmem_Alloc(unsigned   size)
{
    void* ptr = 0;
    unsigned int* info = 0;
    if (size > XHeapFree)
    {
        LOG(" No Memory available available (XMEM)= %d, Asked size = %d \n");

    }
    else 
    {
        info = (unsigned int*)XHeapCurrent;
        *info = size / NUMBER_OFBYTES_IN_INT;
        ptr = (void*)(XHeapCurrent + 1);
        XHeapCurrent = XHeapCurrent + 1 + size / NUMBER_OFBYTES_IN_INT;
        XHeapFree = XHeapFree - size - NUMBER_OFBYTES_IN_INT;
    }
    return ptr;
}

void _YMEM* Dsp_Ymem_Alloc(unsigned   size)
{
    void _YMEM* ptr = 0;
    unsigned int _YMEM* info = 0;
    if (size > YHeapFree)
    {
        LOG(" No Memory available available (YMEM) = %d, Asked size = %d \n");

    }
    else
    {
        info = (unsigned int _YMEM*)YHeapCurrent;
        *info = size / NUMBER_OFBYTES_IN_INT;
        ptr = (void _YMEM*)(YHeapCurrent + 1);
        YHeapCurrent = YHeapCurrent + 1 + size / NUMBER_OFBYTES_IN_INT;
        YHeapFree = YHeapFree - size - NUMBER_OFBYTES_IN_INT;
    }
    return ptr;
}


void Dsp_Xmem_Free(void* ptr)
{
    unsigned int size = 0;
    unsigned int* info = (unsigned int*)((unsigned int)ptr - NUMBER_OFBYTES_IN_INT);
    size = *info;
    XHeapCurrent = XHeapCurrent - 1 - size;
    XHeapFree = XHeapFree + (size + 1) * NUMBER_OFBYTES_IN_INT;

}

void Dsp_Ymem_Free(void _YMEM* ptr)
{
    unsigned int size = 0;
    unsigned int _YMEM* info = (unsigned int _YMEM*)((unsigned int _YMEM)ptr - NUMBER_OFBYTES_IN_INT);
    size = *info;
    YHeapCurrent = YHeapCurrent - 1 - size;
    YHeapFree = YHeapFree + (size + 1) * NUMBER_OFBYTES_IN_INT;

}


void Dsp_Xmem_heap_Info(void)
{
    //LOG(" available heap = %d\r\n", xmemInfo.curAvl);
}

void Dsp_Ymem_heap_Info(void)
{
    //LOG(" available heap = %d\r\n", ymemInfo.curAvl);
}


#if 0

#define YMEM_HEAP_START	4
typedef struct {
  unsigned size;
} memUnit;

typedef struct {
  memUnit* free;
  memUnit* heap;
  unsigned curAvl;
} DspMem;


typedef struct {
  memUnit _YMEM * free;
  memUnit _YMEM * heap;
  unsigned curAvl;
} DspYMem;


uint16_t xHeap[CONFIG_XMEM_HEAP_SIZE_BYTES/2]= {0};
uint16_t chess_storage(YMEM:YMEM_HEAP_START) yHeap[CONFIG_YMEM_HEAP_SIZE_BYTES/2]= {0};

static DspMem xmemInfo;
static DspYMem  _YMEM ymemInfo;

static memUnit* Xcompact( memUnit *p, unsigned nsize )
{
       unsigned bsize, psize;
       memUnit *best;

       best = p;
       bsize = 0;

       while( psize = p->size, psize )
       {
              if( psize & USED )
              {
                  if( bsize != 0 )
                  {
                      best->size = bsize;
                      if( bsize >= nsize )
                      {
                          return best;
                      }
                  }
                  bsize = 0;
                  best = p = (memUnit *)( (unsigned)p + (psize & ~USED) );
              }
              else
              {
                  bsize += psize;
                  p = (memUnit *)( (unsigned)p + psize );
              }
       }

       if( bsize != 0 )
       {
           best->size = bsize;
           if( bsize >= nsize )
           {
               return best;
           }
       }

       return 0;
}


static memUnit _YMEM * Ycompact( memUnit _YMEM  *p, unsigned nsize )
{
       unsigned bsize, psize;
       memUnit _YMEM  *best;

       best = p;
       bsize = 0;

       while( psize = p->size, psize )
       {
              if( psize & USED )
              {
                  if( bsize != 0 )
                  {
                      best->size = bsize;
                      if( bsize >= nsize )
                      {
                          return best;
                      }
                  }
                  bsize = 0;
                  best = p = (memUnit _YMEM *)( (unsigned)p + (psize & ~USED) );
              }
              else
              {
                  bsize += psize;
                  p = (memUnit _YMEM  *)( (unsigned)p + psize );
              }
       }

       if( bsize != 0 )
       {
           best->size = bsize;
           if( bsize >= nsize )
           {
               return best;
           }
       }

       return 0;
}


static void DspXmem_Init( void *heap, unsigned len )
{
     DspMem * pMemInfo = 0;
     len  += 3;
     len >>= 2;
     len <<= 2;
     pMemInfo = &xmemInfo;
     pMemInfo->free = pMemInfo->heap = (memUnit *) heap;
     (pMemInfo->free)->size = (pMemInfo->heap)->size = len - sizeof(memUnit);
     pMemInfo->curAvl = (pMemInfo->free)->size;
     *(unsigned *)((char *)heap + len - 4) = 0;
}


static void DspYmem_Init( void _YMEM *heap, unsigned len )
{
   
     len  += 3;
     len >>= 2;
     len <<= 2;
     ymemInfo.free = ymemInfo.heap = (memUnit  _YMEM *) heap;
     (ymemInfo.free)->size = (ymemInfo.heap)->size = len - sizeof(memUnit);
     ymemInfo.curAvl = (ymemInfo.free)->size;
     *(unsigned *)((char *)heap + len - 4) = 0;

}

 void Dsp_Xmem_Free( void *ptr )
{
     
     DspMem * pMemInfo = 0;
     pMemInfo = &xmemInfo;

     if( ptr )
     {
         memUnit *p;

         p = (memUnit *)( (unsigned)ptr - sizeof(memUnit) );
         p->size &= ~USED;
         pMemInfo->curAvl = pMemInfo->curAvl + p->size;
     }
}


 void Dsp_Ymem_Free( void _YMEM *ptr )
{
     
     

     if( ptr )
     {
         memUnit _YMEM  *p;

         p = (memUnit _YMEM *)( (unsigned)ptr - sizeof(memUnit) );
         p->size &= ~USED;
         ymemInfo.curAvl = ymemInfo.curAvl + p->size;
     }
}


 void *Dsp_Xmem_Alloc( unsigned size  )
{
     unsigned fsize;
     memUnit *p;
     DspMem * pMemInfo = 0;
     pMemInfo = &xmemInfo;     

     if( size == 0 ) return 0;

     size  += 3 + sizeof(memUnit);
     size >>= 2;
     size <<= 2;

     if( pMemInfo->free == 0)
     {  
	pMemInfo->free = Xcompact( pMemInfo->heap, size );
         if( pMemInfo->free == 0 ) {
               return 0;
         }
     }
     if( size > (pMemInfo->free)->size )
     {
        
        pMemInfo->free = Xcompact( pMemInfo->heap, size );
         if( pMemInfo->free == 0 ) {
               LOG(" available memory = %d, request for allocation (XMEM) = %d\r\n", pMemInfo->curAvl,size);
		return 0;
         }
     }


     p = pMemInfo->free;
     fsize = (pMemInfo->free)->size;

     if( fsize >= size + sizeof(memUnit) )
     {
         pMemInfo->free = (memUnit *)( (unsigned)p + size );
         (pMemInfo->free)->size = fsize - size;
     }
     else
     {
         pMemInfo->free = 0;
         size = fsize;
     }
     pMemInfo->curAvl = pMemInfo->curAvl - size; 
     p->size = size | USED;

     return (void *)( (unsigned)p + sizeof(memUnit) );
}


void _YMEM *Dsp_Ymem_Alloc( unsigned size  )
{
     unsigned fsize;
     memUnit _YMEM  *p;
            

     if( size == 0 ) return 0;

     size  += 3 + sizeof(memUnit);
     size >>= 2;
     size <<= 2;

     if( ymemInfo.free == 0)
     {  
	ymemInfo.free = Ycompact( ymemInfo.heap, size );
         if( ymemInfo.free == 0 ) {
               return 0;
         }
     }
     if( size > (ymemInfo.free)->size )
     {
        
        ymemInfo.free = Ycompact( ymemInfo.heap, size );
         if( ymemInfo.free == 0 ) {
               LOG(" available memory = %d, request for allocation (YMEM)= %d\r\n", ymemInfo.curAvl,size);
		return 0;
         }
     }


     p =  ymemInfo.free;
     fsize = (ymemInfo.free)->size;

     if( fsize >= size + sizeof(memUnit) )
     {
         ymemInfo.free = (memUnit  _YMEM *)( (unsigned)p + size );
         (ymemInfo.free)->size = fsize - size;
     }
     else
     {
         ymemInfo.free = 0;
         size = fsize;
     }
     ymemInfo.curAvl = ymemInfo.curAvl - size; 
     p->size = size | USED;

     return (void _YMEM *)( (unsigned)p + sizeof(memUnit) );
}


 void Dsp_Xmem_Compact(  )
{
     DspMem * pMemInfo = 0;
     pMemInfo = &xmemInfo; 
     pMemInfo->free = Xcompact( pMemInfo->heap, INT_MAX );
}

void Dsp_Ymem_Compact(void )
{
 ymemInfo.free = Ycompact( ymemInfo.heap, INT_MAX );
}


void Dsp_Xmem_heap_Info( void)
{
	LOG(" available heap = %d\r\n",xmemInfo.curAvl);
}

void Dsp_Ymem_heap_Info( void)
{
	LOG(" available heap = %d\r\n",ymemInfo.curAvl);
}

void Dsp_Xmem_heap_Init(void)
{
 DspXmem_Init((void*) xHeap,CONFIG_XMEM_HEAP_SIZE_BYTES);
}

void Dsp_Ymem_heap_Init(void)
{
 DspYmem_Init((void _YMEM * ) yHeap,CONFIG_YMEM_HEAP_SIZE_BYTES);
}
#endif
