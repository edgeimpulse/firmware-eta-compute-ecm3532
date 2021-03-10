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
#include "utility.h"
//#include "eta_csp_chip.h"
#include "eta_chip.h"
#include "eta_csp_io.h"
#include "eta_csp_mailbox.h"
#include "api-dma_controller.h"
#include "eta_csp_dma.h"

void io_mem_write(uint16_t chess_storage(IOMEM)* wrAddress, uint16_t val)
{
    volatile uint16_t chess_storage(IOMEM)* pcDest = (volatile uint16_t chess_storage(IOMEM)*)wrAddress;
    *pcDest = val;
}

void io_mem_read(uint16_t chess_storage(IOMEM)* rdAddress, uint16_t *val)
{
    volatile uint16_t chess_storage(IOMEM)* pcSrc = (volatile uint16_t chess_storage(IOMEM)*)rdAddress;

    *val = *pcSrc;
}
//Caution: Src and Dst must be atleast 16bit aligned.
void memcpy_io_mem_to_xmem(void* pvDest, void chess_storage(IOMEM)* pvSource, uint16_t xBytes)
{
    uint16_t x;
    volatile uint16_t* pui16Dest = (volatile uint16_t*)pvDest;
    volatile uint16_t chess_storage(IOMEM)* pui16Source = (volatile uint16_t chess_storage(IOMEM)*)pvSource;
    for (x = 0; x < xBytes / 2; x++)
    {
        pui16Dest[x] = pui16Source[x];
    }
    return;
}

//Caution: Src and Dst must be atleast 16bit aligned.
void memcpy_xmem_to_iomem(void chess_storage(IOMEM)* pvDest, void* pvSource, uint16_t xBytes)
{
    uint16_t x;
    uint16_t tempAddr;

    volatile uint16_t chess_storage(IOMEM)* pui16Dest = (volatile uint16_t chess_storage(IOMEM)*)pvDest;
    volatile uint16_t* pui16Source = (volatile uint16_t*)pvSource;

    for (x = 0; x < xBytes / 2; x++)
    {
        pui16Dest[x] = pui16Source[x];
    }
    return;
}

void memcpy_xmem_to_xmem(void* pvDest, void* pvSource, uint16_t xBytes)
{
    uint16_t x;
    volatile uint16_t *pui16Dest = (uint16_t *)pvDest;
    volatile uint16_t *pui16Source = (uint16_t *)pvSource;
    for (x = 0; x < xBytes / 2; x++)
    {
        pui16Dest[x] = pui16Source[x];
    }
    return;
}


//Provides approximate ms delay.
void delayMs(volatile unsigned long int msDelay)
{
    volatile unsigned long int i;
    unsigned long int  loopCount;

    //loopCount =  delayinMs * 60000 / 13
    //Each loop iteration takes 13 CoolFlux DSP cycles.
    //DSP running at 60MHz.
    loopCount = msDelay * 60000 / 13;
    for (i = 0; i < loopCount; i++)
        ;
}

void dma_xmem_to_iomem(volatile void *  ioMem_dest, void* xmem_src, uint16_t xBytes)
{
#ifdef CONFIG_DSP_ECM3531	   
    volatile uint16_t chess_storage(IOMEM)* tempIoMemBuffer;
    eta_dma_cmd_T  dma_dsptom3;
    tempIoMemBuffer = (uint16_t chess_storage(IOMEM)*)ioMem_dest;
    dma_dsptom3 = return_default_dma_cmd_full();
    dma_dsptom3.channel = 0;
    //dma_dsptom3.xfer_length = N_FFT;
    dma_dsptom3.xfer_length = xBytes / 2;
    dma_dsptom3.word_per_block = 1;
    dma_dsptom3.src_target = DMA_XMEM;
    dma_dsptom3.dst_target = DMA_IOMEM_WINDOW;
    dma_dsptom3.src_address = (unsigned int)xmem_src;
    dma_dsptom3.dst_address = (unsigned int)(tempIoMemBuffer);
    //TRACE("start DMA: size:0x%x\n\r",xBytes);
    eta_csp_launch_dma_and_wait(&dma_dsptom3);
    //TRACE("DMA completed\n\r");
#endif
#ifdef CONFIG_DSP_ECM3532	
   volatile uint16_t chess_storage(IOMEM)* tempIoMemBuffer;
   tDmaCmd dma_cmd;
   tempIoMemBuffer = (uint16_t chess_storage(IOMEM)*)ioMem_dest;
   dma_cmd=EtaCspDmaCmdGetDefault();
   dma_cmd.ui8Channel=0;
   dma_cmd.ui16XferLength=xBytes / 2;
   dma_cmd.ui16WordPerBlock = 1;
   dma_cmd.iSrcTarget = eDmaTargetXmem;
   dma_cmd.iDstTarget = eDmaTargetIoMemWindow;
   dma_cmd.ui32SrcAddress=(unsigned int)xmem_src;
   dma_cmd.ui32DstAddress=(unsigned int)(tempIoMemBuffer);
   EtaCspDmaLaunchAndWait(&dma_cmd);   
#endif

}
