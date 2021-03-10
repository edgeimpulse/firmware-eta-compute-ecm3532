/***************************************************************************//**
 *
 * @file eta_csp_pdm.c
 *
 * @brief This file contains eta_csp_pdm module implementations.
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
 * @addtogroup ecm3532pdm-dsp Pulse Density Modulation (PDM)
 * @ingroup ecm3532csp-dsp
 * @{
 ******************************************************************************/

#include "reg.h"
#include "eta_csp_pdm.h"
#include "reg_eta_ecm3532_dsp.h"
#include "eta_csp_mailbox.h"
#include "eta_csp_io.h"

/***************************************************************************//**
 *
 *  Initialize the PDM module.
 *
 *  @param sPdmConfig is a union. To access the field: enable, for example,
 *  use sPdmConfig.fields.enable.
 *
 *  This function initializes the PDM. It configures and/or enables it.
 *  User may do both in a single call or in 2 seperate calls.
 *
 *  @return Returns the configuration status. ePdmConfigGood if no errors.
 *
 ******************************************************************************/
tPdmConfigRetVal
EtaCspPdmConfig(tPdmConfig sPdmConfig)
{
    uint32_t ui32TempData0;
    uint8_t ui8Instance;


    // Tell M3 To Enable / Config PDM
    EtaCspMboxDsp2M3(MAILBOX_DSP2M3CMD_PDM_CFG, sPdmConfig.packed_config);

    ///////////////////////////////////////////////
    // Determine PDM Instance
    if(sPdmConfig.fields.enable && (sPdmConfig.fields.port_select == ePdmPort0))
    {
        ui8Instance = 0;
    }
    else if(sPdmConfig.fields.enable &&
            (sPdmConfig.fields.port_select == ePdmPort0Alt))
    {
        ui8Instance = 0;
    }
    else if(sPdmConfig.fields.enable &&
            (sPdmConfig.fields.port_select == ePdmPort1))
    {
        ui8Instance = 1;
    }
    else
    {
        return(ePdmConfigInvalidConfig);
    }

    // Determine PDM Instance
    ///////////////////////////////////////////////

    ///////////////////////////////////////////////
    // Configure Actual PDM

    ui32TempData0 = REG_PDM_DSP_CSR(ui8Instance);

    ui32TempData0 &= ~(BM_PDM_DSP_CSR_RX_CH_MODE        |
                   BM_PDM_DSP_CSR_SRSRDY_EDGE_TRIG  |
                   BM_PDM_DSP_CSR_SWAP_RD_CHNL      |
                   BM_PDM_DSP_CSR_PDM_EN);

    ui32TempData0 |=
        (sPdmConfig.fields.chan_config << BP_PDM_DSP_CSR_RX_CH_MODE)       |
        (sPdmConfig.fields.swap_chan <<
            BP_PDM_DSP_CSR_SWAP_RD_CHNL)     |
        (sPdmConfig.fields.edge_trig <<
            BP_PDM_DSP_CSR_SRSRDY_EDGE_TRIG);

    REG_PDM_DSP_CSR(ui8Instance) = ui32TempData0;

    // Set Enable after the rest of configurtion is in place.
    REG_PDM_DSP_CSR(ui8Instance) = ui32TempData0 |
                                (sPdmConfig.fields.enable <<
    BP_PDM_DSP_CSR_PDM_EN);

    // Configure Actual PDM
    ///////////////////////////////////////////////

    return(ePdmConfigGood);
}

/***************************************************************************//**
 *
 *  Read the RX buffer overflow flag.
 *
 *  @param ui16Instance is the pdm instance to read from (0,1).
 *
 *  @return Returns overflow flag.
 *
 ******************************************************************************/
uint_fast8_t
EtaCspPdmRxOfFlagRd(uint16_t ui16Instance)
{
    return (REG_PDM_DSP_STATUS(ui16Instance) & BM_PDM_DSP_STATUS_RX_OF);
}

/***************************************************************************//**
 *
 *  Read the RX buffer underflow flag.
 *
 *  @param ui16Instance is the pdm instance to read from (0,1).
 *
 *  @return Returns underflow flag.
 *
 ******************************************************************************/
uint_fast8_t
EtaCspPdmRxUfFlagRd(uint16_t ui16Instance)
{
    return (REG_PDM_DSP_STATUS(ui16Instance) & BM_PDM_DSP_STATUS_RX_UF);
}

