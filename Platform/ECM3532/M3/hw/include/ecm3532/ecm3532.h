/*******************************************************************************
 *
 * @file ecm3531.h
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

#ifndef ECM3531_H
#define ECM3531_H

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup Configuration_of_CMSIS
  * @{
  */



/* =========================================================================================================================== */
/* ================                                Interrupt Number Definition                                ================ */
/* =========================================================================================================================== */

typedef enum {
/* =======================================  ARM Cortex-M4 Specific Interrupt Numbers  ======================================== */
  Reset_IRQn                = -15,              /*!< -15  Reset Vector, invoked on Power up and warm reset                     */
  NonMaskableInt_IRQn       = -14,              /*!< -14  Non maskable Interrupt, cannot be stopped or preempted               */
  HardFault_IRQn            = -13,              /*!< -13  Hard Fault, all classes of Fault                                     */
  MemoryManagement_IRQn     = -12,              /*!< -12  Memory Management, MPU mismatch, including Access Violation
                                                      and No Match                                                             */
  BusFault_IRQn             = -11,              /*!< -11  Bus Fault, Pre-Fetch-, Memory Access Fault, other address/memory
                                                      related Fault                                                            */
  UsageFault_IRQn           = -10,              /*!< -10  Usage Fault, i.e. Undef Instruction, Illegal State Transition        */
  SVCall_IRQn               =  -5,              /*!< -5 System Service Call via SVC instruction                                */
  DebugMonitor_IRQn         =  -4,              /*!< -4 Debug Monitor                                                          */
  PendSV_IRQn               =  -2,              /*!< -2 Pendable request for system service                                    */
  SysTick_IRQn              =  -1,              /*!< -1 System Tick Timer                                                      */
/* ==========================================  ecm3531 Specific Interrupt Numbers  =========================================== */
  GPIO0_IRQn                =   0,              /*!< 0  GPIO0                                                                  */
  GPIO1_IRQn                =   1,              /*!< 1  GPIO1                                                                  */
  TIMER0_IRQn               =   2,              /*!< 2  TIMER0                                                                 */
  TIMER1_IRQn               =   3,              /*!< 3  TIMER1                                                                 */
  UART0_IRQn                =   4,              /*!< 4  UART0                                                                  */
  UART1_IRQn                =   5,              /*!< 5  UART1                                                                  */
  SPI0_IRQn                 =   6,              /*!< 6  SPI0                                                                   */
  SPI1_IRQn                 =   7,              /*!< 7  SPI1                                                                   */
  I2C0_IRQn                 =   8,              /*!< 8  I2C0                                                                   */
  I2C1_IRQn                 =   9,              /*!< 9  I2C1                                                                   */
  RTC0_IRQn                 =  10,              /*!< 10 RTC0                                                                   */
  RTC1_IRQn                 =  11,              /*!< 11 RTC1                                                                   */
  DSP_IRQn                  =  12,              /*!< 12 DSP                                                                    */
  ADC_IRQn                  =  13,              /*!< 13 ADC                                                                    */
  PWM_IRQn                  =  14,              /*!< 14 PWM                                                                    */
  WDT_IRQn                  =  15,              /*!< 15 WDT                                                                    */
  RTC_IRQn                  =  16,              /*!< 16 RTC                                                                    */
  SW0_IRQn                  =  17,              /*!< 17 SW0                                                                    */
  SW1_IRQn                  =  18,              /*!< 18 SW1                                                                    */
  SW2_IRQn                  =  19,              /*!< 19 SW2                                                                    */
} IRQn_Type;



/* =========================================================================================================================== */
/* ================                           Processor and Core Peripheral Section                           ================ */
/* =========================================================================================================================== */

/* ===========================  Configuration of the ARM Cortex-M4 Processor and Core Peripherals  =========================== */
#define __CM3_REV                 0x0200U       /*!< CM3 Core Revision r2p0 ??                                                 */
#define __NVIC_PRIO_BITS               3        /*!< Number of Bits used for Priority Levels                                   */
#define __MPU_PRESENT                  0        /*!< MPU present or not                                                        */
#define PEND_SV_PRIO    ((1 << __NVIC_PRIO_BITS) - 1)
#define OS_TICK_PRIO    (PEND_SV_PRIO - 1)


/** @} */ /* End of group Configuration_of_CMSIS */

#endif
