OUTPUT_FORMAT ("elf32-littlearm", "elf32-bigarm", "elf32-littlearm")
ENTRY(Reset_Handler)
MEMORY
{
    FLASH (rx) : ORIGIN = 0x01006000, LENGTH = (0x7A000 - 0x16000 - 0x0)
    DSP (rx) : ORIGIN = (0x01006000 + (0x7A000 - 0x16000 - 0x0)), LENGTH = 0x16000
    PBUF (rx) : ORIGIN = ((0x01006000 + (0x7A000 - 0x16000 - 0x0)) + 0x16000), LENGTH = 0x0
    RAM (rwx) : ORIGIN = 0x10000000, LENGTH = (0x40000 - (12288 + 0x1200))
    SHM (rwx) : ORIGIN = (0x10040000 - 0x1200), LENGTH = 0x1200
    HEAP (rwx) : ORIGIN = (0x10040000 - (12288 + 0x1200)), LENGTH = 12288
}
_estack = ORIGIN(RAM) + LENGTH(RAM) - 0x20;
SECTIONS
{
  .text :
  {
        __text = .;
        __text_load = LOADADDR(.text);
        __isr_vector_start = .;
        __text_start = .;
        KEEP(*(.isr_vector))
        __isr_vector_end = .;
        *(EXCLUDE_FILE(*eta_csp_buck*.o *eta_csp_dsp*.o *ssd_detection*.o
            *qfplib-m3*.o *stats*.o *printf-stdarg*.o *ecm3532_pinconfig*.o
            *board_config*.o *system_ecm3532*.o *uart_hal*.o *eta_csp_uart*.o
            *eta_csp_flash*.o *eta_csp*.o *libgcc.a) .text* .rodata*)
        *crtbegin.o(.ctors)
        *crtbegin?.o(.ctors)
        *(EXCLUDE_FILE(*crtend?.o *crtend.o) .ctors)
        *(SORT(.ctors.*))
        *(.ctors)
        *crtbegin.o(.dtors)
        *crtbegin?.o(.dtors)
        *(EXCLUDE_FILE(*crtend?.o *crtend.o) .dtors)
        *(SORT(.dtors.*))
        *(.dtors)
        *(.rodata*)
        KEEP(*(.eh_frame*))
    } > FLASH= 0
    .ARM.extab :
    {
        *(.ARM.extab* .gnu.linkonce.armextab.*)
    } > FLASH= 0
    __exidx_start = .;
    .ARM.exidx :
    {
        *(.ARM.exidx* .gnu.linkonce.armexidx.*)
    } > FLASH= 0
    __exidx_end = .;
    _etext = .;
    .shmSectionBlock (NOLOAD) :
    {
        __SHM_SECTION_START = .;
        KEEP(*(.shmSection))
        __SHM_SECTION_END = .;
    } > SHM
    .data :
    {
        . = ALIGN(4);
         _sidata = LOADADDR(.data);
        __data_load = LOADADDR(.data);
        __data_start = .;
        *(vtable)
        *(.data*)
        PROVIDE_HIDDEN (__preinit_array_start = .);
        KEEP(*(.preinit_array))
        PROVIDE_HIDDEN (__preinit_array_end = .);
        . = ALIGN(4);
        PROVIDE_HIDDEN (__init_array_start = .);
        KEEP(*(SORT(.init_array.*)))
        KEEP(*(.init_array))
        PROVIDE_HIDDEN (__init_array_end = .);
        . = ALIGN(4);
        PROVIDE_HIDDEN (__fini_array_start = .);
        KEEP(*(SORT(.fini_array.*)))
        KEEP(*(.fini_array))
        PROVIDE_HIDDEN (__fini_array_end = .);
        KEEP(*(.jcr*))
        . = ALIGN(4);
        _edata = .;
        __data_end__ = _edata;
    } > RAM AT > FLASH
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
    __heap_start__ = .;
    end = .;
    _ram_start = ORIGIN(RAM);
    .stack_dummy (COPY):
    {
        *(.stack*)
    } > RAM
    .binSectionBlock :
    {
        __BIN_SECTION_START = .;
        KEEP(*(.binSection))
        KEEP(*(.init))
        KEEP(*(.fini))
            __BIN_SECTION_END = .;
    } > FLASH
    .initSection :
    {
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
        *libgcc.a:*
    } > FLASH
    .appHeapBlock (NOLOAD) :
    {
        __heap_start__ = .;
        *(.appHeap*)
        __heap_end__ = .;
    } > HEAP
    .dspBlock :
    {
        KEEP(*(.dsp))
    } > DSP
    .pbufBlock :
    {
        KEEP(*(.pbuf))
    } > PBUF
    __StackTop = ORIGIN(RAM) + LENGTH(RAM) - 4;
    __StackLimit = __StackTop - SIZEOF(.stack_dummy) ;
    PROVIDE(__stack = __StackTop);
    __HeapLimit = __StackLimit;
    ASSERT(__HeapBase <= __HeapLimit, "region RAM overflowed with stack")
}
