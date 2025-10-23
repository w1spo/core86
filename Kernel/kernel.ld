OUTPUT_FORMAT("binary")
ENTRY(_start)
SECTIONS
{
    . = 0x0000;
    
    .text : {
        *(.text)
    }
    
    .data : {
        *(.data)
        *(.rodata)
    }
    
    .bss : {
        *(.bss)
    }
}