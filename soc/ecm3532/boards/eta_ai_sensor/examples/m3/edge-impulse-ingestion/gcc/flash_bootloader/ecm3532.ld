/******************************************************************************
 *
 * ecm3532.ld - Linker script for flash applications using GCC.
 *
 *****************************************************************************/
ENTRY(Reset_Handler)

MEMORY
{
    FLASH (rx) : ORIGIN = 0x01005000, LENGTH = 492K
    SRAM (rwx) : ORIGIN = 0x10000000, LENGTH = 256K
}

SECTIONS
{
    .text :
    {
        _text = .;
        KEEP(*(.isr_vector))
        . = ALIGN(0x4);
        *(.text*)
        . = ALIGN(0x4);
        *(.rodata*)
        . = ALIGN(0x4);
        _etext = .;
    } > FLASH= 0

        .ARM :
    {
        __exidx_start = .;
        *(.gnu.linkonce.d*)
        __exidx_end = .;
    } > FLASH

    .dummy :
    {
        . = ALIGN(0x4);
        _eftext = .;
    } > FLASH

    .data :
    {
        _data = .;
        *(.data*)
        _edata = .;
    } > SRAM AT > FLASH

    .bss (NOLOAD) :
    {
        _bss = .;
        *(.bss*)
        *(COMMON)
        _ebss = .;
    } > SRAM

    .stack (NOLOAD):
    {
        *(.stack*)
    } > SRAM

    .heap :
    {
        __heap_start__ = .;
        end = __heap_start__;
        _end = end;
        __end = end;
        KEEP(*(.heap))
        __heap_end__ = .;
        __HeapLimit = __heap_end__;
    } > SRAM

    /DISCARD/ :
    {
        *(.ARM.exidx*)
        *(.gnu.linkonce.armexidx.*)
    }
}


