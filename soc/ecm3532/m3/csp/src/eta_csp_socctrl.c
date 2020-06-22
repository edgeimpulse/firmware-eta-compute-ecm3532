/***************************************************************************//**
 *
 * @file eta_csp_socctrl.c
 *
 * @brief This file contains eta_csp_socctrl module implementations.
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
 * @addtogroup ecm3532socctrl-m3 SoC Control (SOCCTRL)
 * @ingroup ecm3532csp-m3
 * @{
 ******************************************************************************/

#include "eta_chip.h"
#include "eta_csp_socctrl.h"
#include "eta_csp.h"
#include "memio.h"

/***************************************************************************//**
 *
 *  EtaCspSocCtrlResetStatusGet - Get the M3 reset status value.
 *
 *  @return Status bit settings in Reset Status OR'ed together.
 *
 ******************************************************************************/
uint32_t
EtaCspSocCtrlResetStatusGet(void)
{
    uint32_t ui32Work;
    uint32_t ui32Return = 0;

    ui32Work = REG_SOCCTRL_RESET_STATUS.V;

    if(ui32Work & BM_SOCCTRL_RESET_STATUS_PIN)
    {
        ui32Return |= eSocCtrlResetStatusPin;
    }
    if(ui32Work & BM_SOCCTRL_RESET_STATUS_WDT)
    {
        ui32Return |= eSocCtrlResetStatusWatchdog;
    }
    if(ui32Work & BM_SOCCTRL_RESET_STATUS_M3_DAP)
    {
        ui32Return |= eSocCtrlResetStatusDap;
    }
    if(ui32Work & BM_SOCCTRL_RESET_STATUS_M3_AIRCR_SYS)
    {
        ui32Return |= eSocCtrlResetStatusAirCr;
    }
    if(ui32Work & BM_SOCCTRL_RESET_STATUS_M3_AIRCR_VECT)
    {
        ui32Return |= eSocCtrlResetStatusVector;
    }
    if(ui32Work & BM_SOCCTRL_RESET_STATUS_POR)
    {
        ui32Return |= eSocCtrlResetStatusPor;
    }

    return(ui32Return);
}

/***************************************************************************//**
 *
 *  EtaCspSocCtrlResetStatusClear - Clear the M3 reset status value.
 *
 ******************************************************************************/
void
EtaCspSocCtrlResetStatusClear(void)
{
    REG_SOCCTRL_RESET_STATUS.V = 0;

    //
    // Ensure the writes are complete before returning.
    //
    REG_GPIO_DEBUG1_READ_ONLY.V = REG_RTC_AO_CSR.V;
}

/***************************************************************************//**
 *
 *  EtaCspSocCtrlM3FrequencyGet - Get the M3 frequency in Hz.
 *
 *  @return Current frequency for the M3 in Hz.
 *
 ******************************************************************************/
uint32_t
EtaCspSocCtrlM3FrequencyGet(void)
{
    return(REG_SOCCTRL_M3_FREQUENCY.BF.COUNT * 8);
}

/***************************************************************************//**
 *
 *  EtaCspSocCtrlDspFrequencyGet - Get the DSP frequency in Hz.
 *
 *  @return Current frequency for the DSP in Hz.
 *
 ******************************************************************************/
uint32_t
EtaCspSocCtrlDspFrequencyGet(void)
{
    return(REG_SOCCTRL_DSP_FREQUENCY.BF.COUNT * 8);
}

/***************************************************************************//**
 *
 *  EtaCspSocCtrlStallWfiEnable - Enable WFI to Stall the SOC.
 *
 ******************************************************************************/
void
EtaCspSocCtrlStallWfiEnable()
{
    REG_S1(SOCCTRL_CFG_STATUS,WFI_STALL); 
}

/***************************************************************************//**
 *
 *  EtaCspSocCtrlStallWfiDisable - Disable WFI to Stall the SOC.
 *
 ******************************************************************************/
void
EtaCspSocCtrlStallWfiDisable() 
{
    REG_C1(SOCCTRL_CFG_STATUS,WFI_STALL); 
}

/***************************************************************************//**
 *
 *  EtaCspSocCtrlStallDisable - Disable Stall on the SOC.
 *
 ******************************************************************************/
void
EtaCspSocCtrlStallDisable(void)
{
    REG_M1(SOCCTRL_CFG_STATUS, M3DSEN, 0);
}

/***************************************************************************//**
 *
 *  EtaCspSocCtrlStallEnable - Enable Stall on the SOC.
 *
 ******************************************************************************/
void
EtaCspSocCtrlStallEnable(void)
{
    REG_M1(SOCCTRL_CFG_STATUS, M3DSEN, 1);
}

/***************************************************************************//**
 *
 *  EtaCspSocCtrlStallAllowIrq - Prime the interrupt control bit for Stall wake up.
 *
 *  @param iStallBit - the SOC wake up interrupt control bits.
 *
 ******************************************************************************/
void
EtaCspSocCtrlStallAllowIrq(tStallIntBit iStallBit)
{
    REG_SOCCTRL_M3_DIAL_STALL_SET.V = iStallBit;
}

/***************************************************************************//**
 *
 *  EtaCspSocCtrlStall - Stall the SOC.
 *
 *  @param iStallBit - the SOC wake up interrupt control bits.
 *
 ******************************************************************************/
void
EtaCspSocCtrlStall(tStallIntBit iStallBit)
{
    //
    // Stall.
    //
    REG_SOCCTRL_M3_DIAL_STALL_SET.V = (iStallBit | BM_SOCCTRL_M3_DIAL_STALL_CSR_STALL);

    //
    // Do nothing until we wakeup from stall condition.
    //
    while(REG_R(SOCCTRL_M3_DIAL_STALL_CSR, STALL) == TRUE)
    {
    }
}

/***************************************************************************//**
 *
 *  EtaCspSocCtrlStallAnyIrq - Stall the SOC and use any interrupt to wake up.
 *
 *  @note Make sure peripheral has been set to fire before calling this
 * function so the device can wake up.
 *
 ******************************************************************************/
void
EtaCspSocCtrlStallAnyIrq (void)
{
    EtaCspSocCtrlStall(eStallAnyInt); 
}

/***************************************************************************//**
 *
 * EtaCspSocCtrlPadMuxSet - Set one Pad Mux Selector for one GPIO pin.
 *
 * @param iBit desired GPIO.
 * @param iPadMux desired mux setting.
 *
 ******************************************************************************/
void
EtaCspSocCtrlPadMuxSet(tGpioBit iBit, tSocCtrlPadMux iPadMux)
{
    uint32_t ui32RegAddr;
    uint32_t ui32RegMask;

    //
    // Extract Word Index into Pad Mux Select Registers and get address.
    //
    ui32RegAddr = (iBit & 0x00000018) >> 1;
    ui32RegAddr += (uint32_t)&REG_SOCCTRL_PMUXA.V;

    //
    // Setup RMW Mask.
    //
    ui32RegMask = (0x0000000F << ((iBit & 0x00000007) * 4));

    //
    // Make sure mux is saved.
    //
    iPadMux &= 0x00000007;

    //
    // Mask off bits in proper register then OR in iSelector.
    //
    MEMIO32(ui32RegAddr) &= ~ui32RegMask;
    MEMIO32(ui32RegAddr) |= (iPadMux << ((iBit & 0x00000007) * 4));
}

/***************************************************************************//**
 *
 *  EtaCspSocCtrlUstFreqGet - Return current Clock UST Select
 *
 *  @return Clock UST Select
 *
 ******************************************************************************/
tSocCtrlUSTSelect
EtaCspSocCtrlUstFreqGet(void)
{
    //
    // decode the current setting
    //
    switch(REG_SOCCTRL_CLK_CFG.BF.UST_CLK_SEL)
    {
        default:
               return eSocCtrlUSTSelError;
        case 0x2:
               return eSocCtrlUSTSel2MHz;
        case 0x1:
               return eSocCtrlUSTSel8MHz;
        case 0x0:
               return eSocCtrlUSTSel4MHz;
    }

}

/***************************************************************************//**
 *
 *  EtaCspSocCtrlUstFreqSet - Sets Clock UST Select
 *
 *  @param Clock UST Select
 *
 ******************************************************************************/
void
EtaCspSocCtrlUstFreqSet(tSocCtrlUSTSelect sSelect)
{
    //
    // decode the current setting
    //
    switch(sSelect)
    {

        case eSocCtrlUSTSel2MHz:
            REG_SOCCTRL_CLK_CFG.BF.UST_CLK_SEL = 2;
	    break;
        case eSocCtrlUSTSel4MHz:
            REG_SOCCTRL_CLK_CFG.BF.UST_CLK_SEL = 0;
	    break;
        default: // pick one
        case eSocCtrlUSTSel8MHz:
            REG_SOCCTRL_CLK_CFG.BF.UST_CLK_SEL = 1;
	    break;
    }

}

/***************************************************************************//**
 *
 *  EtaCspSocCtrlDspFreqGet - Return current Clock DSP Select
 *
 *  @return Clock DSP Select
 *
 ******************************************************************************/
tSocCtrlDspSelect
EtaCspSocCtrlDspFreqGet(void)
{
    //
    // decode the current setting
    //
    switch(REG_SOCCTRL_CLK_CFG.BF.DSP_CLK_SEL)
    {
        default:
               return eSocCtrlDspSelError;
        case 0x2:
               return eSocCtrlDspSel2MHz;
        case 0x1:
               return eSocCtrlDspSel8MHz;
        case 0x0:
               return eSocCtrlDspSel4MHz;
    }

}

/***************************************************************************//**
 *
 *  EtaCspSocCtrlDspFreqSet - Sets Clock DSP Select
 *
 *  @param Clock DSP Select
 *
 ******************************************************************************/
void
EtaCspSocCtrlDspFreqSet(tSocCtrlDspSelect sSelect)
{
    //
    // decode the current setting
    //
    switch(sSelect)
    {
        case eSocCtrlDspSel2MHz:
            REG_SOCCTRL_CLK_CFG.BF.DSP_CLK_SEL = 2;
	    break;
        case eSocCtrlDspSel4MHz:
            REG_SOCCTRL_CLK_CFG.BF.DSP_CLK_SEL = 0;
	    break;
        default: // pick one
        case eSocCtrlDspSel8MHz:
            REG_SOCCTRL_CLK_CFG.BF.DSP_CLK_SEL = 1;
	    break;
    }

}

/***************************************************************************//**
 *
 *  EtaCspSocCtrlAdcFreqGet - Return current Clock ADC Select
 *
 *  @return Clock ADC Select
 *
 ******************************************************************************/
tSocCtrlAdcSelect
EtaCspSocCtrlAdcFreqGet(void)
{
    //
    // decode the current setting
    //
    switch(REG_SOCCTRL_CLK_CFG.BF.ADC_CLK_SEL)
    {
        default:
               return eSocCtrlAdcSelError;
        case 0x3:
               return eSocCtrlAdcSelPad2MHz;
        case 0x2:
               return eSocCtrlAdcSelXtal2MHz;
        case 0x1:
               return eSocCtrlAdcSelHfo2MHz;
        case 0x0:
               return eSocCtrlAdcSelHfo1MHz;
    }

}


/***************************************************************************//**
 *
 *  EtaCspSocCtrlAdcFreqSet - Sets Clock ADC Select
 *
 *  @param Clock ADC Select
 *
 ******************************************************************************/
void
EtaCspSocCtrlAdcFreqSet(tSocCtrlAdcSelect sSelect)
{
    //
    // decode the current setting
    //
    switch(sSelect)
    {
        default: // pick one
        case eSocCtrlAdcSelPad2MHz:
            REG_SOCCTRL_CLK_CFG.BF.ADC_CLK_SEL = 3;
	    break;
        case eSocCtrlAdcSelXtal2MHz:
            REG_SOCCTRL_CLK_CFG.BF.ADC_CLK_SEL = 2;
	    break;
        case eSocCtrlAdcSelHfo2MHz:
            REG_SOCCTRL_CLK_CFG.BF.ADC_CLK_SEL = 1;
	    break;
        case eSocCtrlAdcSelHfo1MHz:
            REG_SOCCTRL_CLK_CFG.BF.ADC_CLK_SEL = 0;
	    break;
    }

}

/***************************************************************************//**
 *
 *  EtaCspSocCtrlI2CFreqGet - Return current Clock I2C Select
 *
 *  @return Clock I2C Select
 *
 ******************************************************************************/
tSocCtrlI2CSelect
EtaCspSocCtrlI2CFreqGet(void)
{
    //
    // decode the current setting
    //
    switch(REG_SOCCTRL_CLK_CFG.BF.I2C_CLK_SEL)
    {
        default:
               return eSocCtrlI2CSelError;
        case 0x0:
               return eSocCtrlI2CSel2MHz;
        case 0x1:
               return eSocCtrlI2CSel4MHz;
    }

}

/***************************************************************************//**
 *
 *  EtaCspSocCtrlI2CFreqSet - Sets Clock I2C Select
 *
 *  @param Clock I2C Select
 *
 ******************************************************************************/
void
EtaCspSocCtrlI2CFreqSet(tSocCtrlI2CSelect sSelect)
{
    //
    // decode the current setting
    //
    switch(sSelect)
    {
        case eSocCtrlI2CSel2MHz:
            REG_SOCCTRL_CLK_CFG.BF.I2C_CLK_SEL = 0;
	    break;
        default: // pick one
        case eSocCtrlI2CSel4MHz:
            REG_SOCCTRL_CLK_CFG.BF.I2C_CLK_SEL = 1;
	    break;
    }

}

/***************************************************************************//**
 *
 * EtaCspSocCtrlFence - Set up a fence for writes to the SOC Control regs.
 *
 * Add a fence on the last bus operation to SOC Ctrl registers.
 *
 * NOTE this one works for GPIO8 fencing as well.
 *
 ******************************************************************************/
void
EtaCspSocCtrlFence(void)
{
    EtaCspSocCtrlFenceFast();
}

