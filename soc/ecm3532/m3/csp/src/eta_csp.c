/***************************************************************************//**
 *
 * @file eta_csp.c
 *
 * @brief This file contains eta_csp module implementation.
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
 * This is part of revision 1.0.0a6 of the Tensai Software Development Kit.
 *
 ******************************************************************************/

/***************************************************************************//**
 * @ingroup ecm3532csp-m3
 * @{
 ******************************************************************************/

#include "eta_chip.h"
#include "eta_csp.h"
#include "eta_csp_sys_clock.h"
#include "eta_csp_flash.h"

/***************************************************************************//**
 *
 *  EtaCspInit - Initialize the CSP.
 *
 *  @return The status.
 *
 ******************************************************************************/
tEtaStatus
EtaCspInit(void)
{
    tEtaStatus iStatus = eEtaSuccess;

    //
    // Set the system clock.
    //
    EtaCspSysClockInit();

    //
    // Set the M3 and DSP oscillators extra delay to 0.
    //
    REG_M1(RTC_OSC_CSR, M3_OSC_DLY_SEL, 0x0);
    REG_M1(RTC_OSC_CSR, DSP_OSC_DLY_SEL, 0x0);

    //
    // Enable clock gating in all of the blocks.
    //
    REGN_M1(0, UART_CONFIG2, CLK_ON, 0);
    REGN_M1(1, UART_CONFIG2, CLK_ON, 0);
    REGN_M1(0, SPI_CONFIG2, CLK_ON, 0);
    REGN_M1(1, SPI_CONFIG2, CLK_ON, 0);
    REGN_M1(0, I2C_CONFIG2, CLK_ON, 0);
    REGN_M1(1, I2C_CONFIG2, CLK_ON, 0);
    REG_M1(ADC_CNTRL, MODE, 0);
    REG_M1(GPIO8_CFG_STATUS, CLK_EN_OVR, 0);
    REG_M1(PWM_CTRL, CLK_EN_OVR, 0);
    REG_M1(SOCCTRL_CFG_STATUS, CLK_EN_OVR, 0);
    REG_M1(TIMER_CFG_STATUS, CLK_EN_OVR, 0);

    // new
    REG_M1(RTC_AO_CSR2, OVRD_CLK_GATE_HFO2PMIC, 0);

    // 
    // Turn off the front panel
#ifdef NOTDEF
    fixme this really must be turned off
     REG_M7(SOCCTRL_FPANEL, RUN_STATE, 0x0,
                            CONTINUOUS, 0x0,
                            CLK_DISABLE, 0x1,
                            DSP_OSC, 0x0,
                            M3_OSC, 0x0,
                            M3_OSC_DIV, 0x4,
                            DSP_OSC_DIV, 0x4);
#endif

    //
    // Add a fence to make sure we are done.
    //
    REG_RTC_DEBUG.V = REG_TIMER_CFG_STATUS.V;

    //
    // Turn on the clock gates to the srams in the RTC regs.
    //
    REG_M1(RTC_DEBUG, FORCE_MEM_CLKS, 0);

    //
    // RTC IO bus fence.
    //
    REG_RTC_DEBUG.V = REG_RTC_DEBUG.V;
    //
    // Initialize flash helper function pointers into bootrom.
    //
    iStatus = EtaCspFlashInit();

    //
    // Return the status.
    //
    return(iStatus);
}


