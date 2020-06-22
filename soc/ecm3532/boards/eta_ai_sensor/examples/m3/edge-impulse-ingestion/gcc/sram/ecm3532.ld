/******************************************************************************
 *
 * ecm3532.ld - Linker script for sram applications using GCC.
 *
 *****************************************************************************/
ENTRY(Reset_Handler)

MEMORY
{
    FLASH (rx) : ORIGIN = 0x01000000, LENGTH = 512K
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
    } > SRAM= 0

    .dummy :
    {
        . = ALIGN(0x4);
        _eftext = .;
    } > SRAM

    .data :
       AT (ADDR(.text) + SIZEOF(.text) )
    {
        _data = .;
        *(.data*)
        _edata = .;
    } > SRAM

    .bss :
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


