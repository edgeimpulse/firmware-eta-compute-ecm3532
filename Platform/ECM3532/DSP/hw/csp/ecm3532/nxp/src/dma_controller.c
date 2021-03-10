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

#include "api-dma_controller.h"


dma_err_T dma_configParty(p_hw_dma_regs_T p_dma_channel, comm_party_T party, cf_memory_bank_T memory_bank, unsigned int start_address, unsigned int step, unsigned int type, boolean_T auto_reload_start_address)
{
    if (dma_channelBusy(p_dma_channel))
    {
        return DMA_CHANNEL_BUSY;
    }

    p_dma_channel->cfg_reg[party].mem = memory_bank;
    p_dma_channel->cfg_reg[party].inc = (step != 0);
    
    p_dma_channel->cfg_reg[party].reload = auto_reload_start_address;
    p_dma_channel->cfg_reg[party].step = step;
    p_dma_channel->cfg_reg[party].type = type;

    p_dma_channel->address[party] = start_address;
    
    return DMA_SUCCESS;
}

dma_err_T dma_changeAddress(p_hw_dma_regs_T p_dma_channel, comm_party_T party, unsigned int start_address, unsigned int step, unsigned int type, boolean_T auto_reload_start_address)
{
    if (dma_channelBusy(p_dma_channel))
    {
        return DMA_CHANNEL_BUSY;
    }

    p_dma_channel->cfg_reg[party].reload = auto_reload_start_address;
    p_dma_channel->cfg_reg[party].step = step;
    p_dma_channel->cfg_reg[party].type = type;

    p_dma_channel->address[party] = start_address;

    return DMA_SUCCESS;

}

dma_err_T dma_changeAddress(p_hw_dma_regs_T p_dma_channel, comm_party_T party, unsigned int start_address, unsigned int step, unsigned int type)
{
    if (dma_channelBusy(p_dma_channel))
    {
        return DMA_CHANNEL_BUSY;
    }

    
    p_dma_channel->cfg_reg[party].step = step;
    p_dma_channel->cfg_reg[party].type = type;

    p_dma_channel->address[party] = start_address;

    return DMA_SUCCESS;
}


dma_err_T dma_changeAddress(p_hw_dma_regs_T p_dma_channel, comm_party_T party, unsigned int start_address)
{
    if (dma_channelBusy(p_dma_channel))
    {
        return DMA_CHANNEL_BUSY;
    }
    
    p_dma_channel->address[party] = start_address;
    
    return DMA_SUCCESS;	
}

dma_err_T dma_changeStep(p_hw_dma_regs_T p_dma_channel, comm_party_T party, unsigned int step)
{
    if (dma_channelBusy(p_dma_channel))
    {
        return DMA_CHANNEL_BUSY;
    }
    
    p_dma_channel->cfg_reg[party].step = step;
    
    return DMA_SUCCESS;	
}

dma_err_T dma_changeAutoReloadFlag(p_hw_dma_regs_T p_dma_channel, comm_party_T party, boolean_T enable_flag)
{
    if (dma_channelBusy(p_dma_channel))
    {
        return DMA_CHANNEL_BUSY;
    }
    
    p_dma_channel->cfg_reg[party].reload = enable_flag;
    
    return DMA_SUCCESS;	

}

// ETA Added
dma_err_T dma_configCircularBuffer(p_hw_dma_regs_T p_dma_channel, unsigned int circbuff_trig, boolean_T reload_dst, boolean_T reload_src)
{	
    if (dma_channelBusy(p_dma_channel))
    {
        return DMA_CHANNEL_BUSY;
    }

    p_dma_channel->trigger_reg.circbuff_trigger = circbuff_trig;
    p_dma_channel->trigger_reg.circbuff_dst_reload = reload_dst;
    p_dma_channel->trigger_reg.circbuff_src_reload = reload_src;

    return DMA_SUCCESS;
}

dma_err_T dma_configLevelSensitiveReadySignal(p_hw_dma_regs_T p_dma_channel, comm_party_T party, unsigned int ready_signal_idx, polarity_T polarity)
{	
    if (dma_channelBusy(p_dma_channel))
    {
        return DMA_CHANNEL_BUSY;
    }

    p_dma_channel->cfg_reg[party].rdy_idx = ready_signal_idx;

    if (party == SOURCE)
    {
        p_dma_channel->trigger_reg.src_edge = LEVEL_SENSITIVE;
        p_dma_channel->trigger_reg.src_pol = polarity;
        p_dma_channel->trigger_reg.src_both_edges = 0;
    }
    else
    {
        p_dma_channel->trigger_reg.dst_edge = LEVEL_SENSITIVE;
        p_dma_channel->trigger_reg.dst_pol = polarity;
        p_dma_channel->trigger_reg.dst_both_edges = 0;
    }

    return DMA_SUCCESS;
}

dma_err_T dma_configEdgeSensitiveReadySignal(p_hw_dma_regs_T p_dma_channel, comm_party_T party, unsigned int ready_signal_idx, dma_edge_T used_edge)
{
    if (dma_channelBusy(p_dma_channel))
    {
        return DMA_CHANNEL_BUSY;
    }

    p_dma_channel->cfg_reg[party].rdy_idx = ready_signal_idx;

    if (party == SOURCE)
    {
        p_dma_channel->trigger_reg.src_edge = EDGE_SENSITIVE;
        if (used_edge == DMA_BOTH_EDGES)
        {
            p_dma_channel->trigger_reg.src_both_edges = 1;
        }
        else
        {
            p_dma_channel->trigger_reg.src_pol = used_edge;
            p_dma_channel->trigger_reg.src_both_edges = 0;
        }
        
    }
    else
    {
        p_dma_channel->trigger_reg.dst_edge = EDGE_SENSITIVE;
        if (used_edge == DMA_BOTH_EDGES)
        {
            p_dma_channel->trigger_reg.dst_both_edges = 1;
        }
        else
        {
            p_dma_channel->trigger_reg.dst_pol = used_edge;
            p_dma_channel->trigger_reg.dst_both_edges = 0;
        }
        
    }

    return DMA_SUCCESS;
}

dma_err_T dma_configBlock(p_hw_dma_regs_T p_dma_channel, unsigned int no_blocks, unsigned int no_words_per_block, boolean_T auto_reload_no_blocks)
{
    if (dma_channelBusy(p_dma_channel))
    {
        return DMA_CHANNEL_BUSY;
    }

    
    p_dma_channel->words_per_block = no_words_per_block - 1;

    p_dma_channel->numblocks_reg.auto_reload = (unsigned int) auto_reload_no_blocks;

    /* This must be the last action because it activates the DMA channel 
    ---> Check if compiler may change order, use chess_separator*/
    chess_separator();
    p_dma_channel->numblocks_reg.numblocks = no_blocks;

    return DMA_SUCCESS;
}

dma_err_T dma_getStatus(p_hw_dma_regs_T p_dma_channel, boolean_T *p_busy_dma_channel_flag, transfer_phase_T *p_transfer_phase, unsigned int *p_src_address, unsigned int *p_dst_address)
{
    *p_busy_dma_channel_flag =(boolean_T) dma_channelBusy(p_dma_channel);
    *p_transfer_phase = (transfer_phase_T) p_dma_channel->status_reg.half;
    *p_src_address = p_dma_channel->address[SOURCE];
    *p_dst_address = p_dma_channel->address[DESTINATION];

    return DMA_SUCCESS;
}

dma_err_T dma_changeHalfBufferInterupt(p_hw_dma_regs_T p_dma_channel, boolean_T enable_flag)
{
    if (dma_channelBusy(p_dma_channel))
    {
        return DMA_CHANNEL_BUSY;
    }

    p_dma_channel->numblocks_reg.half = enable_flag;
    p_dma_channel->int_level_reg = 0;

    return DMA_SUCCESS;
}

dma_err_T dma_changeHalfBufferInteruptLevel(p_hw_dma_regs_T p_dma_channel, boolean_T enable_flag, unsigned int int_level)
{
    if (dma_channelBusy(p_dma_channel))
    {
        return DMA_CHANNEL_BUSY;
    }

    p_dma_channel->numblocks_reg.half = enable_flag;
    p_dma_channel->int_level_reg = int_level;

    return DMA_SUCCESS;
}

dma_err_T dma_setDMATimer(p_hw_dma_regs_T p_dma_regs, unsigned int core_hold_duration_in_cycles)
{
    if (core_hold_duration_in_cycles >= (1 << CORE_HOLD_BITS))
    {
        return DMA_INVALID_CORE_HOLD_DURATION;
    }

    p_dma_regs->timer = core_hold_duration_in_cycles;

    return DMA_SUCCESS;
}

dma_err_T dma_cancelTransfer(p_hw_dma_regs_T p_dma_channel)
{
    if (dma_channelBusy(p_dma_channel))
    {
        p_dma_channel->numblocks_reg.numblocks = 0;

        return DMA_SUCCESS;
    }

    return DMA_TRANSFER_ALREADY_FINISHED;
}

dma_err_T dma_getHWRegistersSnapshot(p_hw_dma_regs_T p_dma_regs, hw_dma_regs_T *p_hw_dma_regs_snapshot)
{
    *p_hw_dma_regs_snapshot = *p_dma_regs;
    
    return DMA_SUCCESS;
}
