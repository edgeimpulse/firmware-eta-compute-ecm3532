/***************************************************************************//**
 *
 * @file eta_csp_isr.c
 *
 * @brief This file contains eta_csp_isr module implementations.
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
 * @addtogroup ecm3532isr-m3 Interrupt Service Routine (ISR)
 * @ingroup ecm3532csp-m3
 * @{
 ******************************************************************************/

#include "eta_status.h"
#include "eta_csp_isr.h"
#include "eta_chip.h"
#include "eta_csp.h"

//
// Macro to call the desired handler.
//
#define HandlerCall(ui32Irq) ppfnHandlerTable[ui32Irq](ui32Irq)

//
// The IRQ handler table.
//
static tpfIrqHandler ppfnHandlerTable[IRQ_NUM_TABLE_SZ] =
{
    EtaCspIsrDefaultHandler
};

/***************************************************************************//**
 *
 *  EtaCspIsrHandlerSet - Set the handler for an interrupt.
 *
 *  @param iIrqNum is the interrupt number.
 *  @param pfnHandler a pointer to an IRQ function to call.
 *
 ******************************************************************************/
void
EtaCspIsrHandlerSet(tIrqNum iIrqNum, tpfIrqHandler pfnHandler)
{
    ppfnHandlerTable[iIrqNum] = pfnHandler;
}

/***************************************************************************//**
 *
 *  EtaCspIsrDefaultHandlerSet - Set the default handler for an interrupt.
 *
 *  @param iIrqNum is the interrupt number.
 *
 ******************************************************************************/
void
EtaCspIsrDefaultHandlerSet(tIrqNum iIrqNum)
{
    EtaCspIsrHandlerSet(iIrqNum, EtaCspIsrDefaultHandler);
}

/***************************************************************************//**
 *
 *  EtaCspIsrDefaultHandler - The default ISR handler.
 *
 *  @param iIrqNum is the interrupt number.
 *
 ******************************************************************************/
void
EtaCspIsrDefaultHandler(tIrqNum iIrqNum)
{
//
// Keil.
//
#ifdef __CC_ARM
    __breakpoint(0);

//
// IAR.
//
#elif __ICCARM__
    asm volatile("BKPT #0");

//
// GCC.
//
#else
    __asm__ __volatile__ ("BKPT #0");
#endif
}

/***************************************************************************//**
 *
 *  GPIO0_ISR - Call the GPIO0 ISR Handler.
 *
 ******************************************************************************/
void
Gpio0_Handler(void)
{
    HandlerCall(eIrqNumGpio0);
}

/***************************************************************************//**
 *
 *  GPIO1_ISR - Call the GPIO1 ISR Handler.
 *
 ******************************************************************************/
void
Gpio1_Handler(void)
{
    HandlerCall(eIrqNumGpio1);
}

/***************************************************************************//**
 *
 *  TIMER0_ISR - Call the Timer0 ISR Handler.
 *
 ******************************************************************************/
void
Timer0_Handler(void)
{
    HandlerCall(eIrqNumTimer0);
}

/***************************************************************************//**
 *
 *  TIMER1_ISR - Call the Timer1 ISR Handler.
 *
 ******************************************************************************/
void
Timer1_Handler(void)
{
    HandlerCall(eIrqNumTimer1);
}

/***************************************************************************//**
 *
 *  UART0_ISR - Call the UART0 ISR Handler.
 *
 ******************************************************************************/
void
Uart0_Handler(void)
{
    HandlerCall(eIrqNumUart0);
}

/***************************************************************************//**
 *
 *  UART1_ISR - Call the UART1 ISR Handler.
 *
 ******************************************************************************/
void
Uart1_Handler(void)
{
    HandlerCall(eIrqNumUart1);
}

/***************************************************************************//**
 *
 *  SPI0_ISR - Call the I2C0 ISR Handler.
 *
 ******************************************************************************/
void
Spi0_Handler(void)
{
    HandlerCall(eIrqNumSpi0);
}

/***************************************************************************//**
 *
 *  SPI1_ISR - Call the SPI1 ISR Handler.
 *
 ******************************************************************************/
void
Spi1_Handler(void)
{
    HandlerCall(eIrqNumSpi1);
}

/***************************************************************************//**
 *
 *  I2C0_ISR - Call the I2C0 ISR Handler.
 *
 ******************************************************************************/
void
I2c0_Handler(void)
{
    HandlerCall(eIrqNumI2c0);
}

/***************************************************************************//**
 *
 *  I2C1_ISR - Call the I2C1 ISR Handler.
 *
 ******************************************************************************/
void
I2c1_Handler(void)
{
    HandlerCall(eIrqNumI2c1);
}

/***************************************************************************//**
 *
 *  RTC0_ISR - Call the RTC0 ISR Handler.
 *
 ******************************************************************************/
void
Rtc0_Handler(void)
{
    HandlerCall(eIrqNumRtc0);
}

/***************************************************************************//**
 *
 *  RTC1_ISR - Call the RTC1 ISR Handler.
 *
 ******************************************************************************/
void
Rtc1_Handler(void)
{
    HandlerCall(eIrqNumRtc1);
}

/***************************************************************************//**
 *
 *  DSP_ISR - Call the DSP ISR Handler.
 *
 ******************************************************************************/
void
Dsp_Handler(void)
{
    HandlerCall(eIrqNumDsp);
}

/***************************************************************************//**
 *
 *  ADC_ISR - Call the ADC ISR Handler.
 *
 ******************************************************************************/
void
Adc_Handler(void)
{
    HandlerCall(eIrqNumAdc);
}

/***************************************************************************//**
 *
 *  PWM_ISR - Call the PWM ISR Handler.
 *
 ******************************************************************************/
void
Pwm_Handler(void)
{
    HandlerCall(eIrqNumPwm);
}

/***************************************************************************//**
 *
 *  WDT_ISR - Call the Watchdog timer ISR Handler.
 *
 ******************************************************************************/
void
Wdt_Handler(void)
{
    HandlerCall(eIrqNumWdt);
}

/***************************************************************************//**
 *
 *  RTC_TMR_ISR - Call the RTC timer ISR Handler.
 *
 ******************************************************************************/
void
RtcTimer_Handler(void)
{
    HandlerCall(eIrqNumRtcTimer);
}

/***************************************************************************//**
 *
 *  BOD_ISR - Call the Brown Out Detector ISR Handler.
 *
 ******************************************************************************/
void
Bod_Handler(void)
{
    HandlerCall(eIrqNumBod);
}

/***************************************************************************//**
 *
 *  SW0_ISR - Call the SW0 ISR Handler.
 *
 ******************************************************************************/
void
Sw0_Handler(void)
{
    HandlerCall(eIrqNumSw0);
}

/***************************************************************************//**
 *
 *  SW1_ISR - Call the SW1 ISR Handler.
 *
 ******************************************************************************/
void
Sw1_Handler(void)
{
    HandlerCall(eIrqNumSw1);
}

/***************************************************************************//**
 *
 *  SW2_ISR - Call the SW2 ISR Handler.
 *
 ******************************************************************************/
void
Sw2_Handler(void)
{
    HandlerCall(eIrqNumSw2);
}

/***************************************************************************//**
 *
 *  SW3_ISR - Call the SW3 ISR Handler.
 *
 ******************************************************************************/
void
Sw3_Handler(void)
{
    HandlerCall(eIrqNumSw3);
}

/***************************************************************************//**
 *
 *  SW4_ISR - Call the SW4 ISR Handler.
 *
 ******************************************************************************/
void
Sw4_Handler(void)
{
    HandlerCall(eIrqNumSw4);
}

/***************************************************************************//**
 *
 *  SW5_ISR - Call the SW5 ISR Handler.
 *
 ******************************************************************************/
void
Sw5_Handler(void)
{
    HandlerCall(eIrqNumSw5);
}

