/*******************************************************************************
 *
 * @file startup_ecm3531.s
 *
 * @brief System startup functions
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
#include "config.h"
  .syntax unified
  .cpu cortex-m3
  .fpu softvfp
  .thumb

.global g_pfnVectors
.global Default_Handler

/* start address for the initialization values of the .data section.
defined in linker script */
.word _sidata
/* start address for the .data section. defined in linker script */
.word _sdata
/* end address for the .data section. defined in linker script */
.word _edata
/* start address for the .bss section. defined in linker script */
.word _sbss
/* end address for the .bss section. defined in linker script */
.word _ebss

.word rodata_end

/******************************************************************************
*
* The minimal vector table for a Cortex M3.  Note that the proper constructs
* must be placed on this to ensure that it ends up at physical address
* 0x0000.0000.
*
******************************************************************************/
  .section .isr_vector,"a",%progbits
  .type g_pfnVectors, %object
  .size g_pfnVectors, .-g_pfnVectors

g_pfnVectors:
  .word _estack
  .word Reset_Handler
  .word NMI_Handler
  .word HardFault_Handler
  .word MemManage_Handler
  .word BusFault_Handler
  .word UsageFault_Handler
  .word 0
  .word 0
  .word 0
  .word 0
  .word vPortSVCHandler
  .word DebugMon_Handler
  .word 0
  .word xPortPendSVHandler
  .word SysTick_Handler

  .word GPIO0_ISR
  .word GPIO1_ISR
  .word TIMER0_ISR
  .word TIMER1_ISR
  .word UART0_ISR
  .word UART1_ISR
  .word SPI0_ISR
  .word SPI1_ISR
  .word I2C0_ISR
  .word I2C1_ISR
  .word RTC0_ISR
  .word RTC1_ISR
  .word DSP_ISR
  .word ADC_ISR
  .word PWM_ISR
  .word WDT_ISR
  .word RTC_TMR_ISR
  .word SW0_ISR//BOD_ISR
  .word SW1_ISR
  .word SW2_ISR
  .word SW3_ISR
  .word SW4_ISR
  .word SW5_ISR
  .word SW6_ISR
/**
 * @brief  This is the code that gets called when the processor first
 *          starts execution following a reset event. Only the absolutely
 *          necessary set is performed, after which the application
 *          supplied main() routine is called.
 * @param  None
 * @retval : None
*/

  .global Reset_Handler
  .type Reset_Handler, %function
Reset_Handler:
  mov r1, #0
  cmp r1, #1
  beq Reset_Handler

/*
 * Copy code section.
 */
        ldr r0, =__text_load
        ldr r1, =__text_start
        ldr r2, =_etext
        cmp r0, r1
        beq copy_data
copy_code_loop:
        cmp r1, r2
    itt ne
        ldrne r3, [r0], #4
        strne r3, [r1], #4
        bne copy_code_loop

/*
 * Copy data section.
 */
copy_data:
        ldr r0, =__data_load
        ldr r1, =__data_start
        ldr r2, =_edata
        cmp r0, r1
        beq copy_end
copy_data_loop:
        cmp r1, r2
    itt ne
        ldrne r3, [r0], #4
        strne r3, [r1], #4
        bne copy_data_loop
copy_end:
/*
 * Copy Vector Table
 */
#if defined (CONFIG_FLASH_RELOCATE) ||  defined (CONFIG_FLASH_SHM_RELOCATE)
copy_vector_table:
        ldr r0, =__reset_start
        ldr r1, =__ram_vectors_start
        ldr r2, =__ram_vectors_end
        cmp r0, r1
        beq clear_bss
copy_vector_table_loop:
        cmp r1, r2
    itt ne
        ldrne r3, [r0], #4
        strne r3, [r1], #4
        bne copy_vector_table_loop
#endif
/*
 * Clear .bss
 */
clear_bss:
        ldr r0, =0
        ldr r1, =__bss_start__
        ldr r2, =__bss_end__
clear_bss_loop:
        cmp r1, r2
    it ne
        strne r0, [r1], #4
        bne clear_bss_loop

clear_heap:
        ldr r0, =0
        ldr r1, =__heap_start__
        ldr r2, =__heap_end__
clear_heap_loop:
        cmp r1, r2
    it ne
        strne r0, [r1], #4
        bne clear_heap_loop

#if (defined (CONFIG_FLASH_RELOCATE) ||  defined (CONFIG_FLASH_SHM_RELOCATE)) && defined (CONFIG_PBUF_IN_RAM)
copy_pbuf:
        ldr r0, =__pbuf_load
        ldr r1, =__pbuf_start
        ldr r2, =_epbuf
        cmp r0, r1
        beq call_sysinit
copy_pbuf_loop:
        cmp r1, r2
    itt ne
        ldrne r3, [r0], #4
        strne r3, [r1], #4
        bne copy_pbuf_loop
#endif

call_sysinit:
    cpsie i
    ldr r0, = g_pfnVectors
    ldr r1, = SystemInit
    blx  r1

#if defined (CONFIG_FLASH_RELOCATE) ||  defined (CONFIG_FLASH_SHM_RELOCATE)
call_rvr:
        ldr r0, =__text_load
        ldr r1, =__text_start
        beq branch_to_main
        ldr r1, =0
        ldr r0, = RelocateVectorsToRam
    blx  r0
#if 0
call_turnoff_flash:
    ldr r1, = EtaCspPwrFlashV25Set
        mov     r0, #1
    blx  r0
#endif
#endif

branch_to_main:
        mov     r0, #0          /*  no arguments  */
        mov     r1, #0          /*  no argv either */
    ldr pc, =main

/**
 * @brief  This is the code that gets called when the processor receives an
 *         unexpected interrupt.  This simply enters an infinite loop, preserving
 *         the system state for examination by a debugger.
 *
 * @param  None
 * @retval : None
*/
Default_Handler:
Infinite_Loop:
  b Infinite_Loop
  .size Default_Handler, .-Default_Handler
/*******************************************************************************
*
* Provide weak aliases for each Exception handler to the Default_Handler.
* As they are weak aliases, any function with the same name will override
* this definition.
*
*******************************************************************************/

  .weak NMI_Handler
  .thumb_set NMI_Handler,Default_Handler

  .weak HardFault_Handler
  .thumb_set HardFault_Handler,Default_Handler

  .weak MemManage_Handler
  .thumb_set MemManage_Handler,Default_Handler

  .weak BusFault_Handler
  .thumb_set BusFault_Handler,Default_Handler

  .weak UsageFault_Handler
  .thumb_set UsageFault_Handler,Default_Handler

  .weak SVC_Handler
  .thumb_set SVC_Handler,Default_Handler

  .weak DebugMon_Handler
  .thumb_set DebugMon_Handler,Default_Handler

//  .weak PendSV_Handler
//  .thumb_set PendSV_Handler,Default_Handler

  .weak SysTick_Handler
  .thumb_set SysTick_Handler,Default_Handler

  .weak GPIO0_ISR
  .thumb_set GPIO0_ISR,Default_Handler

  .weak GPIO1_ISR
  .thumb_set GPIO1_ISR,Default_Handler

  .weak TIMER0_ISR
  .thumb_set TIMER0_ISR,Default_Handler

  .weak TIMER1_ISR
  .thumb_set TIMER1_ISR,Default_Handler

  .weak UART0_ISR
  .thumb_set UART0_ISR,Default_Handler

  .weak UART1_ISR
  .thumb_set UART1_ISR,Default_Handler

  .weak SPI0_ISR
  .thumb_set SPI0_ISR,Default_Handler

  .weak SPI1_ISR
  .thumb_set SPI1_ISR,Default_Handler

  .weak I2C0_ISR
  .thumb_set I2C0_ISR,Default_Handler

  .weak I2C1_ISR
  .thumb_set I2C1_ISR,Default_Handler

  .weak RTC0_ISR
  .thumb_set RTC0_ISR,Default_Handler

  .weak RTC1_ISR
  .thumb_set RTC1_ISR,Default_Handler

  .weak DSP_ISR
  .thumb_set DSP_ISR,Default_Handler

  .weak ADC_ISR
  .thumb_set ADC_ISR,Default_Handler

  .weak PWM_ISR
  .thumb_set PWM_ISR,Default_Handler

  .weak WDT_ISR
  .thumb_set WDT_ISR,Default_Handler

  .weak RTC_TMR_ISR
  .thumb_set RTC_TMR_ISR,Default_Handler

  .weak SW0_ISR
  .thumb_set SW0_ISR,Default_Handler

  .weak SW1_ISR
  .thumb_set SW1_ISR,Default_Handler

  .weak SW2_ISR
  .thumb_set SW2_ISR,Default_Handler

  .weak SW3_ISR
  .thumb_set SW3_ISR,Default_Handler

  .weak SW4_ISR
  .thumb_set SW4_ISR,Default_Handler

  .weak SW5_ISR
  .thumb_set SW5_ISR,Default_Handler

  .weak SW6_ISR
  .thumb_set SW6_ISR,Default_Handler
