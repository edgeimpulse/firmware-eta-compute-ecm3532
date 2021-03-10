/***************************************************************************//**
 *
 * @file eta_csp_i2s.c
 *
 * @brief This file contains eta_csp_i2s module implementations.
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
 * @addtogroup ecm3532i2s-dsp Inter-IC Sound (I2S)
 * @ingroup ecm3532csp-dsp
 * @{
 ******************************************************************************/

#include "reg.h"
#include "eta_csp_i2s.h"
#include "reg_eta_ecm3532_dsp.h"
#include "eta_csp_mailbox.h"
#include "eta_csp_io.h"

/***************************************************************************//**
 *
 *  Initialize the I2S module.
 *
 *  @param sI2sConfig is a union. To access the field: enable, for example,
 *  use sI2sConfig.fields.enable.
 *
 *  This function initializes the I2S. It configures and/or enables it.
 *  User may do both in a single call or 2 seperate calls.
 *
 *  @return Returns the configuration status. eI2sConfigGood if no errors.
 *
 ******************************************************************************/
tI2sConfigRetVal
EtaCspI2sConfig(tI2sConfig sI2sConfig)
{
    uint32_t ui32TempData0;
    uint32_t ui32MboxWord;


    //
    // Form mbox word
    //
    ui32MboxWord = ((((uint32_t)sI2sConfig.fields.enable_rx)       << BP_ETA_CSP_COMMON_I2S_RX_ENBLE) |
                    (((uint32_t)sI2sConfig.fields.enable_tx)       << BP_ETA_CSP_COMMON_I2S_TX_ENBLE) |
                    (((uint32_t)sI2sConfig.fields.swap_chan_tx)    << BP_ETA_CSP_COMMON_I2S_TX_SWP_C) |
                    (((uint32_t)sI2sConfig.fields.swap_chan_rx)    << BP_ETA_CSP_COMMON_I2S_RX_SWP_C) |
                    (((uint32_t)sI2sConfig.fields.mode_64bit)      << BP_ETA_CSP_COMMON_I2S_64B_MODE) |
                    (((uint32_t)sI2sConfig.fields.edge_tx_trig)    << BP_ETA_CSP_COMMON_I2S_TX_ED_TR) |
                    (((uint32_t)sI2sConfig.fields.edge_rx_trig)    << BP_ETA_CSP_COMMON_I2S_RX_ED_TR) |
                    (((uint32_t)sI2sConfig.fields.clock_src)       << BP_ETA_CSP_COMMON_I2S_CLCK_SRC) |
                    (((uint32_t)sI2sConfig.fields.sample_freq)     << BP_ETA_CSP_COMMON_I2S_SMPL_FRQ) |
                    (((uint32_t)sI2sConfig.fields.chan_tx_config)  << BP_ETA_CSP_COMMON_I2S_TX_CONFG) |
                    (((uint32_t)sI2sConfig.fields.chan_rx_config)  << BP_ETA_CSP_COMMON_I2S_RX_CONFG) |
                    (((uint32_t)sI2sConfig.fields.chan_tx_upscale) << BP_ETA_CSP_COMMON_I2S_TX_UPSCL) |
                    (((uint32_t)sI2sConfig.fields.chan_rx_upscale) << BP_ETA_CSP_COMMON_I2S_RX_UPSCL) |
                    (((uint32_t)sI2sConfig.fields.int_mode)        << BP_ETA_CSP_COMMON_I2S_INT_MODE));

    ///////////////////////////////////////////////
    // Configure Actual I2S

    //// Trigger gratuitous Soft Reset
    // REG_I2S_DSP_CSR = BM_I2S_DSP_CSR_SOFT_RST ;
    // while (REG_I2S_DSP_CSR == BM_I2S_DSP_CSR_SOFT_RST);

    REG_I2S_DSP_CSR2 =
        (sI2sConfig.fields.chan_tx_upscale << BP_I2S_DSP_CSR2_TX_UPSCALE) |
        (sI2sConfig.fields.chan_rx_upscale <<
    BP_I2S_DSP_CSR2_RX_UPSCALE);

    ui32TempData0 =
        (sI2sConfig.fields.mode_64bit << BP_I2S_DSP_CSR_BIT64_MODE)       |
        (sI2sConfig.fields.chan_tx_config <<
    BP_I2S_DSP_CSR_TX_CH_MODE)       |
        (sI2sConfig.fields.chan_rx_config <<
    BP_I2S_DSP_CSR_RX_CH_MODE)       |
        (sI2sConfig.fields.int_mode << BP_I2S_DSP_CSR_INT_MODE)         |
        (sI2sConfig.fields.edge_tx_trig <<
    BP_I2S_DSP_CSR_DSTRDY_EDGE_TRIG) |
        (sI2sConfig.fields.edge_rx_trig <<
    BP_I2S_DSP_CSR_SRSRDY_EDGE_TRIG) |
        (sI2sConfig.fields.swap_chan_tx <<
    BP_I2S_DSP_CSR_SWAP_WR_CHNL)     |
        (sI2sConfig.fields.swap_chan_rx << BP_I2S_DSP_CSR_SWAP_RD_CHNL);

    REG_I2S_DSP_CSR =  ui32TempData0;

    // Perform enable after rest of configuration is stable
    if(sI2sConfig.fields.enable_rx || sI2sConfig.fields.enable_tx)
    {
        REG_I2S_DSP_CSR = ui32TempData0 | (0x1 << BP_I2S_DSP_CSR_I2S_EN);
    }

    // Tell M3 To Enable / Config I2S
    //EtaCspMboxDsp2M3(MAILBOX_DSP2M3CMD_I2S_CFG, sI2sConfig.packed_config);
    EtaCspMboxDsp2M3(MAILBOX_DSP2M3CMD_I2S_CFG, ui32MboxWord);

    // Configure Actual I2S
    ///////////////////////////////////////////////

    return(eI2sConfigGood);
}

