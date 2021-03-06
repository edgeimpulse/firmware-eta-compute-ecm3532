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
 *   __ecorebss
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
    #elif defined(CONFIG_DSP_IN_RAM) && defined(CONFIG_PBUF_IN_RAM)
        #if (CONFIG_DSP_FW_LENGTH + CONFIG_PBUF_LENGTH) > (CONFIG_CHIP_RAM_SIZE - RAM_MIN_SIZE)
            #error "DSP and PBUF size greater than ram size"
        #endif

        #define RAM_SIZE   (CONFIG_M3_RAM_SIZE - RAM_MIN_SIZE - CONFIG_DSP_FW_LENGTH - CONFIG_PBUF_LENGTH)
        #define FLASH_SIZE (CONFIG_M3_FLASH_SIZE)
        
        #define DSP_FW_START (CONFIG_CHIP_RAM_START + RAM_SIZE)
        #define PBUF_START (DSP_FW_START + CONFIG_DSP_FW_LENGTH)
    #elif defined(CONFIG_DSP_IN_FLASH) && defined(CONFIG_PBUF_IN_RAM)
        #if CONFIG_DSP_FW_LENGTH > (CONFIG_CHIP_FLASH_SIZE - CONFIG_BOOTLOADER_FLASH_SIZE)
            #error "DSP size greater than flash size"
        #endif
        #if CONFIG_PBUF_LENGTH > (CONFIG_CHIP_RAM_SIZE - RAM_MIN_SIZE)
            #error "PBUF size greater than ram size"
        #endif

        #define RAM_SIZE   (CONFIG_M3_RAM_SIZE - RAM_MIN_SIZE - CONFIG_PBUF_LENGTH)
        #define FLASH_SIZE (CONFIG_M3_FLASH_SIZE - CONFIG_DSP_FW_LENGTH)

        #define DSP_FW_START (CONFIG_BOOTLOADER_APP_FLASH_START + FLASH_SIZE)
        #define PBUF_START (CONFIG_CHIP_RAM_START + RAM_SIZE)
    #elif defined(CONFIG_DSP_IN_RAM) && defined(CONFIG_PBUF_IN_FLASH)
        #if CONFIG_PBUF_LENGTH > (CONFIG_CHIP_FLASH_SIZE - CONFIG_BOOTLOADER_FLASH_SIZE)
            #error "PBUF size greater than flash size"
        #endif
        #if CONFIG_DSP_FW_LENGTH > (CONFIG_CHIP_RAM_SIZE - RAM_MIN_SIZE)
            #error "DSP size greater than ram size"
        #endif

        #define RAM_SIZE   (CONFIG_M3_RAM_SIZE - RAM_MIN_SIZE - CONFIG_DSP_FW_LENGTH)
        #define FLASH_SIZE (CONFIG_M3_FLASH_SIZE - CONFIG_PBUF_LENGTH)

        #define DSP_FW_START (CONFIG_CHIP_RAM_START + RAM_SIZE)
        #define PBUF_START (CONFIG_BOOTLOADER_APP_FLASH_START + FLASH_SIZE)
    #endif
#else
    #define RAM_SIZE   (CONFIG_M3_RAM_SIZE - RAM_MIN_SIZE - CONFIG_DSP_FW_LENGTH)
    #define FLASH_SIZE (CONFIG_M3_FLASH_SIZE)
    #define DSP_FW_START (CONFIG_CHIP_RAM_START + RAM_SIZE)
#endif

MEMORY
{
#ifdef CONFIG_BOOTLOADER_APP
    FLASH (rx) : ORIGIN = CONFIG_BOOTLOADER_APP_FLASH_START, LENGTH = FLASH_SIZE
    DSP (rx)   : ORIGIN = DSP_FW_START, LENGTH = CONFIG_DSP_FW_LENGTH
    PBUF (rx)  : ORIGIN = PBUF_START, LENGTH = CONFIG_PBUF_LENGTH
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
    /* Reserve space at the start of the image for the header. */

  .text :
  {
        __text = .;
        __text_load = LOADADDR(.text);
        __isr_vector_start = .;
        __text_start = .;
        KEEP(*(.isr_vector))
        __isr_vector_end = .;
        *(.text*)

        KEEP(*(.init))
        KEEP(*(.fini))

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

        *(.rodata*)

        KEEP(*(.eh_frame*))
    } > RAM


    _etext = .;

   _sidata = LOADADDR(.data);

    .data :
    {
        . = ALIGN(4);
        __data_load = LOADADDR(.data);
        _sdata = .;
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
        __data_end__ = _edata;

    } > RAM

    .bss :
    {
        . = ALIGN(4);
        _sbss = .;
        __bss_start__ = _sbss;
        *(.bss*)
        *(COMMON)
        . = ALIGN(4);
        _ebss = .;
        __bss_end__ = _ebss;
    } > RAM
    /* shared mem section */
    .shmSectionBlock (NOLOAD):
    {
        __SHM_SECTION_START = .;  /* create symbol for start of section */
        KEEP(*(.shmSection))
        __SHM_SECTION_END = .; /* create symbol for end of section */
    } > SHM
    .vector_relocation :
    {
        __vector_tbl_reloc__ = .;
        . = . + (__isr_vector_end - __isr_vector_start);
        . = ALIGN(4);
    } > RAM
    . = ALIGN(8);
    end = .;
    __HeapBase = .;
    _ram_start = ORIGIN(RAM);
    /* .stack_dummy section doesn't contains any symbols. It is only
     * used for linker to calculate size of stack sections, and assign
     * values to stack symbols later */
    .stack_dummy (COPY):
    {
        *(.stack*)
    } > RAM

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
    } > DSP

    .pbufBlock :
    {
        KEEP(*(.pbuf))
    } > PBUF
#else
    .dspBlock :
    {
        KEEP(*(.dsp))
    } > DSP

    .pbufBlock :
    {
        KEEP(*(.pbuf))
    } > RAM
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
