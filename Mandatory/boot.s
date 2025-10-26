.section .multiboot
.align 4

.multiboot_header:
    .long 0x1BADB002              # Magic number
    .long 0x00000003              # Flags
    .long -(0x1BADB002 + 0x00000003) # Checksum

.section .text
.global _start

_start:
    # Ustawienie stosu
    mov $stack_top, %esp
    
    # Wywołanie funkcji kernel_main
    call kernel_main
    
    # Pętla nieskończona jeśli kernel_main wróci
_hang:
    cli
    hlt
    jmp _hang

.section .bss
.space 4096
stack_top: