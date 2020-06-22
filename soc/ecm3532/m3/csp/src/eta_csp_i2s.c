/***************************************************************************//**
 *
 * @file eta_csp_i2s.c
 *
 * @brief This file contains eta_csp_i2s module implementations.
 *
 * Copyright (C) 2018 Eta Compute, Inc
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
 ******************************************************************************/

/***************************************************************************//**
 * @addtogroup ecm3532i2s-m3 Inter-IC Sound (I2S)
 * @ingroup ecm3532csp-m3
 * @{
 ******************************************************************************/

#include <stdint.h>
#include "eta_status.h"
#include "eta_chip.h"
#include "eta_csp_i2s.h"
#include "eta_csp_mailbox_cmd.h"
#include "memio.h"

/***************************************************************************//**
 *
 *  EtaCspI2sConfig - Initialize the I2S module. Meant to be initialized either
 *                    from mailbox command from DSP or locally.
 *
 *  @param cfg encoded configuration value.
 *
 ******************************************************************************/
tI2sConfigRetVal
EtaCspI2sConfig(tI2sConfig i2sConfig)
{

  uint32_t tempdata0 ;

  #ifdef ETA_CSP_I2S_DEBUG_PRINT
  ETA_DIAGS_PRINTF("M3:  packed_config:%x\r\n", i2sConfig.packed_config);
  ETA_DIAGS_PRINTF("M3:  enable_rx       :%x\r\n", i2sConfig.fields.enable_rx        );
  ETA_DIAGS_PRINTF("M3:  enable_tx       :%x\r\n", i2sConfig.fields.enable_tx        );
  ETA_DIAGS_PRINTF("M3:  swap_chan_tx    :%x\r\n", i2sConfig.fields.swap_chan_tx     );
  ETA_DIAGS_PRINTF("M3:  swap_chan_rx    :%x\r\n", i2sConfig.fields.swap_chan_rx     );
  ETA_DIAGS_PRINTF("M3:  mode_64bit      :%x\r\n", i2sConfig.fields.mode_64bit       );
  ETA_DIAGS_PRINTF("M3:  edge_tx_trig    :%x\r\n", i2sConfig.fields.edge_tx_trig     );
  ETA_DIAGS_PRINTF("M3:  edge_rx_trig    :%x\r\n", i2sConfig.fields.edge_rx_trig     );
  ETA_DIAGS_PRINTF("M3:  clock_src       :%x\r\n", i2sConfig.fields.clock_src        );
  ETA_DIAGS_PRINTF("M3:  sample_freq     :%x\r\n", i2sConfig.fields.sample_freq      );
  ETA_DIAGS_PRINTF("M3:  chan_tx_config  :%x\r\n", i2sConfig.fields.chan_tx_config   );
  ETA_DIAGS_PRINTF("M3:  chan_rx_config  :%x\r\n", i2sConfig.fields.chan_rx_config   );
  ETA_DIAGS_PRINTF("M3:  chan_tx_upscale :%x\r\n", i2sConfig.fields.chan_tx_upscale  );
  ETA_DIAGS_PRINTF("M3:  chan_rx_upscale :%x\r\n", i2sConfig.fields.chan_rx_upscale  );
  ETA_DIAGS_PRINTF("M3:  int_mode        :%x\r\n", i2sConfig.fields.int_mode         );
  #endif // ETA_CSP_I2S_DEBUG_PRINT

  ///////////////////////////////////////////////
  // Setup Clock Source
  switch (i2sConfig.fields.clock_src)
  {
    case (eI2sClkSrc2MhzXtal):
          // Power up the 16MHz oscillator
          REG_M1(RTC_OSC_CSR2,VHFO_PD_N,1);
          // Fence
          REG_GPIO_DEBUG0_READ_ONLY.V = REG_SOCCTRL_PMUXB.V;
          REG_M1(SOCCTRL_CLK_CFG,ADC_CLK_SEL,BFV_SOCCTRL_CLK_CFG_ADC_CLK_SEL_2MHZ_XTAL);
          break;
    case (eI2sClkSrc2MhzHfo):
          REG_M1(SOCCTRL_CLK_CFG,ADC_CLK_SEL,BFV_SOCCTRL_CLK_CFG_ADC_CLK_SEL_2MHZ_HFO);
          break;
    case (eI2sClkSrc1MhzHfo):
          REG_M1(SOCCTRL_CLK_CFG,ADC_CLK_SEL,BFV_SOCCTRL_CLK_CFG_ADC_CLK_SEL_1MHZ_HFO);
          break;
    case (eI2sClkSrcFromPad):
          // Enable ACLK as input
          tempdata0 = REG_SOCCTRL_PMUXB.V;
          tempdata0 &= ~(BM_SOCCTRL_PMUXB_GPIO12 );
          tempdata0 |= ( BFV_SOCCTRL_PMUXB_GPIO12_I2S_ACLK <<BP_SOCCTRL_PMUXB_GPIO12) ;
          REG_SOCCTRL_PMUXB.V = tempdata0;
          // Fence
          REG_GPIO_DEBUG0_READ_ONLY.V = REG_SOCCTRL_PMUXB.V;
          // Select ACLK as clock source
          REG_M1(SOCCTRL_CLK_CFG,ADC_CLK_SEL,BFV_SOCCTRL_CLK_CFG_ADC_CLK_SEL_2MHZ_PAD);
          break;
    default:
          return (eI2sConfigInvalidClkSource);
          break;
  }
  // Setup Clock Source
  ///////////////////////////////////////////////

  ///////////////////////////////////////////////
  // Setup Pad Mux
  if (i2sConfig.fields.enable_tx && i2sConfig.fields.enable_rx) // Enable BCLK, LRCLK, DATAOUT, DATAIN
  {
    #ifdef ETA_CSP_I2S_DEBUG_PRINT
    ETA_DIAGS_PRINTF("M3:  Configuring GPIO13,14,15,16 (TX and RX)\r\n");
    #endif // ETA_CSP_I2S_DEBUG_PRINT

    tempdata0 = REG_SOCCTRL_PMUXC.V;
    tempdata0 &= ~ BM_SOCCTRL_PMUXC_GPIO16 ;
    tempdata0 |=  BFV_SOCCTRL_PMUXC_GPIO16_I2S_DATA_OUT << BP_SOCCTRL_PMUXC_GPIO16 ;
    REG_SOCCTRL_PMUXC.V = tempdata0;

    tempdata0 = REG_SOCCTRL_PMUXB.V;

    tempdata0 &= ~(BM_SOCCTRL_PMUXB_GPIO15 |
                   BM_SOCCTRL_PMUXB_GPIO14 |
                   BM_SOCCTRL_PMUXB_GPIO13 );
    tempdata0 |= ( BFV_SOCCTRL_PMUXB_GPIO15_I2S_DATA_IN <<BP_SOCCTRL_PMUXB_GPIO15) |
                 ( BFV_SOCCTRL_PMUXB_GPIO14_I2S_LRCLK <<BP_SOCCTRL_PMUXB_GPIO14) |
                 ( BFV_SOCCTRL_PMUXB_GPIO13_I2S_BCLK <<BP_SOCCTRL_PMUXB_GPIO13) ;
    REG_SOCCTRL_PMUXB.V = tempdata0;
  }
  else if (i2sConfig.fields.enable_rx) // Enable BCLK, LRCLK, DATAIN
  {
    #ifdef ETA_CSP_I2S_DEBUG_PRINT
    ETA_DIAGS_PRINTF("M3:  Configuring GPIO13,14,15 (RX Only)\r\n");
    #endif // ETA_CSP_I2S_DEBUG_PRINT

    tempdata0 = REG_SOCCTRL_PMUXB.V;

    tempdata0 &= ~(BM_SOCCTRL_PMUXB_GPIO15 |
                   BM_SOCCTRL_PMUXB_GPIO14 |
                   BM_SOCCTRL_PMUXB_GPIO13 );
    tempdata0 |= ( BFV_SOCCTRL_PMUXB_GPIO15_I2S_DATA_IN <<BP_SOCCTRL_PMUXB_GPIO15) |
                 ( BFV_SOCCTRL_PMUXB_GPIO14_I2S_LRCLK <<BP_SOCCTRL_PMUXB_GPIO14) |
                 ( BFV_SOCCTRL_PMUXB_GPIO13_I2S_BCLK <<BP_SOCCTRL_PMUXB_GPIO13) ;
    REG_SOCCTRL_PMUXB.V = tempdata0;
  }
  else if (i2sConfig.fields.enable_tx) // Enable BCLK, LRCLK, DATAOUT
  {
    #ifdef ETA_CSP_I2S_DEBUG_PRINT
    ETA_DIAGS_PRINTF("M3:  Configuring GPIO13,14,16 (TX Only)\r\n");
    #endif // ETA_CSP_I2S_DEBUG_PRINT

    tempdata0 = REG_SOCCTRL_PMUXC.V;
    tempdata0 &= ~ BM_SOCCTRL_PMUXC_GPIO16 ;
    tempdata0 |=  BFV_SOCCTRL_PMUXC_GPIO16_I2S_DATA_OUT << BP_SOCCTRL_PMUXC_GPIO16 ;
    REG_SOCCTRL_PMUXC.V = tempdata0;

    tempdata0 = REG_SOCCTRL_PMUXB.V;

    tempdata0 &= ~(BM_SOCCTRL_PMUXB_GPIO14 |
                   BM_SOCCTRL_PMUXB_GPIO13 );
    tempdata0 |= ( BFV_SOCCTRL_PMUXB_GPIO14_I2S_LRCLK <<BP_SOCCTRL_PMUXB_GPIO14) |
                 ( BFV_SOCCTRL_PMUXB_GPIO13_I2S_BCLK <<BP_SOCCTRL_PMUXB_GPIO13) ;
    REG_SOCCTRL_PMUXB.V = tempdata0;
  }

  // Fence
  REG_GPIO_DEBUG0_READ_ONLY.V = REG_SOCCTRL_PMUXB.V;

  // Setup Pad Mux
  ///////////////////////////////////////////////

  ///////////////////////////////////////////////
  // Configure Actual I2S

//  // Trigger gratuitous Soft Reset
//  REG_I2S_CSR2.V = BM_I2S_CSR2_SFTRST ;
//  while (REG_I2S_CSR2.V == BM_I2S_CSR2_SFTRST);

  REG_I2S_CSR2.V = (i2sConfig.fields.mode_64bit     << BP_I2S_CSR2_BIT64_MODE)  |
                   (i2sConfig.fields.sample_freq    << BP_I2S_CSR2_SAMPLE_FREQ) ;

  REG_I2S_CSR.V  = 1  << BP_I2S_CSR_I2S_EN ;

  // Configure Actual I2S
  ///////////////////////////////////////////////

  return eI2sConfigGood;
}

//
// Initialize the I2S.
//
/***************************************************************************//**
 *
 *  EtaCspI2sConfigUnpack - Initialize the I2S module. Meant to be initialized
 *                          from mailbox command from DSP
 *
 *  @param cfg packed version of tI2sConfig
 *
 ******************************************************************************/
tI2sConfigRetVal EtaCspI2sConfigUnpack(uint32_t packed_config)
{
  tI2sConfig i2sConfig;
  i2sConfig.packed_config = packed_config;
  return (EtaCspI2sConfig(i2sConfig));
}

/***************************************************************************//**
 *
 * EtaCspI2sFence - Set up a fence for writes to the I2S registers.
 *
 *
 ******************************************************************************/
void
EtaCspI2sFence(void)
{
    EtaCspI2sFenceFast();
}

