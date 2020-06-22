/******************************************************************************
 *
 * ecm3532.ld - Linker script for flash_boot_sram applications using GCC.
 *
 *****************************************************************************/
ENTRY(Copy2Sram)

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
        *(.data*)
        . = ALIGN(4);
    } > SRAM AT > FLASH = 0

    .ARM :
    {
        __exidx_start = .;
        *(.gnu.linkonce.d*)
        __exidx_end = .;
        _etext = .;
    } > SRAM AT > FLASH

    .copy2sram :
    {
        KEEP(*(.copy2sram))
        . = ALIGN(4);
    } > FLASH

    .bss (NOLOAD):
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
}


