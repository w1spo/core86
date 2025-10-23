OUTPUT_FORMAT("binary")
ENTRY(kernel_entry)
SECTIONS
{
    . = 0x100000;   /* adres, gdzie bootloader wczyta kernel */
    .text : { *(.text) }
    .rodata : { *(.rodata) }
    .data : { *(.data) }
    .bss : { *(.bss) }
}
