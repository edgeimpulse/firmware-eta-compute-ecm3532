/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */
#include <config.h>
OUTPUT_FORMAT ("elf32-littlearm", "elf32-bigarm", "elf32-littlearm")

/* Linker script to place sections and symbol values. Should be used together
 * with other linker script that defines memory regions FLASH and RAM.
 * It references following symbols, which must be defined in code:
 *   Reset_Handler : Entry of reset handler
 *
 * It defines following symbols, which code can use without definition:
 *   __exidx_start
 *   __exidx_end
 *   __etext
 *   __data_start__
 *   __preinit_array_start
 *   __preinit_array_end
 *   __init_array_start
 *   __init_array_end
 *   __fini_array_start
 *   __fini_array_end
 *   __data_end__
 *   __bss_start__
 *   __bss_end__
 *   __end__
 *   end
 *   __HeapBase
 *   __HeapLimit
 *   __StackLimit
 *   __StackTop
 *   __stack
 *   __coredata_start__
 *   __coredata_end__
 *   __corebss_start__
 *   __corebss_end__
 *   __ecoredata
 *   __ecorebss6
 */
ENTRY(Reset_Handler)

#if CONFIG_M3_RAM_SIZE < CONFIG_HEAP_SIZE
    #error "RAM size less than HEAP size"
#endif

#define RAM_MIN_SIZE (CONFIG_HEAP_SIZE + CONFIG_SHM_LENGTH)
#define HEAP_START (CONFIG_CHIP_RAM_MAX - RAM_MIN_SIZE)
#define SHM_START (CONFIG_CHIP_RAM_MAX - CONFIG_SHM_LENGTH)

#ifdef CONFIG_BOOTLOADER_APP
    #define FLASH_END (CONFIG_BOOTLOADER_APP_FLASH_START + CONFIG_M3_FLASH_SIZE)
    #define RAM_END   (CONFIG_CHIP_RAM_MAX - RAM_MIN_SIZE)

    #ifdef CONFIG_DSP_IN_FLASH
        #if (CONFIG_DSP_FW_LENGTH % 4096) != 0
            #error "DSP FW length need to be 4K aligned"
        #endif
    #endif
    #ifdef CONFIG_PBUF_IN_FLASH
        #if (CONFIG_PBUF_LENGTH % 4096) != 0
            #error "PBUF length need to be 4K aligned"
        #endif
    #endif
    #if (CONFIG_M3_FLASH_SIZE % 4096) != 0
        #error "M3 FLASH size need to be 4K aligned"
    #endif

    #if defined(CONFIG_DSP_IN_FLASH) && defined(CONFIG_PBUF_IN_FLASH)
        #if (CONFIG_DSP_FW_LENGTH + CONFIG_PBUF_LENGTH) > (CONFIG_CHIP_FLASH_SIZE - CONFIG_BOOTLOADER_FLASH_SIZE)
            #error "DSP and PBUF size greater than flash size"
        #endif

        #define RAM_SIZE   (CONFIG_M3_RAM_SIZE - RAM_MIN_SIZE)
        #define FLASH_SIZE (CONFIG_M3_FLASH_SIZE - CONFIG_DSP_FW_LENGTH - CONFIG_PBUF_LENGTH)
        
        #define DSP_FW_START (CONFIG_BOOTLOADER_APP_FLASH_START + FLASH_SIZE)
        #define DSP_END (DSP_FW_START + CONFIG_DSP_FW_LENGTH)
        #define PBUF_START DSP_END
    #elif defined(CONFIG_DSP_IN_FLASH) && defined(CONFIG_PBUF_IN_RAM)
        #if CONFIG_DSP_FW_LENGTH > (CONFIG_CHIP_FLASH_SIZE - CONFIG_BOOTLOADER_FLASH_SIZE)
            #error "DSP size greater than flash size"
        #endif
        #if CONFIG_PBUF_LENGTH > (CONFIG_CHIP_RAM_SIZE - RAM_MIN_SIZE)
            #error "PBUF size greater than ram size"
        #endif

        #define RAM_SIZE   (CONFIG_M3_RAM_SIZE - RAM_MIN_SIZE - CONFIG_PBUF_LENGTH)
        #define FLASH_SIZE (CONFIG_M3_FLASH_SIZE - CONFIG_DSP_FW_LENGTH - CONFIG_PBUF_LENGTH)
        
        #define DSP_FW_START (CONFIG_BOOTLOADER_APP_FLASH_START + FLASH_SIZE)
        #define DSP_END (DSP_FW_START + CONFIG_DSP_FW_LENGTH)
        #define PBUF_START DSP_END

        #define PBUF_RAM_START (CONFIG_CHIP_RAM_START + RAM_SIZE)
    #else
        #error "DSP has to be in FLASH in relocate to RAM build"
    #endif
#else
    #define RAM_SIZE   (CONFIG_M3_RAM_SIZE - RAM_MIN_SIZE)
    #define FLASH_SIZE (CONFIG_M3_FLASH_SIZE - CONFIG_DSP_FW_LENGTH)
    #define DSP_FW_START (CONFIG_CHIP_FLASH_START + FLASH_SIZE)
#endif

MEMORY
{
#ifdef CONFIG_BOOTLOADER_APP
    FLASH (rx) : ORIGIN = CONFIG_BOOTLOADER_APP_FLASH_START, LENGTH = FLASH_SIZE
    DSP (rx)   : ORIGIN = DSP_FW_START, LENGTH = CONFIG_DSP_FW_LENGTH
    PBUF (rx)  : ORIGIN = PBUF_START, LENGTH = CONFIG_PBUF_LENGTH
    #ifdef CONFIG_PBUF_IN_RAM
        PBUF_RAM (rwx)  : ORIGIN = PBUF_RAM_START, LENGTH = CONFIG_PBUF_LENGTH
    #endif
#else
    FLASH (rx) : ORIGIN = CONFIG_CHIP_FLASH_START, LENGTH = FLASH_SIZE
    DSP (rx)   : ORIGIN = DSP_FW_START, LENGTH = CONFIG_DSP_FW_LENGTH
#endif
    RAM (rwx)  : ORIGIN = CONFIG_CHIP_RAM_START, LENGTH = RAM_SIZE
    SHM (rwx) : ORIGIN = SHM_START, LENGTH = CONFIG_SHM_LENGTH
    HEAP (rwx) : ORIGIN = HEAP_START, LENGTH = CONFIG_HEAP_SIZE
}

_estack = ORIGIN(RAM) + LENGTH(RAM) - 0x20;

SECTIONS
{
  .reset :
  {
    __reset_start = .;
        KEEP(*(.isr_vector))
        . = ALIGN(0x4);
    __reset_end = .;
  } >FLASH

  .ram_vectors (NOLOAD) : {
    __ram_vectors_start = .;
    * (.ram_vectors);
    . = ALIGN(4); /* section size must be multiply of 4 */
    __ram_vectors_end = .;
  } > RAM
  __ram_vectors_size = __ram_vectors_end - __ram_vectors_start;
  .text :
  {
    CREATE_OBJECT_SYMBOLS
        __text_load = LOADADDR(.text);
        __text_start = .;
        /*
        *(.text .text.* .gnu.linkonce.t.*)
        KEEP(*(.init))
        KEEP(*(.fini))
        */
        *(EXCLUDE_FILE(*eta_csp_buck*.o  *eta_csp_dsp*.o *ssd_detection*.o
            *qfplib-m3*.o *stats*.o *printf-stdarg*.o *ecm3532_pinconfig*.o
            *board_config*.o *system_ecm3532*.o *uart_hal*.o *eta_csp_uart*.o
            *eta_csp_flash*.o *eta_csp*.o *libgcc.a*) .text* .rodata*)
 
        /* .ctors */
        *crtbegin.o(.ctors)
        *crtbegin?.o(.ctors)
        *(EXCLUDE_FILE(*crtend?.o *crtend.o) .ctors)
        *(SORT(.ctors.*))
        *(.ctors)

        /* .dtors */
        *crtbegin.o(.dtors)
        *crtbegin?.o(.dtors)
        *(EXCLUDE_FILE(*crtend?.o *crtend.o) .dtors)
        *(SORT(.dtors.*))
        *(.dtors)

        *(.rodata .rodata.* .gnu.linkonce.r.*)

        KEEP(*(.eh_frame*))
        _etext = .;
    } >RAM AT>FLASH


    /* shared mem section */
    .shmSectionBlock (NOLOAD):
    {
        __SHM_SECTION_START = .;  /* create symbol for start of section */
        KEEP(*(.shmSection))
        __SHM_SECTION_END = .; /* create symbol for end of section */
    } > SHM

    .data :
    {
        __data_load = LOADADDR(.data);
        . = ALIGN(4);
        __data_start = .;
        *(vtable)
        *(.data*)

        /* preinit data */
        PROVIDE_HIDDEN (__preinit_array_start = .);
        KEEP(*(.preinit_array))
        PROVIDE_HIDDEN (__preinit_array_end = .);

        . = ALIGN(4);
        /* init data */
        PROVIDE_HIDDEN (__init_array_start = .);
        KEEP(*(SORT(.init_array.*)))
        KEEP(*(.init_array))
        PROVIDE_HIDDEN (__init_array_end = .);


        . = ALIGN(4);
        /* finit data */
        PROVIDE_HIDDEN (__fini_array_start = .);
        KEEP(*(SORT(.fini_array.*)))
        KEEP(*(.fini_array))
        PROVIDE_HIDDEN (__fini_array_end = .);

        KEEP(*(.jcr*))
        . = ALIGN(4);
        /* All data end */
	    _edata = .;

    } >RAM AT>FLASH

   .bss :
    {
        . = ALIGN(4);
        __bss_start__ = .;
        *(.bss*)
        *(COMMON)
        . = ALIGN(4);
        __bss_end__ = .;
    } > RAM

    . = ALIGN(8);
    __HeapBase = .;
    end = .;
    _ram_start = ORIGIN(RAM);

    /* .stack_dummy section doesn't contains any symbols. It is only
     * used for linker to calculate size of stack sections, and assign
     * values to stack symbols later */
    .stack_dummy (COPY):
    {
        *(.stack*)
    } > RAM

    __exidx_start = .;
    .ARM.exidx :
    {
        *(.ARM.exidx* .gnu.linkonce.armexidx.*)
    } > FLASH
    __exidx_end = .;

    /* bin section */
    .binSection :
    {
        __BIN_SECTION_START = .;  /* create symbol for start of section */
        KEEP(*(.binSection))
        __BIN_SECTION_END = .; /* create symbol for end of section */
    } > FLASH
    .initSection :
    {
        KEEP(*(.init))
        KEEP(*(.fini))
        *eta_csp_buck*.o(.text* .rodata*)
        *eta_csp_dsp*.o(.text* .rodata*)
        *ssd_detection*.o(.text* .rodata*)
        *qfplib-m3*.o(.text* .rodata*)
        *stats*.o(.text* .rodata*)
        *arm_addsubsf3*.o(.text* .rodata*)
        *_arm_fixsfsi*.o(.text* .rodata*)
        *printf-stdarg*.o(.text* .rodata*)
        *ecm3532_pinconfig*.o(.text* .rodata*)
        *board_config*.o(.text* .rodata*)
        *system_ecm3532*.o(.text* .rodata*)
        *uart_hal*.o(.text* .rodata*)
        *eta_csp_uart*.o(.text* .rodata*)
        *eta_csp_flash*.o(.text* .rodata*)
        *eta_csp*.o(.text* .rodata*)
        */libgcc.a:*
        */libc.a:*
        */libm.a:*

    } > FLASH

    .appHeapBlock (NOLOAD) :
    {
        __heap_start__ = .;
        *(.appHeap*)
        __heap_end__ = .;
    } > HEAP

#ifdef CONFIG_BOOTLOADER_APP
    .dspBlock :
    {
        KEEP(*(.dsp))
    } >DSP
#ifdef CONFIG_PBUF_IN_RAM
    .pbufBlock :
    {
        __pbuf_load = LOADADDR(.pbufBlock);
        __pbuf_start = .;
        KEEP(*(.pbuf))
        _epbuf = .;
    } > PBUF_RAM AT>PBUF
#else
    .pbufBlock :
    {
        KEEP(*(.pbuf))
    } >PBUF
#endif
#else
    .dspBlock :
    {
        KEEP(*(.dsp))
    } >DSP

    .pbufBlock :
    {
        KEEP(*(.pbuf))
    } >RAM AT>FLASH
#endif

    /* Set stack top to end of RAM; stack limit is bottom of stack */
    __StackTop = ORIGIN(RAM) + LENGTH(RAM) - 4;
    __StackLimit = __StackTop - SIZEOF(.stack_dummy) ;
    PROVIDE(__stack = __StackTop);

    /* Top of head is the bottom of the stack */
    __HeapLimit = __StackLimit;

    /* Check if data + heap + stack exceeds RAM limit */
    ASSERT(__HeapBase <= __HeapLimit, "region RAM overflowed with stack")
}
