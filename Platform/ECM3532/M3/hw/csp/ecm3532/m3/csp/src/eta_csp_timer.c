/***************************************************************************//**
 *
 * @file eta_csp_timer.c
 *
 * @brief This file contains eta_csp_timer module implementations.
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
 * This is part of revision 1.0.0b1 of the Tensai Chip Support Package.
 *
 ******************************************************************************/

/***************************************************************************//**
 * @addtogroup ecm3532timer-m3 Timer
 * @ingroup ecm3532csp-m3
 * @{
 ******************************************************************************/

#include <stdint.h>
#include "eta_chip.h"
#include "eta_csp_timer.h"
#include "eta_csp_gpio.h"

/***************************************************************************//**
 *
 *  EtaCspTimerInitMs - Initialize the timer to count in milliseconds.
 *
 ******************************************************************************/
void
EtaCspTimerInitMs(void)
{
    //
    // Stop timer and reset to zero.
    //
    REG_W2(TIMER_DEBUG_CTL, MODE, FALSE, OPERATION, FALSE);

    //
    // Enable counting.
    //
    REG_W2(TIMER_DEBUG_CTL, MODE, TRUE, OPERATION, TRUE);
}

/***************************************************************************//**
 *
 *  EtaCspTimerSoftReset - Perform a soft reset on the Timer.
 *
 ******************************************************************************/
void
EtaCspTimerSoftReset(void)
{
    REG_S1(TIMER_CFG_STATUS, SFTRST);
}

/***************************************************************************//**
 *
 *  EtaCspTimerCountGetMs - Get the timer count in milliseconds.
 *
 *  @return the timer count in milliseconds.
 *
 ******************************************************************************/
uint64_t
EtaCspTimerCountGetMs(void)
{
    uint32_t ui32Lo1 = REG_TIMER_COUNTER_LO.V;
    uint32_t ui32Hi1 = REG_TIMER_COUNTER_HI.V;
    uint32_t ui32Lo2 = REG_TIMER_COUNTER_LO.V;
    uint32_t ui32Hi2 = REG_TIMER_COUNTER_HI.V;
    uint64_t ui64Count = (((uint64_t)ui32Hi1 << 32U) | (uint64_t)ui32Lo1);

    if(ui32Hi1 != ui32Hi2)
    {
        ui64Count = (((uint64_t)ui32Hi2 << 32U) | (uint64_t)ui32Lo2);
    }

    //
    // Return the count.
    //
    return(ETA_CSP_TIMER_MS(ui64Count));
}

/***************************************************************************//**
 *
 *  EtaCspTimerTicksGet - Get the timer count in milliseconds and fill in the
 *                        passed pointers.
 *
 ******************************************************************************/
void
EtaCspTimerTicksGet(uint32_t *ui32CounterHi, uint32_t *ui32CounterLo)
{
    uint32_t ui32FlagSave;

    do
    {
        //
        // First, read and remember the counter flag.
        //
        ui32FlagSave = REG_TIMER_CFG_STATUS.V;

        //
        // Grab the current values in the counters.
        //
        *ui32CounterLo = REG_TIMER_COUNTER_LO.V;
        *ui32CounterHi = REG_TIMER_COUNTER_HI.V;

        //
        // Now keep doing it until the FLAG bit BEFORE and AFTER are the same.
        //
    }
    while(!((ui32FlagSave ^ REG_TIMER_CFG_STATUS.V) &
            BM_TIMER_CFG_STATUS_FLAG));
}

/***************************************************************************//**
 *
 *  EtaCspTimerDelayMs - Delay for a number of milliseconds by polling a timer.
 *
 *  @param ui64DelayMs is the number of milliseconds to delay for.
 *
 ******************************************************************************/
void
EtaCspTimerDelayMs(uint64_t ui64DelayMs)
{
    uint64_t ui64Count = EtaCspTimerCountGetMs();
    uint64_t ui64Expire = ui64Count + ui64DelayMs;

    //
    // Poll the timer until the desired number of milliseconds has been reached.
    //
    do
    {
        ui64Count = EtaCspTimerCountGetMs();
    }
    while(ui64Count < ui64Expire);
}

/***************************************************************************//**
 *
 *  EtaCspTimerCmpSet - Set next count for comparator.
 *
 *  @param ui32Cycle offset from current timer value.
 *  @param iCmp comparator to set.
 *
 ******************************************************************************/
void
EtaCspTimerCmpSet(uint32_t ui32Cycle, tTimerComparatorNumber iCmp)
{
    switch(iCmp)
    {
        case eTimerComparatorA:
        {
            REG_TIMER_COMPAREA.V = ui32Cycle;
            break;
        }

        case eTimerComparatorB:
        {
            REG_TIMER_COMPAREB.V = ui32Cycle;
            break;
        }

        case eTimerComparatorC:
        {
            REG_TIMER_COMPAREC.V = ui32Cycle;
            break;
        }

        case eTimerComparatorD:
        {
            REG_TIMER_COMPARED.V = ui32Cycle;
            break;
        }
    }
}


/***************************************************************************//**
 *
 *  EtaCspTimerIntGet Get a timer interrupt bit value.
 *
 *  @param iBit Which interrupt bit to return.
 *
 *  @return Value of specified iBit.
 *
 * Example:   value = EtaCspTimerIntGet(eTimerIntCompareC);
 ******************************************************************************/
int32_t
EtaCspTimerIntGet(tTimerInterrupts iIBit)
{
    switch(iIBit)
    {
	default: // fall thru
        case eTimerIntCompareA:
            return  REG_TIMER_INT_STATUS.BF.CMPA;
        case eTimerIntCompareB:
            return  REG_TIMER_INT_STATUS.BF.CMPB;
        case eTimerIntCompareC:
            return  REG_TIMER_INT_STATUS.BF.CMPC;
        case eTimerIntCompareD:
            return  REG_TIMER_INT_STATUS.BF.CMPD;
        case eTimerIntCaptured:
            return  REG_TIMER_INT_STATUS.BF.CAPTURED;
        case eTimerIntLowOflow:
            return  REG_TIMER_INT_STATUS.BF.LO_OVERFLOW;
        case eTimerIntHiOflow:
            return  REG_TIMER_INT_STATUS.BF.HI_OVERFLOW;
    }
}

/***************************************************************************//**
 *
 *  EtaCspTimerIntClear Clear a timer interrupt bit value.
 *
 *  @param iBit Which interrupt bit to Clear.
 *
 * Example:   EtaCspTimerIntClear(eTimerIntCompareC);
 ******************************************************************************/
void
EtaCspTimerIntClear(tTimerInterrupts iIBit)
{
    switch(iIBit)
    {
	default: // fall thru
        case eTimerIntCompareA:
            REG_TIMER_INT_STATUS_CLR.BF.CMPA  = 1;
	    break;
        case eTimerIntCompareB:
            REG_TIMER_INT_STATUS_CLR.BF.CMPB  = 1;
	    break;
        case eTimerIntCompareC:
            REG_TIMER_INT_STATUS_CLR.BF.CMPC  = 1;
	    break;
        case eTimerIntCompareD:
            REG_TIMER_INT_STATUS_CLR.BF.CMPD  = 1;
	    break;
        case eTimerIntCaptured:
            REG_TIMER_INT_STATUS_CLR.BF.CAPTURED  = 1;
	    break;
        case eTimerIntLowOflow:
            REG_TIMER_INT_STATUS_CLR.BF.LO_OVERFLOW  = 1;
	    break;
        case eTimerIntHiOflow:
            REG_TIMER_INT_STATUS_CLR.BF.HI_OVERFLOW  = 1;
	    break;
    }
}

/***************************************************************************//**
 *
 *  EtaCspTimerIntAllClear Clear all timer interrupt bit value.
 *
 ******************************************************************************/
void
EtaCspTimerIntAllClear(void)
{
            REG_TIMER_INT_STATUS_CLR.V = 0x0000007F;
}

/***************************************************************************//**
 *
 *  EtaCspTimerIntAllGet Get all of the timer interrupt bits.
 *
 ******************************************************************************/
uint32_t
EtaCspTimerIntAllGet(void)
{
    return REG_TIMER_INT_STATUS.V;
}


/***************************************************************************//**
 *
 *  EtaCspTimerIntSet Set a timer interrupt bit value.
 *
 *  @param iBit Which interrupt bit to set.
 *
 * Example:   EtaCspTimerIntSet(eTimerIntCompareC);
 ******************************************************************************/
void
EtaCspTimerIntSet(tTimerInterrupts iIBit)
{
    switch(iIBit)
    {
	default: // fall thru
        case eTimerIntCompareA:
            REG_TIMER_INT_STATUS_SET.BF.CMPA  = 1;
	    break;
        case eTimerIntCompareB:
            REG_TIMER_INT_STATUS_SET.BF.CMPB  = 1;
	    break;
        case eTimerIntCompareC:
            REG_TIMER_INT_STATUS_SET.BF.CMPC  = 1;
	    break;
        case eTimerIntCompareD:
            REG_TIMER_INT_STATUS_SET.BF.CMPD  = 1;
	    break;
        case eTimerIntCaptured:
            REG_TIMER_INT_STATUS_SET.BF.CAPTURED  = 1;
	    break;
        case eTimerIntLowOflow:
            REG_TIMER_INT_STATUS_SET.BF.LO_OVERFLOW  = 1;
	    break;
        case eTimerIntHiOflow:
            REG_TIMER_INT_STATUS_SET.BF.HI_OVERFLOW  = 1;
	    break;
    }
}

/***************************************************************************//**
 *
 * EtaCspTimerIntEnaleAllGet Get all of the timer interrupt enable bits.
 *
 ******************************************************************************/
uint32_t 
EtaCspTimerIntEnableAllGet(tTimerIrqNumber iNumber)
{
    //
    // Pick one or the other interrupt enable registers and return it.
    //
    return (iNumber) ? REG_TIMER_1_INT_ENABLE.V : REG_TIMER_0_INT_ENABLE.V;
}

/***************************************************************************//**
 *
 * Get timer interrupt enable bit.
 *
 ******************************************************************************/
bool 
EtaCspTimerIntEnableGet(tTimerIrqNumber iNumber, tTimerInterrupts iIBit)
{
  uint32_t ui32Work;
  
  //
  // Pick one or the other interrupt enable registers.
  //
  ui32Work = (iNumber) ? REG_TIMER_1_INT_ENABLE.V : REG_TIMER_0_INT_ENABLE.V;

  return ( (ui32Work & (0x00000001 << iIBit)) != 0);
}


/***************************************************************************//**
 *
 *  EtaCspTimerIntEnableSet Set a timer interrupt enable bit value.
 *
 *  @param iNumber Which interrupt request on which to set the enable bit.
 *  @param iBit Which interrupt bit to set.
 *
 * Example:   EtaCspTimerIntEnableSet(eTimerIntCompareC);
 ******************************************************************************/
void
EtaCspTimerIntEnableSet(tTimerIrqNumber iNumber, tTimerInterrupts iIBit)
{
    if(iNumber)
    {
        switch(iIBit)
        {
	    default: // fall thru
            case eTimerIntCompareA:
                REG_TIMER_1_INT_ENABLE_SET.BF.CMPA = 1;
	        break;
            case eTimerIntCompareB:
                REG_TIMER_1_INT_ENABLE_SET.BF.CMPB = 1;
	        break;
            case eTimerIntCompareC:
                REG_TIMER_1_INT_ENABLE_SET.BF.CMPC = 1;
	        break;
            case eTimerIntCompareD:
                REG_TIMER_1_INT_ENABLE_SET.BF.CMPD = 1;
	        break;
            case eTimerIntCaptured:
                REG_TIMER_1_INT_ENABLE_SET.BF.CAPTURED = 1;
	        break;
            case eTimerIntLowOflow:
                REG_TIMER_1_INT_ENABLE_SET.BF.LO_OVERFLOW = 1;
	        break;
            case eTimerIntHiOflow:
                REG_TIMER_1_INT_ENABLE_SET.BF.HI_OVERFLOW = 1;
	        break;
        }
    }
    else
    {
        switch(iIBit)
        {
	    default: // fall thru
            case eTimerIntCompareA:
                REG_TIMER_0_INT_ENABLE_SET.BF.CMPA = 1;
	        break;
            case eTimerIntCompareB:
                REG_TIMER_0_INT_ENABLE_SET.BF.CMPB = 1;
	        break;
            case eTimerIntCompareC:
                REG_TIMER_0_INT_ENABLE_SET.BF.CMPC = 1;
	        break;
            case eTimerIntCompareD:
                REG_TIMER_0_INT_ENABLE_SET.BF.CMPD = 1;
	        break;
            case eTimerIntCaptured:
                REG_TIMER_0_INT_ENABLE_SET.BF.CAPTURED = 1;
	        break;
            case eTimerIntLowOflow:
                REG_TIMER_0_INT_ENABLE_SET.BF.LO_OVERFLOW = 1;
	        break;
            case eTimerIntHiOflow:
                REG_TIMER_0_INT_ENABLE_SET.BF.HI_OVERFLOW = 1;
	        break;
	        //assert(0); with enumerated values you theoretically can't get here.
	        break;
        }
    }
}

/***************************************************************************//**
 *
 *  EtaCspTimerIntEnableClear Clear a timer interrupt enable bit value.
 *
 *  @param iNumber Which interrupt request on which to clear the enable bit.
 *  @param iBit Which interrupt bit to set.
 *
 * Example:   EtaCspTimerIntEnableClear(eTimerIntCompareC);
 ******************************************************************************/
void
EtaCspTimerIntEnableClear(tTimerIrqNumber iNumber, tTimerInterrupts iIBit)
{
    if(iNumber)
    {
        switch(iIBit)
        {
	    default: // fall thru
            case eTimerIntCompareA:
                REG_TIMER_1_INT_ENABLE_CLR.BF.CMPA = 1;
	        break;
            case eTimerIntCompareB:
                REG_TIMER_1_INT_ENABLE_CLR.BF.CMPB = 1;
	        break;
            case eTimerIntCompareC:
                REG_TIMER_1_INT_ENABLE_CLR.BF.CMPC = 1;
	        break;
            case eTimerIntCompareD:
                REG_TIMER_1_INT_ENABLE_CLR.BF.CMPD = 1;
	        break;
            case eTimerIntCaptured:
                REG_TIMER_1_INT_ENABLE_CLR.BF.CAPTURED = 1;
	        break;
            case eTimerIntLowOflow:
                REG_TIMER_1_INT_ENABLE_CLR.BF.LO_OVERFLOW = 1;
	        break;
            case eTimerIntHiOflow:
                REG_TIMER_1_INT_ENABLE_CLR.BF.HI_OVERFLOW = 1;
	        break;
        }
    }
    else
    {
        switch(iIBit)
        {
	    default: // fall thru
            case eTimerIntCompareA:
                REG_TIMER_0_INT_ENABLE_CLR.BF.CMPA = 1;
	        break;
            case eTimerIntCompareB:
                REG_TIMER_0_INT_ENABLE_CLR.BF.CMPB = 1;
	        break;
            case eTimerIntCompareC:
                REG_TIMER_0_INT_ENABLE_CLR.BF.CMPC = 1;
	        break;
            case eTimerIntCompareD:
                REG_TIMER_0_INT_ENABLE_CLR.BF.CMPD = 1;
	        break;
            case eTimerIntCaptured:
                REG_TIMER_0_INT_ENABLE_CLR.BF.CAPTURED = 1;
	        break;
            case eTimerIntLowOflow:
                REG_TIMER_0_INT_ENABLE_CLR.BF.LO_OVERFLOW = 1;
	        break;
            case eTimerIntHiOflow:
                REG_TIMER_0_INT_ENABLE_CLR.BF.HI_OVERFLOW = 1;
	        break;
        }
    }
}

/***************************************************************************//**
 *
 *  EtaCspTimerIntEnableAllClear Clear all timer interrupt bit value.
 *
 *  @param iNumber Which interrupt request on which to clear the enable bit.
 *
 ******************************************************************************/
void
EtaCspTimerIntEnableAllClear(tTimerIrqNumber iNumber)
{
    if(iNumber)
    {
         REG_TIMER_1_INT_ENABLE.V = 0x00000000;
    }
    else
    {
         REG_TIMER_0_INT_ENABLE.V = 0x00000000;
    }
}

/***************************************************************************//**
 *
 *  EtaCspTimerCaptureEnable - Enable Capture on edge of GPIO[iBit]
 *
 *  @param iPolarity Rising or Falling edge of GPIO pin to capture time.
 *  @param iBit Which GPIO bit will cause the capture.
 *
 * NOTE: the GPIO selector mux will be programmed as well.
 *
 * Example:   EtaCspTimerCaptureEnable(eGpioCapSelRising,eGpioBit8);
 ******************************************************************************/
void
EtaCspTimerCaptureEnable(tGpioCapSelPolarity iPolarity, tGpioBit iGpioBit)
{
    //
    // Select rising or falling edge over in GPIO8
    //
    REG_GPIO8_CAPTURE_SEL.BF.POLARITY = (iPolarity == eGpioCapSelRising);

    //
    // Enable the capture information to flow from GPIO8 to the timer.
    //
    REG_GPIO8_CAPTURE_SEL.BF.ENABLE = 1;

    //
    // Select the GPIO bit to monitor for capture purposes.
    //
    REG_GPIO8_CAPTURE_SEL.BF.BIT = (uint32_t)iGpioBit;

    //
    // Enable the capture operation inside the timer module.
    //
    REG_TIMER_CFG_STATUS.BF.ENABLE_CAPTURE = 1;


}

/***************************************************************************//**
 *
 *  EtaCspTimerCaptureDisable - Disable Capturing time on the edge of a GPIO pin.
 *
 *
 * NOTE: the GPIO selector mux will be disabled as well.
 *
 * Example:   EtaCspTimerCaptureDisable();
 ******************************************************************************/
void
EtaCspTimerCaptureDisable(void)
{

    //
    // Disable the capture information to flow from GPIO8 to the timer.
    //
    REG_GPIO8_CAPTURE_SEL.BF.ENABLE = 0;

    //
    // Disable the capture operation inside the timer module.
    //
    REG_TIMER_CFG_STATUS.BF.ENABLE_CAPTURE = 0;

}

/***************************************************************************//**
 *
 *  EtaCspTimerCaptureStateGet - Disable Capturing time on the edge of a GPIO pin.
 *  @return - boolean, true if enabled.
 *
 ******************************************************************************/
bool
EtaCspTimerCaptureStateGet(void)
{

    //
    // Send it back to the caller.
    //
    return !!REG_GPIO8_CAPTURE_SEL.BF.ENABLE;

}

/***************************************************************************//**
 *
 *  EtaCspTimerCaptureGet - Get the time captured into the timer capture reg.
 *
 *  Example:   uint64_t ui64Value = EtaCspTimerCaptureGet();
 ******************************************************************************/
uint64_t
EtaCspTimerCaptureGet(void)
{
    uint32_t ui32Lo = REG_TIMER_CAPTURE_LO.V;
    uint32_t ui32Hi = REG_TIMER_CAPTURE_HI.V;
    uint64_t ui64Count = (((uint64_t)ui32Hi << 32U) | (uint64_t)ui32Lo);

    //
    // Return the 64-bit capture register value.
    //
    return ui64Count;
}

/***************************************************************************//**
 *
 *  EtaCspTimerCaptureLowGet - Get the time captured into the timer capture reg.
 *
 *  Example:   uint32_t ui32Value = EtaCspTimerCaptureLowGet();
 ******************************************************************************/
//
// Read lower 32-bits of capture register value.
//
uint32_t
EtaCspTimerCaptureLowGet(void)
{
    //
    // Return the 64-bit capture register value.
    //
    return REG_TIMER_CAPTURE_LO.V;
}

/***************************************************************************//**
 *
 *  EtaCspTimerFlagGet - Get the state of the timer toggle flag.
 *
 *  Example:   bool bValue = EtaCspTimerFlagGet();
 *
 *  NOTE: this flag toggles with each timer tick.
 ******************************************************************************/
bool
EtaCspTimerFlagGet(void)
{
    //
    // Return the state of the timer toggle flag.
    //
    return REG_TIMER_CFG_STATUS.BF.FLAG;
}

/***************************************************************************//**
 *
 * EtaCspTimerFence - Set up a fence for writes to the TIMER registers.
 *
 *
 ******************************************************************************/
void
EtaCspTimerFence(void)
{
    EtaCspTimerFenceFast();
}

