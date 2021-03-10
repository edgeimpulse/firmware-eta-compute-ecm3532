/***************************************************************************//**
 *
 * @file eta_csp_dma.c
 *
 * @brief This file contains eta_csp_dma module implementations.
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

#include <string.h>
#include <stdlib.h>

#include "cf6_chess.h"

#include "reg.h"
#include "api-dma_controller.h"
#include "eta_csp_mailbox.h"
#include "eta_csp_dma.h"
#include "eta_chip.h"
#include "eta_csp_io.h"
#include "eta_csp_status.h"

#define DMA_CHANNEL_COUNT (4)
volatile uint_fast8_t hw_dma_ch_st[DMA_CHANNEL_COUNT];

/***************************************************************************//**
 *
 *  EtaCspDmaLaunchAndWait - Launch DMA and Wait (polls) for its completion
 *
 *  @param psDmaCmd - Pointer to tDmaCmd structure
 *
 *  @return Returns eEtaSuccess if launch was successful, else eEtaFailure.
 *
 ******************************************************************************/
tEtaStatus
EtaCspDmaLaunchAndWait(tDmaCmd *psDmaCmd)
{
    tEtaStatus iEtaStatus;

    p_hw_dma_regs_T dma_controller_ptr;

    dma_controller_ptr =
        (p_hw_dma_regs_T)(DMA_CONTROLLER_ADDRESS +
                          ((psDmaCmd->ui8Channel) * DMA_CHANNEL_STRIDE_SIZE));
    iEtaStatus = EtaCspDmaLaunch(psDmaCmd);

    // Wait for DMA to complete
    while(dma_channelBusy(dma_controller_ptr))
    {
    }

    return(iEtaStatus);
}

/***************************************************************************//**
 *
 *  EtaCspDmaWait - Wait (polls) for DMA based off of the dma_cmd
 *
 *  @param psDmaCmd - Pointer to tDmaCmd structure
 *
 *  @return Returns eEtaSuccess if all went well, else config error
 *
 ******************************************************************************/
tEtaStatus
EtaCspDmaWait(tDmaCmd *psDmaCmd)
{
    tEtaStatus iEtaStatus;
    p_hw_dma_regs_T dma_controller_ptr;

    iEtaStatus = EtaCspDmaChWait(psDmaCmd->ui8Channel);

    return(iEtaStatus);
}

/***************************************************************************//**
 *
 *  Checks the busy state of DMA channel.
 *
 *  @param ui8Channel is the channel to check the state of.
 *
 *  @return Returns eEtaBusy if channel is busy, else eEtaSuccess.
 *
 ******************************************************************************/
tEtaStatus
EtaCspDmaChBusyGet(uint_fast8_t ui8Channel)
{
    p_hw_dma_regs_T dma_controller_ptr;

    dma_controller_ptr =
        (p_hw_dma_regs_T)(DMA_CONTROLLER_ADDRESS +
                          (ui8Channel * DMA_CHANNEL_STRIDE_SIZE));

    if(dma_channelBusy(dma_controller_ptr))
    {
        return eEtaBusy;
    }
    else
    {
        return eEtaSuccess;
    }
}

/***************************************************************************//**
 *
 *  EtaCspGetFreeDmaCh
 *
 *  @param ui8Channel - Channel to poll off of.
 *
 *  @return Returns eEtaSuccess if all went well, else config error
 *
 ******************************************************************************/

tEtaStatus EtaCspGetFreeDmaCh(void)
{
  int ch = DMA_CHANNEL_COUNT -1;

  do {
    if (EtaCspDmaChBusyGet(ch) != eEtaBusy)
      break;
    ch--;
  } while (ch >= 0);

  if (ch < 0)
  {
    etaPrintf("\r\n DMA Error no free channel\r\n");
    ch = eEtaBusy;
  }

  return ch;
}

/***************************************************************************//**
 *
 *  EtaCspDmaChWait - Wait (polls) for DMA based off of the channel
 *
 *  @param ui8Channel - Channel to poll off of.
 *
 *  @return Returns eEtaSuccess if all went well, else config error
 *
 ******************************************************************************/
tEtaStatus
EtaCspDmaChWait(uint_fast8_t ui8Channel)
{
    tEtaStatus iEtaStatus;
    p_hw_dma_regs_T dma_controller_ptr;

    dma_controller_ptr =
        (p_hw_dma_regs_T)(DMA_CONTROLLER_ADDRESS +
                          (ui8Channel * DMA_CHANNEL_STRIDE_SIZE));

    // Wait for DMA to complete
    while(dma_channelBusy(dma_controller_ptr))
    {
    }

    iEtaStatus = eEtaSuccess;
    return(iEtaStatus);
}

/***************************************************************************//**
 *
 *  EtaCspDmaLaunchWithRetFunc - Launch DMA and specify irq handler for DMA
 *  completion.
 *
 *  @param psDmaCmd - Pointer to tDmaCmd structure
 *  @param RetHandler - Pointer to tpfIrqHandler ISR handler
 *
 *  > The ISR will be in place if the channel is reused, so issue:
 *    EtaCspIsrDefaultHandlerSet(INT_NUM_DMA0 + psDmaCmd->ui8Channel,
 *    DMA_ISR_LINE, RetHandler);
 *    If you don't want subsequent DMA completions to execute the provided
 *    handler
 *
 *  @return Returns eEtaSuccess if launch was successful, else eEtaFailure.
 *
 ******************************************************************************/
tEtaStatus
EtaCspDmaLaunchWithRetFunc(tDmaCmd *psDmaCmd,
                           tpfIrqHandler RetHandler)
{
    tEtaStatus iEtaStatus;

    EtaCspIsrHandlerSet(eIrqNumDma0+ psDmaCmd->ui8Channel, DMA_ISR_LINE,
                        RetHandler);

    iEtaStatus = EtaCspDmaLaunch(psDmaCmd);

    return(iEtaStatus);
}

/***************************************************************************//**
 *
 *  EtaCspDmaChTerminate - Terminates a DMA
 *
 *  @param ui8Channel - Integer for Channel
 *
 *  @return Returns eEtaSuccess if all went well, else config error
 *
 ******************************************************************************/
tEtaStatus
EtaCspDmaChTerminate(uint_fast8_t ui8Channel)
{
    int retval;
    tEtaStatus iEtaStatus;
    p_hw_dma_regs_T dma_controller_ptr;

    dma_controller_ptr =
        (p_hw_dma_regs_T)(DMA_CONTROLLER_ADDRESS +
                          ((ui8Channel) * DMA_CHANNEL_STRIDE_SIZE));

    retval = dma_cancelTransfer(dma_controller_ptr);

    if(retval)
    {
        iEtaStatus = eEtaFailure;
    }
    else
    {
        iEtaStatus = eEtaSuccess;
    }

    return(iEtaStatus);
}

/***************************************************************************//**
 *
 *  EtaCspDmaCmdTerminate - Terminates a DMA
 *
 *  @param psDmaCmd - Pointer to tDmaCmd structure (only Channel is needed)
 *
 *  @return Returns eEtaSuccess if all went well, else config error
 *
 ******************************************************************************/
tEtaStatus
EtaCspDmaCmdTerminate(tDmaCmd *psDmaCmd)
{
    int retval;
    tEtaStatus iEtaStatus;
    p_hw_dma_regs_T dma_controller_ptr;

    dma_controller_ptr =
        (p_hw_dma_regs_T)(DMA_CONTROLLER_ADDRESS +
                          ((psDmaCmd->ui8Channel) * DMA_CHANNEL_STRIDE_SIZE));

    retval = dma_cancelTransfer(dma_controller_ptr);

    if(retval)
    {
        iEtaStatus = eEtaFailure;
    }
    else
    {
        iEtaStatus = eEtaSuccess;
    }

    return(iEtaStatus);
}

/***************************************************************************//**
 *
 *  EtaCspDmaCmdGetDefault - Return a default, simplified dma_cmd of
 *      tDmaCmd. This is used to provide backwards support to simple
 *      tDmaCmd
 *
 *  @return - initialized default tDmaCmd
 *
 *  Note: Arguably, the first few fields (assigned to 0x0) need not be defined
 *        here since they are so integral to the DMA that the user would surely
 *        assign them (or in the case of src/dst address may be don't cares).
 *        But it seems sane to eliminate variability so we'll assign everything
 *        even if it gets overriden.
 *
 *  Note: Generally, for "normal, no frills" DMA, only the first fiew fields
 *        (assigned to 0x0) are needed.
 *
 ******************************************************************************/
tDmaCmd
EtaCspDmaCmdGetDefault()
{
    tDmaCmd sDmaCmdDefault;

    sDmaCmdDefault.ui8Channel = 0x0;
    sDmaCmdDefault.iSrcTarget = 0x0;
    sDmaCmdDefault.iDstTarget = 0x0;
    sDmaCmdDefault.ui32SrcAddress = 0x0;
    sDmaCmdDefault.ui32DstAddress = 0x0;
    sDmaCmdDefault.ui16XferLength = 0x0;

    sDmaCmdDefault.ui16WordPerBlock = ETA_CSP_DMA_DEFAULT_WORDS_PER_BLOCK;
    sDmaCmdDefault.src_reload_per_block =
        ETA_CSP_DMA_DEFAULT_SRC_RELOAD_PER_BLOCK;
    sDmaCmdDefault.dst_reload_per_block =
        ETA_CSP_DMA_DEFAULT_DST_RELOAD_PER_BLOCK;
    sDmaCmdDefault.cmd_reload_per_xfer =
        ETA_CSP_DMA_DEFAULT_CMD_RELOAD_PER_XFER;
    sDmaCmdDefault.ui16TimeOut = ETA_CSP_DMA_DEFAULT_TIMEOUT;
    sDmaCmdDefault.ui8IntLevel = ETA_CSP_DMA_DEFAULT_INT_LEVEL;
    sDmaCmdDefault.ui8CircBuffTrig = 0x0;
    sDmaCmdDefault.circ_buff_src_reload = 0x0;
    sDmaCmdDefault.circ_buff_dst_reload = 0x0;
    sDmaCmdDefault.ui8PackCmd = BFV_DMA_TRIG_CONFIG_PACK_CMD_NO_PACK;
    sDmaCmdDefault.ui8SemSrcDstRdy = ETA_CSP_DMA_DEFAULT_SEM_SRCDSTRDY;
    return(sDmaCmdDefault);
}


/***************************************************************************//**
 *
 *  Launch DMA based off of *tDmaCmd*.
 *
 *  @param psDmaCmd  is the pointer to the *tDmaCmd* structure.
 *
 *  This function takes the struct of type *tDmaCmd*, configure the DMA
 *  channel as per the fields provided in the struct, and then launches the
 *  DMA tranfer. This function call returns after launching the DMA transfer.
 *  User can call EtaCspDmaWait() or EtaCspDmaChWait() to wait polling
 *  until the DMA transfer finishes, or enable the interrupts from DMA and
 *  use a ISR handle the completion of DMA transfer. DMA completion interrupt
 *  always fires. In addition, user can enable half-buffer completion
 *  interrupts by providing non-zero value to the ui8IntLevel field of the
 *  command struct. After a number of blocks provided in the ui8IntLevel
 *  is tranferred, the half-buffer interrupt fires.
 *
 *  > src_reload_per_block doesn't do anything useful when the source is
 *    *eDmaTargetIoMemI2s*, *eDmaTargetIoMemAdc*, *eDmaTargetIoMemSpi*,
 *    *eDmaTargetIoMemI2c*, *eDmaTargetIoMemPdm0*, or *eDmaTargetIoMemPdm1*.
 *    dst_reload_per_block doesn't do anything useful when the destination is
 *    *eDmaTargetIoMemI2s*, *eDmaTargetIoMemSpi*, or *eDmaTargetIoMemI2c*.
 *
 *  @return Returns eEtaSuccess if launch was successful, else eEtaFailure.
 *
 ******************************************************************************/
tEtaStatus
EtaCspDmaLaunch(tDmaCmd *psDmaCmdFull)
{
    uint16_t tempdata_config_src = 0;
    uint16_t tempdata_config_dst = 0;
    uint16_t tempdata_num_word_per_block = 0;
    uint16_t tempdata_src_addr;
    uint16_t tempdata_dst_addr;
    uint16_t tempdata_numblocks = 0;
    uint16_t tempdata_config_trig = 0;
    uint16_t tempdata_timer_config = 0;
    uint16_t tempdata_int_level = 0;
    uint32_t ahb_window_addr;
    tEtaStatus iEtaStatus;

    // Default Trigger is all 0s.
    // For now, we only support LEVEL (not edge) and ACTIVE HIGH triggers...
    // which is all 0, which is default.
    // Still, I'll program it all here for completeness.
    tempdata_config_trig |= (0 << BP_DMA_TRIG_CONFIG_SRC_POL);
    tempdata_config_trig |= (0 << BP_DMA_TRIG_CONFIG_SRC_EDGE);
    tempdata_config_trig |= (0 << BP_DMA_TRIG_CONFIG_SRC_BOTH_EDGES);
    tempdata_config_trig |= (0 << BP_DMA_TRIG_CONFIG_DST_POL);
    tempdata_config_trig |= (0 << BP_DMA_TRIG_CONFIG_DST_EDGE);
    tempdata_config_trig |= (0 << BP_DMA_TRIG_CONFIG_DST_BOTH_EDGES);

    // Common Config Trigger (Circular Buffer)
    tempdata_config_trig |=
        (psDmaCmdFull->ui8CircBuffTrig <<
    BP_DMA_TRIG_CONFIG_CIRC_BUFF_TRIG);
    tempdata_config_trig |=
        (psDmaCmdFull->circ_buff_dst_reload <<
            BP_DMA_TRIG_CONFIG_CIRC_BUFF_DST_RELOAD);
    tempdata_config_trig |=
        (psDmaCmdFull->circ_buff_src_reload <<
            BP_DMA_TRIG_CONFIG_CIRC_BUFF_SRC_RELOAD);

    // Common Config Trigger (Pack/Unpack)
    tempdata_config_trig |=
        (psDmaCmdFull->ui8PackCmd << BP_DMA_TRIG_CONFIG_PACK_CMD);

    // Common Config Source
    tempdata_config_src |= (1 << BP_DMA_CONFIG_SRC_SRCINC);
    tempdata_config_src |=
        (psDmaCmdFull->src_reload_per_block <<
            BP_DMA_CONFIG_SRC_SRC_RELOAD_PER_BLOCK);

    // Common Config Destination
    tempdata_config_dst |= (1 << BP_DMA_CONFIG_DST_DSTINC);
    tempdata_config_dst |=
        (psDmaCmdFull->dst_reload_per_block <<
            BP_DMA_CONFIG_DST_DST_RELOAD_PER_BLOCK);

    // Common Word Per Block
    tempdata_num_word_per_block |=
        ((psDmaCmdFull->ui16WordPerBlock - 1) <<
            BP_DMA_WORDS_PER_BLOCK_WORDS_PER_BLOCK);

    // Common Num Blocks
    tempdata_numblocks |= psDmaCmdFull->ui16XferLength <<
        BP_DMA_NUMBLOCKS_NUMBLOCKS;
    tempdata_numblocks |= psDmaCmdFull->cmd_reload_per_xfer <<
        BP_DMA_NUMBLOCKS_AUTO_RELOAD;

    if(psDmaCmdFull->ui8IntLevel != 0)
    {
        tempdata_numblocks |= 1 << BP_DMA_NUMBLOCKS_HALF_BUFFER_INT;
        tempdata_int_level |= psDmaCmdFull->ui8IntLevel <<
            BP_DMA_INT_LEVEL_INT_LEVEL;
    }

    tempdata_timer_config |= psDmaCmdFull->ui16TimeOut <<
        BP_DMA_TIMEOUT_TIMEOUT;

    if(psDmaCmdFull->iSrcTarget == eDmaTargetDynamicAhb)
    {

        // Note, we must round off the window to be 4 byte aligned for things to
        // work correctly.
        // Assign window to targeted address
        ahb_window_addr = psDmaCmdFull->ui32SrcAddress;
        psDmaCmdFull->ui32SrcAddress = AHB_WINDOW_IOMEM_ADDR(
                                                               psDmaCmdFull->ui8Channel)
                                         + (ahb_window_addr & 0x3) / 2;
        psDmaCmdFull->iSrcTarget = eDmaTargetIoMemWindow;

        iEtaStatus = EtaCspAhbWindowSet(psDmaCmdFull->ui8Channel,
                                        ahb_window_addr & 0xfffffffc);
    }

    if(psDmaCmdFull->iDstTarget == eDmaTargetDynamicAhb)
    {

        // Note, we must round off the window to be 4 byte aligned for things to
        // work correctly.
        // Assign window to targeted address
        ahb_window_addr = psDmaCmdFull->ui32DstAddress;
        psDmaCmdFull->ui32DstAddress = AHB_WINDOW_IOMEM_ADDR(
                                                               psDmaCmdFull->ui8Channel)
                                         + (ahb_window_addr & 0x3) / 2;
        psDmaCmdFull->iDstTarget = eDmaTargetIoMemWindow;

        iEtaStatus = EtaCspAhbWindowSet(psDmaCmdFull->ui8Channel,
                                        ahb_window_addr & 0xfffffffc);
    }

    // Configure source
    switch(psDmaCmdFull->iSrcTarget)
    {
        case eDmaTargetXmem:
        {
            tempdata_config_src |=
                (BFV_DMA_CONFIG_SRC_SRC_XMEM << BP_DMA_CONFIG_SRC_SRC);
            tempdata_config_src |=
                (SRCDST_RDY_ALWAYS1 << BP_DMA_CONFIG_SRC_SRC_FLOWCTRL);
            tempdata_config_src |= (2 << BP_DMA_CONFIG_SRC_SRC_STEP);
            tempdata_src_addr = psDmaCmdFull->ui32SrcAddress <<
                BP_DMA_SRC_ADDR_ADDRESS;
            break;
        }

        case eDmaTargetYmem:
        {
            tempdata_config_src |=
                (BFV_DMA_CONFIG_SRC_SRC_YMEM << BP_DMA_CONFIG_SRC_SRC);
            tempdata_config_src |=
                (SRCDST_RDY_ALWAYS1 << BP_DMA_CONFIG_SRC_SRC_FLOWCTRL);
            tempdata_config_src |= (2 << BP_DMA_CONFIG_SRC_SRC_STEP);
            tempdata_src_addr = psDmaCmdFull->ui32SrcAddress <<
                BP_DMA_SRC_ADDR_ADDRESS;
            break;
        }

        case eDmaTargetXmemByte:
        {
            tempdata_config_src |=
                (BFV_DMA_CONFIG_SRC_SRC_XMEM << BP_DMA_CONFIG_SRC_SRC);
            tempdata_config_src |=
                (SRCDST_RDY_ALWAYS1 << BP_DMA_CONFIG_SRC_SRC_FLOWCTRL);
            tempdata_config_src |= (1 << BP_DMA_CONFIG_SRC_SRC_STEP);
            tempdata_src_addr = psDmaCmdFull->ui32SrcAddress <<
                BP_DMA_SRC_ADDR_ADDRESS;
            break;
        }

        case eDmaTargetYmemByte:
        {
            tempdata_config_src |=
                (BFV_DMA_CONFIG_SRC_SRC_YMEM << BP_DMA_CONFIG_SRC_SRC);
            tempdata_config_src |=
                (SRCDST_RDY_ALWAYS1 << BP_DMA_CONFIG_SRC_SRC_FLOWCTRL);
            tempdata_config_src |= (1 << BP_DMA_CONFIG_SRC_SRC_STEP);
            tempdata_src_addr = psDmaCmdFull->ui32SrcAddress <<
                BP_DMA_SRC_ADDR_ADDRESS;
            break;
        }

        case eDmaTargetIoMemWindow:
        {
            tempdata_config_src |=
                (BFV_DMA_CONFIG_SRC_SRC_IOMEM < BP_DMA_CONFIG_SRC_SRC);
            tempdata_config_src |=
                (SRCDST_RDY_ALWAYS1 << BP_DMA_CONFIG_SRC_SRC_FLOWCTRL);
            tempdata_config_src |= (1 << BP_DMA_CONFIG_SRC_SRC_STEP);
            tempdata_src_addr = psDmaCmdFull->ui32SrcAddress <<
                BP_DMA_SRC_ADDR_ADDRESS;
            break;
        }

        case eDmaTargetIoMemI2s:
        {
            tempdata_config_src |=
                (BFV_DMA_CONFIG_SRC_SRC_IOMEM < BP_DMA_CONFIG_SRC_SRC);
            tempdata_config_src |=
                (SRCDST_RDY_I2S << BP_DMA_CONFIG_SRC_SRC_FLOWCTRL);
            tempdata_config_src |= (0 << BP_DMA_CONFIG_SRC_SRC_STEP);
            tempdata_src_addr = REG_I2S_DSP_RX_DATA_ADDR <<
                BP_DMA_SRC_ADDR_ADDRESS;
            break;
        }

        case eDmaTargetIoMemSpi:
        {
            tempdata_config_src |=
                (BFV_DMA_CONFIG_SRC_SRC_IOMEM < BP_DMA_CONFIG_SRC_SRC);
            tempdata_config_src |=
                (SRCDST_RDY_SPI << BP_DMA_CONFIG_SRC_SRC_FLOWCTRL);
            tempdata_config_src |= (0 << BP_DMA_CONFIG_SRC_SRC_STEP);
            tempdata_src_addr = REG_SPI2_TXRX_2BYTE_FIFO_DATA_ADDR <<
                BP_DMA_SRC_ADDR_ADDRESS;
            break;
        }

        case eDmaTargetIoMemI2c:
        {
            tempdata_config_src |=
                (BFV_DMA_CONFIG_SRC_SRC_IOMEM < BP_DMA_CONFIG_SRC_SRC);
            tempdata_config_src |=
                (SRCDST_RDY_I2C << BP_DMA_CONFIG_SRC_SRC_FLOWCTRL);
            tempdata_config_src |= (0 << BP_DMA_CONFIG_SRC_SRC_STEP);
            tempdata_src_addr = REG_I2C2_TXRX_1BYTE_FIFO_DATA_ADDR <<
                BP_DMA_SRC_ADDR_ADDRESS;
            break;
        }

        case eDmaTargetIoMemPdm0:
        {
            tempdata_config_src |=
                (BFV_DMA_CONFIG_SRC_SRC_IOMEM < BP_DMA_CONFIG_SRC_SRC);
            tempdata_config_src |=
                (SRCDST_RDY_PDM0 << BP_DMA_CONFIG_SRC_SRC_FLOWCTRL);
            tempdata_config_src |= (0 << BP_DMA_CONFIG_SRC_SRC_STEP);
            tempdata_src_addr = REG_PDM_DSP_RX_DATA_ADDR(0) <<
                BP_DMA_SRC_ADDR_ADDRESS;
            break;
        }

        case eDmaTargetIoMemPdm1:
        {
            tempdata_config_src |=
                (BFV_DMA_CONFIG_SRC_SRC_IOMEM < BP_DMA_CONFIG_SRC_SRC);
            tempdata_config_src |=
                (SRCDST_RDY_PDM1 << BP_DMA_CONFIG_SRC_SRC_FLOWCTRL);
            tempdata_config_src |= (0 << BP_DMA_CONFIG_SRC_SRC_STEP);
            tempdata_src_addr = REG_PDM_DSP_RX_DATA_ADDR(1) <<
                BP_DMA_SRC_ADDR_ADDRESS;
            break;
        }

        case eDmaTargetIoMemAdc:
        {
            tempdata_config_src |=
                (BFV_DMA_CONFIG_SRC_SRC_IOMEM < BP_DMA_CONFIG_SRC_SRC);
            tempdata_config_src |=
                (SRCDST_RDY_ADC << BP_DMA_CONFIG_SRC_SRC_FLOWCTRL);
            tempdata_config_src |= (0 << BP_DMA_CONFIG_SRC_SRC_STEP);
            tempdata_src_addr = REG_ADC_DSP_RX_DATA_ADDR <<
                BP_DMA_SRC_ADDR_ADDRESS;
            break;
        }

        default:
        {
            return(eEtaFailure);
            break;
        }
    }

    // Configure destination
    switch(psDmaCmdFull->iDstTarget)
    {
        case eDmaTargetXmem:
        {
            tempdata_config_dst |=
                (BFV_DMA_CONFIG_DST_DST_XMEM << BP_DMA_CONFIG_DST_DST);
            tempdata_config_dst |=
                (SRCDST_RDY_ALWAYS1 << BP_DMA_CONFIG_DST_DST_FLOWCTRL);
            tempdata_config_dst |= (2 << BP_DMA_CONFIG_DST_DST_STEP);
            tempdata_dst_addr = psDmaCmdFull->ui32DstAddress <<
                BP_DMA_DST_ADDR_ADDRESS;
            break;
        }

        case eDmaTargetYmem:
        {
            tempdata_config_dst |=
                (BFV_DMA_CONFIG_DST_DST_YMEM << BP_DMA_CONFIG_DST_DST);
            tempdata_config_dst |=
                (SRCDST_RDY_ALWAYS1 << BP_DMA_CONFIG_DST_DST_FLOWCTRL);
            tempdata_config_dst |= (2 << BP_DMA_CONFIG_DST_DST_STEP);
            tempdata_dst_addr = psDmaCmdFull->ui32DstAddress;
            break;
        }

        case eDmaTargetXmemByte:
        {
            tempdata_config_dst |=
                (BFV_DMA_CONFIG_DST_DST_XMEM << BP_DMA_CONFIG_DST_DST);
            tempdata_config_dst |=
                (SRCDST_RDY_ALWAYS1 << BP_DMA_CONFIG_DST_DST_FLOWCTRL);
            tempdata_config_dst |= (1 << BP_DMA_CONFIG_DST_DST_STEP);
            tempdata_dst_addr = psDmaCmdFull->ui32DstAddress <<
                BP_DMA_DST_ADDR_ADDRESS;
            break;
        }

        case eDmaTargetYmemByte:
        {
            tempdata_config_dst |=
                (BFV_DMA_CONFIG_DST_DST_YMEM << BP_DMA_CONFIG_DST_DST);
            tempdata_config_dst |=
                (SRCDST_RDY_ALWAYS1 << BP_DMA_CONFIG_DST_DST_FLOWCTRL);
            tempdata_config_dst |= (1 << BP_DMA_CONFIG_DST_DST_STEP);
            tempdata_dst_addr = psDmaCmdFull->ui32DstAddress <<
                BP_DMA_DST_ADDR_ADDRESS;
            break;
        }

        case eDmaTargetIoMemWindow:
        {
            tempdata_config_dst |=
                (BFV_DMA_CONFIG_DST_DST_IOMEM < BP_DMA_CONFIG_DST_DST);
            tempdata_config_dst |=
                (SRCDST_RDY_ALWAYS1 << BP_DMA_CONFIG_DST_DST_FLOWCTRL);
            tempdata_config_dst |= (1 << BP_DMA_CONFIG_DST_DST_STEP);
            tempdata_dst_addr = psDmaCmdFull->ui32DstAddress <<
                BP_DMA_DST_ADDR_ADDRESS;
            break;
        }

        case eDmaTargetIoMemI2s:
        {
            tempdata_config_dst |=
                (BFV_DMA_CONFIG_DST_DST_IOMEM < BP_DMA_CONFIG_DST_DST);
            tempdata_config_dst |=
                (SRCDST_RDY_I2S << BP_DMA_CONFIG_DST_DST_FLOWCTRL);
            tempdata_config_dst |= (0 << BP_DMA_CONFIG_DST_DST_STEP);
            tempdata_dst_addr = REG_I2S_DSP_TX_DATA_ADDR <<
                BP_DMA_DST_ADDR_ADDRESS;
            break;
        }

        case eDmaTargetIoMemSpi:
        {
            tempdata_config_dst |=
                (BFV_DMA_CONFIG_DST_DST_IOMEM < BP_DMA_CONFIG_DST_DST);
            tempdata_config_dst |=
                (SRCDST_RDY_SPI << BP_DMA_CONFIG_DST_DST_FLOWCTRL);
            tempdata_config_dst |= (0 << BP_DMA_CONFIG_DST_DST_STEP);
            tempdata_dst_addr = REG_SPI2_TXRX_2BYTE_FIFO_DATA_ADDR <<
                BP_DMA_DST_ADDR_ADDRESS;
            break;
        }

        case eDmaTargetIoMemI2c:
        {
            tempdata_config_dst |=
                (BFV_DMA_CONFIG_DST_DST_IOMEM < BP_DMA_CONFIG_DST_DST);
            tempdata_config_dst |=
                (SRCDST_RDY_I2C << BP_DMA_CONFIG_DST_DST_FLOWCTRL);
            tempdata_config_dst |= (0 << BP_DMA_CONFIG_DST_DST_STEP);
            tempdata_dst_addr = REG_I2C2_TX_1BYTE_DATA_CMD_FIFO_ADDR <<
                BP_DMA_DST_ADDR_ADDRESS;
            break;
        }

        case eDmaTargetIoMemPdm0: // unsupported
        case eDmaTargetIoMemPdm1: // unsupported
        case eDmaTargetIoMemAdc: // unsupported
        default:
        {
            return(eEtaFailure);
            break;
        }
    }

    // Add semaphore source / destinate ready if enabled
    if(psDmaCmdFull->ui8SemSrcDstRdy != ETA_CSP_DMA_DEFAULT_SEM_SRCDSTRDY) //
                                                                              // If
                                                                              // not
                                                                              // default,
                                                                              // then
                                                                              // we
                                                                              // intend
                                                                              // to
                                                                              // use
                                                                              // semaphore
                                                                              // src
                                                                              // or
                                                                              // dst
                                                                              // ready
    {
        if(psDmaCmdFull->iDstTarget == eDmaTargetIoMemWindow)
        {
            tempdata_config_dst &= ~(BM_DMA_CONFIG_DST_DST_FLOWCTRL);
            tempdata_config_dst |=
                (SRCDST_RDY_CSR12 + psDmaCmdFull->ui8SemSrcDstRdy) <<
                    BP_DMA_CONFIG_DST_DST_FLOWCTRL;
        }
        else if(psDmaCmdFull->iSrcTarget == eDmaTargetIoMemWindow)
        {
            tempdata_config_src &= ~(BM_DMA_CONFIG_SRC_SRC_FLOWCTRL);
            tempdata_config_src |=
                (SRCDST_RDY_CSR12 + psDmaCmdFull->ui8SemSrcDstRdy) <<
                    BP_DMA_CONFIG_SRC_SRC_FLOWCTRL;
        }
        else
        {
            return(eEtaFailure);
        }
    }

    // Perform actual configuration
    REG_DMA_CONFIG_SRC(psDmaCmdFull->ui8Channel) = tempdata_config_src;
    REG_DMA_CONFIG_DST(psDmaCmdFull->ui8Channel) = tempdata_config_dst;
    REG_DMA_WORDS_PER_BLOCK(psDmaCmdFull->ui8Channel) =
        tempdata_num_word_per_block;
    REG_DMA_SRC_ADDR(psDmaCmdFull->ui8Channel) = tempdata_src_addr;
    REG_DMA_DST_ADDR(psDmaCmdFull->ui8Channel) = tempdata_dst_addr;
    REG_DMA_TRIG_CONFIG(psDmaCmdFull->ui8Channel) = tempdata_config_trig;
    REG_DMA_TIMEOUT(psDmaCmdFull->ui8Channel) = tempdata_timer_config;
    REG_DMA_INT_LEVEL(psDmaCmdFull->ui8Channel) = tempdata_int_level;

    // Kick off DMA
    REG_DMA_NUMBLOCKS(psDmaCmdFull->ui8Channel) = tempdata_numblocks;

    return(eEtaSuccess);
}

/***************************************************************************//**
 *
 *  EtaCspDmaCheckForCircOverflow - Check if Circular DMA has overflowed.
 *
 *  @param ui16AppBuffCount   - application variable counting number of buffers
 *that have been DMA'd.
 *  @param ui16AppProcCount   - application variable counting number of buffers
 *that have been Processed and can be reused.
 *  @param ui8DmaChannel      - DMA Channel the Circular Buffer is on.
 *  @param ui16DmaXferLength   - DMA transfer length
 *
 *  @return Command Return Value which will indicate all is well or we
 *overflowed
 *
 *  Note: For best use, place this before the buffer processor routine
 *increments the ui16AppProcCount. This will ensure
 *        may result in a few false overflows, but should catch all the real
 *ones. Better safe than sorry.
 *
 ******************************************************************************/
tDmaCmdRetVal
EtaCspDmaCheckForCircOverflow(uint16_t ui16AppBuffCount, uint16_t ui16AppProcCount,
                              uint_fast8_t ui8DmaChannel, uint16_t ui16DmaXferLength)
{
    uint8_t circ_buff_trig;
    uint8_t circ_dma_in_prog;
    uint16_t buffers_in_use;

    // Get buffer count in DMA configuration
    circ_buff_trig =
        (REG_DMA_TRIG_CONFIG(ui8DmaChannel) &
         BM_DMA_TRIG_CONFIG_CIRC_BUFF_TRIG) >>
        BP_DMA_TRIG_CONFIG_CIRC_BUFF_TRIG;

    // Lets see if we have DMA'd anything into the current buffer (not yet part
    // of ui16AppBuffCount, but still in use)
    circ_dma_in_prog =
        ((REG_DMA_NUMBLOCKS(ui8DmaChannel) & BM_DMA_NUMBLOCKS_NUMBLOCKS) >>
         BP_DMA_NUMBLOCKS_NUMBLOCKS) != ui16DmaXferLength;

    // This is the number of buffers that have been fully or partially received
    // but not processed.
    buffers_in_use = ui16AppBuffCount + circ_dma_in_prog - ui16AppProcCount;

    // So, if buffers in use is greater than the number of buffers in the
    // circular buffer ring, then we've overflowed.
    if(buffers_in_use > circ_buff_trig)
    {
        return(eDmaCmdCircBuffOverflow);
    }

    // No overflow
    return(eDmaCmdGood);
}

/***************************************************************************//**
 *
 *  EtaCspDmaCircBuffIncCount - Check how many Circular Buffers have been
 *received
 *
 *  @param ui16AppBuffCount   - application variable counting number of buffers
 *that have been received.
 *  @param ui8DmaChannel      - DMA Channel the Circular Buffer is on.
 *
 *  @return Updated application buffer count
 *
 *  Note: This is expected to be called after a DMA interrupt, and assumes we
 *have at least one
 *        buffer available.
 *
 ******************************************************************************/
uint16_t
EtaCspDmaCircBuffIncCount(uint16_t ui16AppBuffCount, uint_fast8_t ui8DmaChannel)
{
    uint8_t curr_circ_buff;
    uint8_t circ_buff_trig;

    ui16AppBuffCount++;

    ////////////////////////////////////////////////////////////////////
    // Check if we missed an interrupt (because we were too busy)

    // Get current rollign counter.
    curr_circ_buff =
        (REG_DMA_STATUS(ui8DmaChannel) & BM_DMA_STATUS_CIRC_BUFF_CURR) >>
        BP_DMA_STATUS_CIRC_BUFF_CURR;

    // Get buffer count in DMA configuration
    circ_buff_trig =
        (REG_DMA_TRIG_CONFIG(ui8DmaChannel) &
         BM_DMA_TRIG_CONFIG_CIRC_BUFF_TRIG) >>
        BP_DMA_TRIG_CONFIG_CIRC_BUFF_TRIG;

    if((ui16AppBuffCount % circ_buff_trig) != curr_circ_buff) // If different, we
                                                            // are missing
                                                            // updates
    {
        while((ui16AppBuffCount % circ_buff_trig) != curr_circ_buff)
        {
            ui16AppBuffCount++;
        }
    }

    // Check if we missed an interrupt (because we were too busy)
    ////////////////////////////////////////////////////////////////////

    return(ui16AppBuffCount);
}

/***************************************************************************//**
 *
 *  EtaCspDmaUnpackLaunch - Launch DMA from M3 memory space with unpack feature.
 *
 *  @param ui8Channel        - Integer to choose AHB window and DMA channel.
 *  @param ui32AhbByteAddr   - M3 memory space byte address.
 *  @param iDstTarget        - eDmaTargetXmem or eDmaTargetYmem.
 *  @param ui32DstAddr       - Address of the local buffer head.
 *  @param ui16XferLength    - No. of 16 bit block tranfers.
 *  @param iPackCmd          - Unpack command (eDmaPackCmdZeroPad, eDmaPackCmdSignExt)
 *
 *  > Source is always the the M3 memory space via the AHB window.
 *
 *  @return Returns eEtaSuccess if launch was successful, else eEtaFailure.
 *
 ******************************************************************************/
tEtaStatus
EtaCspDmaUnpackLaunch(uint_fast8_t ui8Channel, uint32_t ui32AhbByteAddr,
                      tDmaTarget iDstTarget, uint32_t ui32DstAddr,
                      uint16_t ui16XferLength,
                      tDmaPackCmd iPackCmd)
{
  tEtaStatus iEtaStatus;

  tDmaCmd sDmaCmd=EtaCspDmaCmdGetDefault();

  sDmaCmd.iSrcTarget=eDmaTargetDynamicAhb;
  sDmaCmd.ui32SrcAddress=ui32AhbByteAddr;
  sDmaCmd.iDstTarget=iDstTarget;
  sDmaCmd.ui32DstAddress=ui32DstAddr;
  sDmaCmd.ui8Channel=ui8Channel;
  sDmaCmd.ui16XferLength=ui16XferLength;
  sDmaCmd.ui8PackCmd=iPackCmd;
  iEtaStatus = EtaCspDmaLaunch(&sDmaCmd);
  return iEtaStatus;
}

/***************************************************************************//**
 *
 *  EtaCspDmaUnpackLaunchAndWait - Launch DMA from M3 memory space with unpack
 *feature and wait till tranfer completes.
 *
 *  @param ui8Channel        - Integer to choose AHB window and DMA channel.
 *  @param ui32AhbByteAddr   - M3 memory space byte address.
 *  @param iDstTarget        - eDmaTargetXmem or eDmaTargetYmem.
 *  @param ui32DstAddr       - Address of the local buffer head.
 *  @param ui16XferLength    - No. of 16 bit block tranfers.
 *  @param iPackCmd          - Unpack command (eDmaPackCmdZeroPad, eDmaPackCmdSignExt)
 *
 *  > Source is always the the M3 memory space via the AHB window.
 *
 *  @return Returns eEtaSuccess if launch was successful, else eEtaFailure.
 *
 ******************************************************************************/
tEtaStatus
EtaCspDmaUnpackLaunchAndWait(uint_fast8_t ui8Channel, uint32_t ui32AhbByteAddr,
                             tDmaTarget iDstTarget, uint32_t ui32DstAddr,
                             uint16_t ui16XferLength,
                             tDmaPackCmd iPackCmd)
{
  tEtaStatus iEtaStatus;

  tDmaCmd sDmaCmd=EtaCspDmaCmdGetDefault();

  sDmaCmd.iSrcTarget=eDmaTargetDynamicAhb;
  sDmaCmd.ui32SrcAddress=ui32AhbByteAddr;
  sDmaCmd.iDstTarget=iDstTarget;
  sDmaCmd.ui32DstAddress=ui32DstAddr;
  sDmaCmd.ui8Channel=ui8Channel;
  sDmaCmd.ui16XferLength=ui16XferLength;
  sDmaCmd.ui8PackCmd=iPackCmd;
  iEtaStatus = EtaCspDmaLaunchAndWait(&sDmaCmd);
  return iEtaStatus;
}

/***************************************************************************//**
 *
 *  EtaCspDmaPackLaunch - Launch DMA to M3 memory space with pack feature.
 *
 *  @param ui8Channel        - Integer to choose AHB window and DMA channel.
 *  @param ui32AhbByteAddr   - M3 memory space byte address.
 *  @param iSrcTarget        - eDmaTargetXmem or eDmaTargetYmem.
 *  @param ui32SrcAddr       - Address of the local buffer head.
 *  @param ui16XferLength    - No. of 16 bit block tranfers.
 *
 *  > Destination is always the the M3 memory space via the AHB window.
 *
 *  @return Returns eEtaSuccess if launch was successful, else eEtaFailure.
 *
 ******************************************************************************/
tEtaStatus
EtaCspDmaPackLaunch (uint_fast8_t ui8Channel, uint32_t ui32AhbByteAddr,
                     tDmaTarget iSrcTarget, uint32_t ui32SrcAddr,
                     uint16_t ui16XferLength)
{
  tEtaStatus iEtaStatus;

  tDmaCmd sDmaCmd=EtaCspDmaCmdGetDefault();

  sDmaCmd.iSrcTarget=iSrcTarget;
  sDmaCmd.ui32SrcAddress=ui32SrcAddr;
  sDmaCmd.iDstTarget=eDmaTargetDynamicAhb;
  sDmaCmd.ui32DstAddress=ui32AhbByteAddr;
  sDmaCmd.ui8Channel=ui8Channel;
  sDmaCmd.ui16XferLength=ui16XferLength;
  sDmaCmd.ui8PackCmd=eDmaPackCmdRepack;
  iEtaStatus = EtaCspDmaLaunch(&sDmaCmd);
  return iEtaStatus;
}

/***************************************************************************//**
 *
 *  EtaCspDmaPackLaunchAndWait - Launch DMA to M3 memory space with pack
 *feature and wait till tranfer completes.
 *
 *  @param ui8Channel        - Integer to choose AHB window and DMA channel.
 *  @param ui32AhbByteAddr   - M3 memory space byte address.
 *  @param iSrcTarget        - eDmaTargetXmem or eDmaTargetYmem.
 *  @param ui32SrcAddr       - Address of the local buffer head.
 *  @param ui16XferLength    - No. of 16 bit block tranfers.
 *
 *  > Destination is always the the M3 memory space via the AHB window.
 *
 *  @return Returns eEtaSuccess if launch was successful, else eEtaFailure.
 *
 ******************************************************************************/
tEtaStatus
EtaCspDmaPackLaunchAndWait (uint_fast8_t ui8Channel, uint32_t ui32AhbByteAddr,
                            tDmaTarget iSrcTarget, uint32_t ui32SrcAddr,
                            uint16_t ui16XferLength)
{
  tEtaStatus iEtaStatus;

  tDmaCmd sDmaCmd=EtaCspDmaCmdGetDefault();

  sDmaCmd.iSrcTarget=iSrcTarget;
  sDmaCmd.ui32SrcAddress=ui32SrcAddr;
  sDmaCmd.iDstTarget=eDmaTargetDynamicAhb;
  sDmaCmd.ui32DstAddress=ui32AhbByteAddr;
  sDmaCmd.ui8Channel=ui8Channel;
  sDmaCmd.ui16XferLength=ui16XferLength;
  sDmaCmd.ui8PackCmd=eDmaPackCmdRepack;
  iEtaStatus = EtaCspDmaLaunchAndWait(&sDmaCmd);
  return iEtaStatus;
}

// This was the original launch DMA, preserved here for posterity
// The old version took 348 clocks.
// The new version takes 72 clocks (and has more features).
// int eta_csp_launch_dma_old (eta_dma_cmd_T *p_cmd_full_struct)
// {
// boolean_T int_level_flag;
// p_hw_dma_regs_T  dma_controller_ptr;
// dma_controller_ptr = (p_hw_dma_regs_T)(DMA_CONTROLLER_ADDRESS +
// ((p_cmd_full_struct->channel) * DMA_CHANNEL_STRIDE_SIZE));
//
// if (p_cmd_full_struct->ui16XferLength > (1024*16)-1)
// {
// ETA_CSP_DMA_DEBUG etaPrintf ("Unsupported xfer_length at line %d of file
// %s\r\n", __LINE__, __FILE__);
// return -1;
// }
//
//// Configure source
// switch (p_cmd_full_struct->src_target)
// {
// case DMA_XMEM :
// if (dma_configLevelSensitiveReadySignal (dma_controller_ptr, SOURCE,
// SRCDST_RDY_ALWAYS1, ACTIVE_HIGH)) return -1;
// if (dma_configParty(dma_controller_ptr, SOURCE, CF_XMEM,
// p_cmd_full_struct->src_address, 2, 0,
// p_cmd_full_struct->src_reload_per_block)) return -1;
// break;
// case DMA_YMEM :
// if (dma_configLevelSensitiveReadySignal (dma_controller_ptr, SOURCE,
// SRCDST_RDY_ALWAYS1, ACTIVE_HIGH)) return -1;
// if (dma_configParty(dma_controller_ptr, SOURCE, CF_YMEM,
// p_cmd_full_struct->src_address, 2, 0,
// p_cmd_full_struct->src_reload_per_block)) return -1;
// break;
// case DMA_XMEM_BYTE :
// if (dma_configLevelSensitiveReadySignal (dma_controller_ptr, SOURCE,
// SRCDST_RDY_ALWAYS1, ACTIVE_HIGH)) return -1;
// if (dma_configParty(dma_controller_ptr, SOURCE, CF_XMEM,
// p_cmd_full_struct->src_address, 1, 0,
// p_cmd_full_struct->src_reload_per_block)) return -1;
// break;
// case DMA_YMEM_BYTE :
// if (dma_configLevelSensitiveReadySignal (dma_controller_ptr, SOURCE,
// SRCDST_RDY_ALWAYS1, ACTIVE_HIGH)) return -1;
// if (dma_configParty(dma_controller_ptr, SOURCE, CF_YMEM,
// p_cmd_full_struct->src_address, 1, 0,
// p_cmd_full_struct->src_reload_per_block)) return -1;
// break;
// case DMA_IOMEM_WINDOW :
// if (dma_configLevelSensitiveReadySignal (dma_controller_ptr, SOURCE,
// SRCDST_RDY_ALWAYS1, ACTIVE_HIGH)) return -1;
// if (dma_configParty(dma_controller_ptr, SOURCE, CF_IOMEM,
// p_cmd_full_struct->src_address, 1, 0,
// p_cmd_full_struct->src_reload_per_block)) return -1;
// break;
// case DMA_IOMEM_I2S :
// if (dma_configLevelSensitiveReadySignal (dma_controller_ptr, SOURCE,
// SRCDST_RDY_I2S, ACTIVE_HIGH)) return -1;
// if (dma_configParty(dma_controller_ptr, SOURCE, CF_IOMEM,
// REG_I2S_DSP_RX_DATA_ADDR, 0, 0, p_cmd_full_struct->src_reload_per_block))
// return -1;
// break;
// case DMA_IOMEM_PDM0 :
// if (dma_configLevelSensitiveReadySignal (dma_controller_ptr, SOURCE,
// SRCDST_RDY_PDM0, ACTIVE_HIGH)) return -1;
// if (dma_configParty(dma_controller_ptr, SOURCE, CF_IOMEM,
// REG_PDM_DSP_RX_DATA_ADDR(0), 0, 0, p_cmd_full_struct->src_reload_per_block))
// return -1;
// break;
// case DMA_IOMEM_PDM1 :
// if (dma_configLevelSensitiveReadySignal (dma_controller_ptr, SOURCE,
// SRCDST_RDY_PDM1, ACTIVE_HIGH)) return -1;
// if (dma_configParty(dma_controller_ptr, SOURCE, CF_IOMEM,
// REG_PDM_DSP_RX_DATA_ADDR(1), 0, 0, p_cmd_full_struct->src_reload_per_block))
// return -1;
// break;
// case DMA_IOMEM_ADC :
// if (dma_configLevelSensitiveReadySignal (dma_controller_ptr, SOURCE,
// SRCDST_RDY_ADC, ACTIVE_HIGH)) return -1;
// if (dma_configParty(dma_controller_ptr, SOURCE, CF_IOMEM,
// REG_ADC_DSP_RX_DATA_ADDR, 0, 0, p_cmd_full_struct->src_reload_per_block))
// return -1;
// break;
// default   :
// ETA_CSP_DMA_DEBUG etaPrintf ("Unsupported option at line %d of file %s\r\n",
// __LINE__, __FILE__);
// return -1;
// break;
// }
//
//// Configure destination
// switch (p_cmd_full_struct->dst_target)
// {
// case DMA_XMEM :
// if (dma_configLevelSensitiveReadySignal (dma_controller_ptr, DESTINATION,
// SRCDST_RDY_ALWAYS1, ACTIVE_HIGH)) return -1;
// if (dma_configParty(dma_controller_ptr, DESTINATION, CF_XMEM,
// p_cmd_full_struct->dst_address, 2, 0,
// p_cmd_full_struct->dst_reload_per_block)) return -1;
// break;
// case DMA_YMEM :
// if (dma_configLevelSensitiveReadySignal (dma_controller_ptr, DESTINATION,
// SRCDST_RDY_ALWAYS1, ACTIVE_HIGH)) return -1;
// if (dma_configParty(dma_controller_ptr, DESTINATION, CF_YMEM,
// p_cmd_full_struct->dst_address, 2, 0,
// p_cmd_full_struct->dst_reload_per_block)) return -1;
// break;
// case DMA_XMEM_BYTE :
// if (dma_configLevelSensitiveReadySignal (dma_controller_ptr, DESTINATION,
// SRCDST_RDY_ALWAYS1, ACTIVE_HIGH)) return -1;
// if (dma_configParty(dma_controller_ptr, DESTINATION, CF_XMEM,
// p_cmd_full_struct->dst_address, 1, 0,
// p_cmd_full_struct->dst_reload_per_block)) return -1;
// break;
// case DMA_YMEM_BYTE :
// if (dma_configLevelSensitiveReadySignal (dma_controller_ptr, DESTINATION,
// SRCDST_RDY_ALWAYS1, ACTIVE_HIGH)) return -1;
// if (dma_configParty(dma_controller_ptr, DESTINATION, CF_YMEM,
// p_cmd_full_struct->dst_address, 1, 0,
// p_cmd_full_struct->dst_reload_per_block)) return -1;
// break;
// case DMA_IOMEM_WINDOW :
// if (dma_configLevelSensitiveReadySignal (dma_controller_ptr, DESTINATION,
// SRCDST_RDY_ALWAYS1, ACTIVE_HIGH)) return -1;
// if (dma_configParty(dma_controller_ptr, DESTINATION, CF_IOMEM,
// p_cmd_full_struct->dst_address, 1, 0,
// p_cmd_full_struct->dst_reload_per_block)) return -1;
// break;
// case DMA_IOMEM_I2S :
// if (dma_configLevelSensitiveReadySignal (dma_controller_ptr, DESTINATION,
// SRCDST_RDY_I2S, ACTIVE_HIGH)) return -1;
// if (dma_configParty(dma_controller_ptr, DESTINATION, CF_IOMEM,
// REG_I2S_DSP_TX_DATA_ADDR, 0, 0, p_cmd_full_struct->dst_reload_per_block))
// return -1;
// break;
// case DMA_IOMEM_PDM0 :
// if (dma_configLevelSensitiveReadySignal (dma_controller_ptr, SOURCE,
// SRCDST_RDY_PDM0, ACTIVE_HIGH)) return -1;
// if (dma_configParty(dma_controller_ptr, SOURCE, CF_IOMEM,
// REG_PDM_DSP_TX_DATA_ADDR(0), 0, 0, p_cmd_full_struct->src_reload_per_block))
// return -1;
// break;
// case DMA_IOMEM_PDM1 :
// if (dma_configLevelSensitiveReadySignal (dma_controller_ptr, SOURCE,
// SRCDST_RDY_PDM1, ACTIVE_HIGH)) return -1;
// if (dma_configParty(dma_controller_ptr, SOURCE, CF_IOMEM,
// REG_PDM_DSP_TX_DATA_ADDR(1), 0, 0, p_cmd_full_struct->src_reload_per_block))
// return -1;
// break;
// case DMA_IOMEM_ADC : // unsupported
// default   :
// ETA_CSP_DMA_DEBUG etaPrintf ("Unsupported option at line %d of file %s\r\n",
// __LINE__, __FILE__);
// return -1;
// break;
// }
//
// if (p_cmd_full_struct->int_level) // If non-zero int_level
// int_level_flag = DMA_TRUE;
// else
// int_level_flag = DMA_FALSE;
//
// if (dma_changeHalfBufferInteruptLevel(dma_controller_ptr, int_level_flag,
// p_cmd_full_struct->int_level)) return -1;
//
// if (dma_setDMATimer(dma_controller_ptr, p_cmd_full_struct->timeout)) return
// -1;
//
// if (dma_configCircularBuffer(dma_controller_ptr,
// p_cmd_full_struct->circ_buff_trig, p_cmd_full_struct->circ_buff_dst_reload,
// p_cmd_full_struct->circ_buff_src_reload)) return -1;
//
// if (dma_configBlock(dma_controller_ptr, p_cmd_full_struct->ui16XferLength,
// p_cmd_full_struct->word_per_block, p_cmd_full_struct->cmd_reload_per_xfer))
// return -1;
//
// return 0;
// }

