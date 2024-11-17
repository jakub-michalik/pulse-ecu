/* Linker script for STM32F407 (1MB flash, 192KB RAM) */

MEMORY
{
    FLASH (rx)  : ORIGIN = 0x08000000, LENGTH = 1024K
    SRAM  (rwx) : ORIGIN = 0x20000000, LENGTH = 128K
    CCMRAM(rwx) : ORIGIN = 0x10000000, LENGTH = 64K
}

ENTRY(Reset_Handler)

_Min_Heap_Size  = 0x200;
_Min_Stack_Size = 0x400;

SECTIONS
{
    .isr_vector : { . = ALIGN(4); KEEP(*(.isr_vector)) . = ALIGN(4); } > FLASH
    .text :
    {
        . = ALIGN(4);
        *(.text) *(.text*) *(.glue_7) *(.glue_7t)
        KEEP(*(.init)) KEEP(*(.fini))
        . = ALIGN(4); _etext = .;
    } > FLASH
    .rodata : { . = ALIGN(4); *(.rodata) *(.rodata*) . = ALIGN(4); } > FLASH
    .data :
    {
        . = ALIGN(4); _sdata = .;
        *(.data) *(.data*)
        . = ALIGN(4); _edata = .;
    } > SRAM AT > FLASH
    .bss :
    {
        . = ALIGN(4); _sbss = .;
        *(.bss) *(.bss*) *(COMMON)
        . = ALIGN(4); _ebss = .;
    } > SRAM
    ._user_heap_stack :
    {
        . = ALIGN(8);
        PROVIDE(end = .);
        . = . + _Min_Heap_Size;
        . = . + _Min_Stack_Size;
        . = ALIGN(8);
    } > SRAM
}
