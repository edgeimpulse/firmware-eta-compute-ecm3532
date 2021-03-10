/* ----------------------------------------------------------------------------------- */
/* Copyright (c) 2010 NXP B.V.                                                         */
/* All rights reserved.                                                                */
/* This source code and any compilation or derivative thereof is the                   */
/* proprietary information of NXP B.V. and                                             */
/* is confidential in nature.                                                          */
/*                                                                                     */
/* Under no circumstances is this software to be combined with any Open                */
/* Source Software in any way or placed under an Open Source License of                */
/* any type without the express written permission of                                  */
/* NXP B.V.                                                                            */
/*
/* Version 3.0.0
/* ----------------------------------------------------------------------------------- */


/*--------------------------------------------------------------------**
    Purpose:  unique header file implementing the API of the module
**--------------------------------------------------------------------*/

#ifndef API_DMA_CONTROLLER_H
#define API_DMA_CONTROLLER_H

#include "cf6_chess.h"


typedef struct
{
 unsigned int mem: 2;
 unsigned int inc: 1;
 unsigned int rdy_idx: 4;
 unsigned int reload: 1;
 unsigned int step: 5;
 unsigned int type: 3;
//NXP ORIG unsigned int reserved: 8;
} hw_comm_party_cfg_reg_T;

typedef struct
{
  unsigned int numblocks: 14;
  unsigned int half: 1;
  unsigned int auto_reload: 1;
//NXP ORIG unsigned int reserved: 8;
} hw_numblocks_T;

typedef struct
{
 unsigned int half: 1;
  unsigned int reserved: 15; // was 23.  How can this be giving an error?
} hw_status_T;

typedef struct
{
  unsigned int src_pol: 1;
  unsigned int src_edge: 1;
  unsigned int src_both_edges: 1;
  
  unsigned int dst_pol: 1;
  unsigned int dst_edge: 1;
  unsigned int dst_both_edges: 1;
//NXP ORIG  unsigned int reserved:10;
  unsigned int reserved1:2;
  unsigned int circbuff_trigger:3;
  unsigned int reserved2:3;
  unsigned int circbuff_dst_reload:1;
  unsigned int circbuff_src_reload:1;
} hw_trigger_T;


typedef struct
{
  hw_comm_party_cfg_reg_T cfg_reg[2]; // 0, 1
  unsigned int words_per_block; // 2
  unsigned int address[2]; //3,4
  hw_numblocks_T numblocks_reg; //5
  hw_status_T status_reg;  //7
  hw_trigger_T trigger_reg; //7
#define CORE_HOLD_BITS 10 
  unsigned int timer; // 8
  unsigned int busy; // 9
  unsigned int int_level_reg; // A
} hw_dma_regs_T;


typedef volatile hw_dma_regs_T chess_storage(IOMEM) *p_hw_dma_regs_T;

typedef enum {SOURCE = 0, DESTINATION = 1} comm_party_T;
typedef enum {DMA_FALSE = 0, DMA_TRUE = 1} boolean_T;
typedef enum {CF_IOMEM = 0, CF_XMEM = 1, CF_YMEM = 2} cf_memory_bank_T;
typedef enum {ACTIVE_HIGH = 0, ACTIVE_LOW = 1} polarity_T;
typedef enum {DMA_SUCCESS = 0, DMA_CHANNEL_BUSY, DMA_INVALID_CORE_HOLD_DURATION, DMA_TRANSFER_ALREADY_FINISHED} dma_err_T;
typedef enum {DMA_RISING_EDGE = 0, DMA_FALLING_EDGE = 1, DMA_BOTH_EDGES = 3} dma_edge_T;
typedef enum {TRANSFERRING_SECOND_HALF = 0, TRANSFERRING_FIRST_HALF = 1} transfer_phase_T;
typedef enum {LEVEL_SENSITIVE = 0, EDGE_SENSITIVE = 1} sensitivity_T;

/*
Configure one of the parties involved in the data transfer:
- The channel to perform the transfer: p_dma_chann_reg.
- The party to be configured: party.
- The memory used (XMEM, YMEM or IOMEM): memory.
- The address: start_address.
- The step to update the address after each transferred word: step.
- Whether the address is reset at the end of each transferred word: auto_reload_start_address.
*/
dma_err_T dma_configParty(p_hw_dma_regs_T p_dma_chann_reg, comm_party_T party, cf_memory_bank_T memory, unsigned int start_address, unsigned int step, unsigned int type, boolean_T auto_reload_start_address);

/*
Change the address of the source/destination of a data transfer and the step this is updated with after each transferred word.
*/
dma_err_T dma_changeAddress(p_hw_dma_regs_T p_dma_chann_reg, comm_party_T party, unsigned int start_address, unsigned int step, unsigned int type);

/*
Change the address of the source/destination of a data transfer.
*/
dma_err_T dma_changeAddress(p_hw_dma_regs_T p_dma_chann_reg, comm_party_T party, unsigned int start_address);

/*
Change the step an address is updated with:
    - The DMA channel: p_dma_chan_reg.
    - The party represented by the address (source/ destination): party.
    - The step value to be set: step.
*/
dma_err_T dma_changeStep(p_hw_dma_regs_T p_dma_chann_reg, comm_party_T party, unsigned int step);

/*
Change whether the address is reset at the end of each block:
    - The DMA channel: p_dma_chan_reg.
    - Address of the source/destination: party.
    - Enable/disable the reset address setting: enable_flag.
*/
dma_err_T dma_changeAutoReloadFlag(p_hw_dma_regs_T p_dma_chann_reg, comm_party_T party, boolean_T enable_flag);

/*
Configure a circular buffer DMA
    - The channel responsible for the transfer: p_dma_channel.
    - Number of full transfers before reloading DST or SRC Addr: circbuff_trig.
    - Reload Destination Address: reload_dst
    - Reload Source Address: reload_src
*/
dma_err_T dma_configCircularBuffer(p_hw_dma_regs_T p_dma_channel, unsigned int circbuff_trig, boolean_T reload_dst, boolean_T reload_src);

/*
Configure a level triggering signal:
    - The channel responsible for the transfer: p_dma_chann_reg.
    - Belonging to the source/destination of a transfer: party.
    - Having the following index: ready_signal_idx.
    - Triggering level (high/low): polarity.
*/
dma_err_T dma_configLevelSensitiveReadySignal(p_hw_dma_regs_T p_dma_chann_reg, comm_party_T party, unsigned int ready_signal_idx, polarity_T polarity);

/*
Configure an edge triggering signal:
    - The channel responsible for the transfer: p_dma_chann_reg.
    - Belonging to the source/destination of a transfer: party.
    - Having the following index: ready_signal_idx.
    - What edge (rising, falling, both) is used to trigger: used_edge.
*/
dma_err_T dma_configEdgeSensitiveReadySignal(p_hw_dma_regs_T p_dma_chann_reg, comm_party_T party, unsigned int ready_signal_idx, dma_edge_T used_edge);

/*
Commit a transfer on a DMA channel. The size of the data transfer is given by:
    - Number of blocks.
    - Words in a block.
    - Whether the DMA transfer is repeated, by reseting the initial addresses and the number of blocks.
*/
dma_err_T dma_configBlock(p_hw_dma_regs_T p_dma_chann_reg, unsigned int no_blocks, unsigned int no_words_per_block, boolean_T auto_reload_no_blocks);

/*
Get status information of DMA channel, consisting of:
    - Whether the DMA channel is busy with a transfer.
    - Which half is being transferred (first/ second).
    - Source and destination address at the moment of the call.
*/
dma_err_T dma_getStatus(p_hw_dma_regs_T p_dma_chann_reg, boolean_T *p_busy_dma_channel_flag, transfer_phase_T *p_transfer_phase, unsigned int *p_src_address, unsigned int *p_dst_address);

/*
Enable/disable the half buffer interrupt
*/
dma_err_T dma_changeHalfBufferInterupt(p_hw_dma_regs_T p_dma_chann_reg, boolean_T enable_flag);

/*
Enable/disable the half buffer interrupt with interrupt level
*/
dma_err_T dma_changeHalfBufferInteruptLevel(p_hw_dma_regs_T p_dma_channel, boolean_T enable_flag, unsigned int int_level);
/*
Set the value in cycles for the DMA to wait until taking control of the bus
*/
dma_err_T dma_setDMATimer(p_hw_dma_regs_T p_dma_chann_reg, unsigned int max_cycles_before_core_hold);

/*
Cancel a transfer.
*/
dma_err_T dma_cancelTransfer(p_hw_dma_regs_T p_dma_chann_reg);

/* Get a copy of the DMA Controller configuration registers */
dma_err_T dma_getHWRegistersSnapshot(p_hw_dma_regs_T p_dma_chann_reg, hw_dma_regs_T *p_hw_dma_regs_snapshot);

/* Returns whether the DMA channel is busy or not. */
static inline int dma_channelBusy(p_hw_dma_regs_T p_dma_chann_reg)
{
    return ((p_dma_chann_reg->numblocks_reg.numblocks) || (p_dma_chann_reg->busy)); 
}

#endif
