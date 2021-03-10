/***************************************************************************//**
 *
 * @file eta_csp_dma.h
 *
 * @brief This file contains eta_csp_dma module definitions.
 *
 * Copyright (C) 2020 Eta Compute, Inc
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * This is part of revision 1.0.0b1 of the Tensai Chip Support Package.
 *
 ******************************************************************************/

/***************************************************************************//**
 * @addtogroup ecm3532dma-dsp Direct Memory Access (DMA)
 * @ingroup ecm3532csp-dsp
 * @{
 ******************************************************************************/

#ifndef __ETA_CSP_DMA_H__
#define __ETA_CSP_DMA_H__

#include "eta_chip.h"
#include "eta_csp_isr.h"
#include "eta_csp_status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define SRCDST_RDY_PDM0    0
#define SRCDST_RDY_PDM1    1
#define SRCDST_RDY_I2C     4
#define SRCDST_RDY_ADC     5
#define SRCDST_RDY_I2S     6
#define SRCDST_RDY_SPI     7
#define SRCDST_RDY_ALWAYS1 8
#define SRCDST_RDY_CSR12   12
#define SRCDST_RDY_CSR13   13
#define SRCDST_RDY_CSR14   14
#define SRCDST_RDY_CSR15   15

#define DMA_CHANNEL_STRIDE_SIZE 16

#define ETA_CSP_DMA_DEFAULT_WORDS_PER_BLOCK      1U
#define ETA_CSP_DMA_DEFAULT_SRC_RELOAD_PER_BLOCK DMA_FALSE
#define ETA_CSP_DMA_DEFAULT_DST_RELOAD_PER_BLOCK DMA_FALSE
#define ETA_CSP_DMA_DEFAULT_CMD_RELOAD_PER_XFER  DMA_FALSE
#define ETA_CSP_DMA_DEFAULT_TIMEOUT              0U
#define ETA_CSP_DMA_DEFAULT_INT_LEVEL            0U
#define ETA_CSP_DMA_DEFAULT_SEM_SRCDSTRDY        4U

typedef enum
{
    eDmaTargetXmem,
    eDmaTargetYmem,
    eDmaTargetXmemByte,
    eDmaTargetYmemByte,
    eDmaTargetIoMemI2s,
    eDmaTargetIoMemAdc,
    eDmaTargetIoMemSpi,
    eDmaTargetIoMemI2c,
    eDmaTargetIoMemPdm0,
    eDmaTargetIoMemPdm1,
    eDmaTargetDynamicAhb,
    eDmaTargetIoMemWindow
}
tDmaTarget;

//
//
//! DMA Pack Select
//
typedef enum
{
    //
    //! No Op - default
    //
    eDmaPackCmdNop = 0,

    //
    //! Zero Pad
    //
    eDmaPackCmdZeroPad = 1,

    //
    //! Sign Extend
    //
    eDmaPackCmdSignExt = 2,

    //
    //! Repack
    //
    eDmaPackCmdRepack = 3,
}
tDmaPackCmd;

typedef struct
{
    uint8_t ui8Channel;
    tDmaTarget iSrcTarget;
    tDmaTarget iDstTarget;
    uint32_t ui32SrcAddress; // Not needed for specific
                             // peripherals (like I2S, SPI, etc).
                             // Can hold 32 bit AHB Address, but
                             // usually only lower 16 matter.
    uint32_t ui32DstAddress; // Not needed for specific
                             // peripherals (like I2S, SPI, etc).
                             // Can hold 32 bit AHB Address, but
                             // usually only lower 16 matter.
    uint16_t ui16XferLength; // Expressed in blocks (total is
                             // xfer * word/block)
    uint16_t ui16WordPerBlock; // Words per block. Generally this
                               // is in multiples of 16 bits
    boolean_T src_reload_per_block; // After each block, reload
                                    // source address
    boolean_T dst_reload_per_block; // After each block, reload
                                    // destination address
    boolean_T cmd_reload_per_xfer; // After each transfer, reload
                                   // transfer length, source
                                   // address, destination address
    uint16_t ui16TimeOut; // If 0, nothing. If > 0, is number
                          // of cycles DMA can be preempted
                          // before it stalls core and usurps
                          // access for a cycle.
    uint8_t ui8IntLevel; // If 0, nothing, If > 1 is number
                         // of blocks to transfer before
                         // interrupt
    uint8_t ui8CircBuffTrig; // If 0, nothing, If > 1 is number
                             // of full transfers before we
                             // reload address.
    boolean_T circ_buff_src_reload; // After each circular buffer,
                                    // reload source address
    boolean_T circ_buff_dst_reload; // After each circular buffer,
                                    // reload destination address
    uint8_t ui8PackCmd; // 0=nop, 1=zero pad, 2=sign ext,
                        // 3=repack
    uint8_t ui8SemSrcDstRdy; // 0,1,2,3 means semaphore source or
                             // dest ready 0,1,2,3. A value of 4
                             // means disable (default)
} tDmaCmd;

//
//
//! DMA Return Value
//
typedef enum
{
    //
    //! Configuration is Good
    //
    eDmaCmdGood = 0,

    //
    //! Configuration is invalid
    //
    eDmaCmdInvalidConfig = 1,

    //
    //! Circular Buffer Has Overflowed
    //
    eDmaCmdCircBuffOverflow = 2,
}
tDmaCmdRetVal;

/*******************************************************************************
 *
 * External function definitions.
 *
 ******************************************************************************/
#define eta_csp_launch_dma(X) EtaCspDmaLaunch(X)

//extern int eta_csp_launch_dma_old(tDmaCmd *p_cmd_struct);

// Same as launch, but add a wait here.
#define eta_csp_launch_dma_and_wait(X) EtaCspDmaLaunchAndWait(X)

// Returns default cmd_full for backward compatability (or if you don't care
// about cool/new features)
#define return_default_dma_cmd_full EtaCspDmaCmdGetDefault

// This is the wait command. Note the one inside eta_csp_launch_dma_and_wait is
// slightly different, but practically the same
#define eta_csp_wait_for_dma(X) EtaCspDmaWait(X)

// This is the wait command for those who dont want to use the struct
#define eta_csp_wait_for_dma_ch(X) EtaCspDmaChWait(X)

// This is a launch and execute provided return function when DMA completes.
#define eta_csp_launch_dma_w_retfunc(X,Y) EtaCspDmaLaunchWithRetFunc(X,Y)

// This is a terminate command
#define eta_csp_terminate_dma_cmd(X) EtaCspDmaCmdTerminate(X)
#define eta_csp_terminate_dma_ch(X) EtaCspDmaChTerminate(X)

//
// EtaCspDmaCmdGetDefault - Return a default, simplified dma_cmd of tDmaCmd.
// This is used to provide backwards support to simple tDmaCmd
//
extern tDmaCmd EtaCspDmaCmdGetDefault();


//
// EtaCspDmaCheckForCircOverflow - Check if Circular DMA has overflowed.
//
extern tDmaCmdRetVal EtaCspDmaCheckForCircOverflow(uint16_t ui16AppBuffCount,
                                                   uint16_t ui16AppProcCount,
                                                   uint_fast8_t ui8DmaChannel,
                                                   uint16_t ui16DmaXferLength);

//
// EtaCspDmaCircBuffIncCount - Check how many Circular Buffers have been received
//
extern uint16_t EtaCspDmaCircBuffIncCount(uint16_t ui16AppBuffCount,
                                          uint_fast8_t ui8DmaChannel);

//
// EtaCspDmaChBusyGet - Check busy state of a channel
//
extern tEtaStatus EtaCspDmaChBusyGet(uint_fast8_t ui8Channel);

//
// EtaCspDmaChWait - Wait (polls) for DMA based off of the channel
//
extern tEtaStatus EtaCspDmaChWait(uint_fast8_t ui8Channel);


//
// EtaCspDmaLaunchAndWait - Launch DMA and Wait (polls) for its completion
//
extern tEtaStatus EtaCspDmaLaunchAndWait(tDmaCmd *psDmaCmd);


//
// EtaCspDmaLaunch - Launch DMA based off of dma_cmd
//
extern tEtaStatus EtaCspDmaLaunch(tDmaCmd *psDmaCmdFull);


//
// EtaCspDmaLaunchWithRetFunc - Launch DMA and specify irq handler for DMA completion.
//
extern tEtaStatus EtaCspDmaLaunchWithRetFunc(tDmaCmd *psDmaCmd,
                                      tpfIrqHandler RetHandler);


//
// EtaCspDmaWait - Wait (polls) for DMA based off of the dma_cmd
//
extern tEtaStatus EtaCspDmaWait(tDmaCmd *psDmaCmd);


//
// EtaCspDmaChTerminate - Terminates a DMA
//
extern tEtaStatus EtaCspDmaChTerminate(uint_fast8_t ui8Channel);

//
// EtaCspDmaCmdTerminate - Terminates a DMA
//
extern tEtaStatus EtaCspDmaCmdTerminate(tDmaCmd *psDmaCmd);

//
// EtaCspDmaUnpackLaunchAndWait - Launch DMA from M3 memory space with unpack feature
//
extern tEtaStatus EtaCspDmaUnpackLaunchAndWait(uint_fast8_t ui8Channel, uint32_t ui32AhbByteAddr,
                                        tDmaTarget iDstTarget, uint32_t ui32DstAddr,
                                        uint16_t ui16XferLength,
                                        tDmaPackCmd iPackCmd);

//
// EtaCspDmaUnpackLaunch - Launch DMA from M3 memory space with unpack feature
//
extern tEtaStatus EtaCspDmaUnpackLaunch(uint_fast8_t ui8Channel, uint32_t ui32AhbByteAddr,
                                 tDmaTarget iDstTarget, uint32_t ui32DstAddr,
                                 uint16_t ui16XferLength,
                                 tDmaPackCmd iPackCmd);

//
// EtaCspDmaPackLaunchAndWait - Launch DMA to M3 memory space with pack feature
//
extern tEtaStatus EtaCspDmaPackLaunchAndWait (uint_fast8_t ui8Channel, uint32_t ui32AhbByteAddr,
                                       tDmaTarget iSrcTarget, uint32_t ui32SrcAddr,
                                       uint16_t ui16XferLength);

//
// EtaCspDmaPackLaunch - Launch DMA to M3 memory space with pack feature
//
extern tEtaStatus EtaCspDmaPackLaunch (uint_fast8_t ui8Channel, uint32_t ui32AhbByteAddr,
                                tDmaTarget iSrcTarget, uint32_t ui32SrcAddr,
                                uint16_t ui16XferLength);
//
// EtaCspGetFreeDmaCh - Get A free DMA Channel, Error on All Busy
//
tEtaStatus EtaCspGetFreeDmaCh(void);

#ifdef __cplusplus
}
#endif

#endif // __ETA_CSP_DMA_H__

