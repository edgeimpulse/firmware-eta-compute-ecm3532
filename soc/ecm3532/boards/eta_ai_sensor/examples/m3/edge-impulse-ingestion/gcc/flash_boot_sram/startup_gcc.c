/***************************************************************************//**
 *
 * @file startup_gcc.c
 *
 * @brief Definitions for interrupt handlers, the vector table, and the stack.
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
 * This is part of revision 1.0.0a3 of the Tensai Software Development Kit.
 *
 ******************************************************************************/

#include <stdint.h>
#include "eta_chip.h"
#include "memio.h"

#include "eta_csp_rtc.h"

/***************************************************************************//**
 *
 * Reserve space for the system stack.
 *
 ******************************************************************************/
__attribute__ ((section(".stack")))
static uint32_t g_pui32Stack[4096];

/***************************************************************************//**
 *
 * Forward declaration of the handlers.
 *
 ******************************************************************************/
extern void Copy2Sram(void)       __attribute((naked));
extern void Reset_Handler(void) __attribute__((weak, naked, alias("Default_Reset_Handler")));
extern void Nmi_Handler(void) __attribute((weak, alias("Default_Fault_Handler")));
extern void Fault_Handler(void) __attribute((weak, alias("Default_Fault_Handler")));
extern void MemManage_Handler(void) __attribute((weak, alias("Default_Fault_Handler")));
extern void Bus_Fault_Handler(void) __attribute((weak, alias("Default_Bus_Fault_Handler")));
extern void DebugMon_Handler(void) __attribute((weak, alias("Default_Handler")));
extern void Svc_Handler(void)     __attribute((weak, alias("Default_Handler")));
extern void PendSv_Handler(void)  __attribute((weak, alias("Default_Handler")));
extern void SysTick_Handler(void) __attribute((weak, alias("Default_Handler")));

extern void Gpio0_Handler(void)   __attribute((weak, alias("Default_Handler")));
extern void Gpio1_Handler(void)   __attribute((weak, alias("Default_Handler")));
extern void Timer0_Handler(void)  __attribute((weak, alias("Default_Handler")));
extern void Timer1_Handler(void)  __attribute((weak, alias("Default_Handler")));
extern void Uart0_Handler(void)   __attribute((weak, alias("Default_Handler")));
extern void Uart1_Handler(void)   __attribute((weak, alias("Default_Handler")));
extern void Spi0_Handler(void)    __attribute((weak, alias("Default_Handler")));
extern void Spi1_Handler(void)    __attribute((weak, alias("Default_Handler")));
extern void I2c0_Handler(void)    __attribute((weak, alias("Default_Handler")));
extern void I2c1_Handler(void)    __attribute((weak, alias("Default_Handler")));
extern void Rtc0_Handler(void)    __attribute((weak, alias("Default_Handler")));
extern void Rtc1_Handler(void)    __attribute((weak, alias("Default_Handler")));
extern void Dsp_Handler(void)     __attribute((weak, alias("Default_Handler")));
extern void Adc_Handler(void)     __attribute((weak, alias("Default_Handler")));
extern void Pwm_Handler(void)     __attribute((weak, alias("Default_Handler")));
extern void Wdt_Handler(void)     __attribute((weak, alias("Default_Handler")));
extern void RtcTimer_Handler(void) __attribute((weak, alias("Default_Handler")));
extern void Bod_Handler(void)     __attribute((weak, alias("Default_Handler")));
extern void Sw0_Handler(void)     __attribute((weak, alias("Default_Handler")));
extern void Sw1_Handler(void)     __attribute((weak, alias("Default_Handler")));
extern void Sw2_Handler(void)     __attribute((weak, alias("Default_Handler")));
extern void Sw3_Handler(void)     __attribute((weak, alias("Default_Handler")));
extern void Sw4_Handler(void)     __attribute((weak, alias("Default_Handler")));
extern void Sw5_Handler(void)     __attribute((weak, alias("Default_Handler")));

extern void Default_Handler(void);

/***************************************************************************//**
 *
 * The vector table.
 *
 ******************************************************************************/
__attribute__((section(".isr_vector"), used))
void (*const g_pfnVectors[])(void) =
{
    (void (*)(void))((uint32_t)g_pui32Stack + sizeof(g_pui32Stack)),
        // Stack pointer
    Copy2Sram, // Copy2Sram as the default Reset handler.
    Nmi_Handler, // The NMI handler
    Fault_Handler, // The hard fault handler
    MemManage_Handler, // The MPU fault handler
    Bus_Fault_Handler, // The bus fault handler
    Fault_Handler, // The usage fault handler
    0,                // Reserved
    0,                // Reserved
    0,                // Reserved
    0,                // Reserved
    Svc_Handler,      // SVC handler
    DebugMon_Handler, // Debug monitor handler
    0,                // Reserved
    PendSv_Handler,   // The PendSV handler
    SysTick_Handler,  // The SysTick handler

    //
    // External interrupt service routines (ISR).
    //
    Gpio0_Handler, // 0: Gpio0
    Gpio1_Handler, // 1: Gpio1
    Timer0_Handler, // 2: Timer0
    Timer1_Handler, // 3: Timer1
    Uart0_Handler, // 4: Uart0
    Uart1_Handler, // 5: Uart1
    Spi0_Handler, // 6: Spi0
    Spi1_Handler, // 7: Spi1
    I2c0_Handler, // 8: I2c0
    I2c1_Handler, // 9: I2c1
    Rtc0_Handler, // 10: Rtc0
    Rtc1_Handler, // 11: Rtc1
    Dsp_Handler, // 12: Dsp
    Adc_Handler, // 13: Adc
    Pwm_Handler, // 14: Pwm
    Wdt_Handler, // 15: Wdt
    RtcTimer_Handler, // 16: RtcTimer
    Bod_Handler, // 17: Bod
    Sw0_Handler, // 18: Sw0
    Sw1_Handler, // 19: Sw1
    Sw2_Handler, // 20: Sw2
    Sw3_Handler, // 21: Sw3
    Sw4_Handler, // 22: Sw4
    Sw5_Handler, // 23: Sw5

};

/***************************************************************************//**
 *
 * The following are constructs created by the linker, indicating where the
 * the "data" and "bss" segments reside in memory.  The initializers for the
 * for the "data" segment resides immediately following the "text" segment.
 *
 ******************************************************************************/
extern uint32_t _etext;
extern uint32_t _bss;
extern uint32_t _ebss;
extern uint32_t g_pui32FaultInfo;

/***************************************************************************//**
 *
 * Helper functions.
 *
 ******************************************************************************/
void *startup_get_my_pc(void);
void startup_my_sp_set(void *pvSp);
void EtaFaultInfoGet(uint32_t *pui32FaultInfo);

/***************************************************************************//**
 *
 * The entry point for the application.
 *
 ******************************************************************************/
int main(int argc, char *argv[]);

/***************************************************************************//**
 *
 * Fault_Handler - Fault ISR handler.
 *
 ******************************************************************************/
void
Default_Fault_Handler(void)
{
    //
    // This function will find the bottom of stack for the faulting stack and
    // store it into g_pui32FaultInfo. This will point to eight words from
    // the final stack pointer of the faulting program.
    // See the description of EtaFaultInfoGet().
    //
    EtaFaultInfoGet((uint32_t *) &g_pui32FaultInfo);

    //
    // Once it returns it spins here and returns to the debugger so
    // you know right away when it faults and you know which fault.
    //
    while(1)
    {
        __asm volatile ("    BKPT      #1");
    }
}

/***************************************************************************//**
 *
 * Default Bus_Fault_Handler - Bus Fault ISR handler.
 *
 ******************************************************************************/
void
Default_Bus_Fault_Handler(void)
{
    //
    // This function will find the bottom of stack for the faulting stack and
    // store it into g_pui32FaultInfo. This will point to eight words from
    // the final stack pointer of the faulting program.
    // See the description of EtaFaultInfoGet().
    //
    EtaFaultInfoGet((uint32_t *) &g_pui32FaultInfo);

    //
    // Once it returns it spins here and returns to the debugger so
    // you know right away when it faults and you know which fault.
    //
    while(1)
    {
        __asm volatile ("    BKPT      #1");
    }
}

/***************************************************************************//**
 *
 * Default_Handler - Default ISR handler.
 *
 ******************************************************************************/
void
Default_Handler(void)
{
    __asm("    movs     r0, #20");
    while(1)
    {
        __asm("    BKPT      #1");
    }
}

/***************************************************************************//**
 *
 * Copy2Sram - Copy application to SRAM.
 *
 ******************************************************************************/
__attribute__((section(".copy2sram"), used))
void
Copy2Sram(void)
{
    //
    // Copy any .text bytes from beginning of flash to beginning of sram
    //
    __asm("    ldr      r2, =_etext\n"
          "    ldr      r0, =0x01000000\n"
          "    ldr      r1, =0x10000000\n"
          "    nop                  \n"
          "    nop                  \n"
          "text_copy_loop:\n"
          "    ldr      r3, [r0], #4\n"
          "    nop                  \n"
          "    nop                  \n"
          "    str      r3, [r1], #4\n"
          "    nop                  \n"
          "    nop                  \n"
          "    nop                  \n"
          "    cmp      r1, r2\n"
          "    ble      text_copy_loop\n");

    //
    // Set the vector table pointer.
    //
    __asm volatile ("    ldr    r0, =0xE000ED08\n"
                    "    ldr    r1, =g_pfnVectors\n"
                    "    str    r1, [r0]");

    //
    // Zero fill the bss segment.
    //
    __asm volatile("    ldr     r0, =_bss\n"
                   "    ldr     r1, =_ebss\n"
                   "    mov     r2, #0\n"
                   "zero_loop:\n"
                   "        cmp     r0, r1\n"
                   "        it      lt\n"
                   "        strlt   r2, [r0], #4\n"
                   "        blt     zero_loop");

    //
    // Replace Copy2Sram in the vector table with the actual reset handler.
    //
    MEMIO32((&g_pfnVectors[1])) = (uint32_t)Reset_Handler;

    //
    // Call the reset handler.
    //
    Reset_Handler();
}

/***************************************************************************//**
 *
 * Reset_Handler - Reset ISR handler.
 *
 ******************************************************************************/
void __attribute__((weak, naked))
Default_Reset_Handler(void)
{
    //
    // Set the vector table pointer.
    //
    __asm volatile("    ldr    r0, =0xE000ED08\n"
                   "    ldr    r1, =g_pfnVectors\n"
                   "    str    r1, [r0]");

    //
    // Set the stack pointer.
    //
    __asm volatile("    ldr    r0, =0xE000ED08\n"
                   "    ldr    r1, [r0]\n"
                   "    ldr    sp, [r1]");

    //
    // Call the main routine.
    //
    main(0, 0);

    //
    // If main ever returns, trap it here and wake up the debugger if connected.
    //
    while(1)
    {
        __asm("    BKPT      #1");
    }
}

/***************************************************************************//**
 *
 * startup_get_my_pc - Get the current program counter (PC).
 *
 * @return return the current PC.
 *
 ******************************************************************************/
void *
startup_get_my_pc(void)
{
    void *pvPc;

    asm volatile ("mov %0, pc" : "=r" (pvPc));
    return(pvPc);
}

/***************************************************************************//**
 *
 * startup_get_my_sp - Get the current stack pointer (SP).
 *
 * @return return the current stack pointer.
 *
 ******************************************************************************/
void *
startup_get_my_sp(void)
{
    void *pvSp;

    asm volatile ("mov %0, sp" : "=r" (pvSp));
    return(pvSp);
}

/***************************************************************************//**
 *
 * startup_my_sp_set - Set the stack pointer (SP).
 *
 * @param pvSp pointer to the desired stack pointer.
 *
 ******************************************************************************/
void
startup_my_sp_set(void *pvSp)
{
    asm volatile ("mov sp,   %0" : "=r" (pvSp));
}

/***************************************************************************//**
 *
 * EtaFaultInfoGet - Save a pointer to the fault info in high SRAM
 *
 * @param pui32StoreItHere High Sram Address (in R0 at call to this function).
 *
 *       Info[7] = PSR
 *       Info[6] = PC
 *       Info[5] = LR
 *       Info[4] = R12
 *       Info[3] = R3
 *       Info[2] = R2
 *       Info[1] = R1
 *       Info[0] = R0
 ******************************************************************************/
void
EtaFaultInfoGet(uint32_t *pui32StoreItHere)
{
	__asm volatile(" tst lr, #4 \n"
                   " ite eq\n"
                   " mrseq r1, msp\n"
                   " mrsne r1, psp\n"
                   " str r1, [r0, #24]\n"
                   " bx lr\n");
}

//
// This will point to the bottom of the faulting stack.
//
uint32_t g_pui32FaultInfo;


