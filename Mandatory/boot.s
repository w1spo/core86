.section .multiboot
.align 4

multiboot_header:
    .long 0x1BADB002
    .long 0x00000003
    .long -(0x1BADB002 + 0x00000003)

.section .bss
.align 16

multiboot_magic:
    .skip 4
multiboot_info:
    .skip 4

stack_bottom:
    .skip 16384
stack_top:

.section .data
.align 16

gdt_start:
    .quad 0x0000000000000000
gdt_code_kernel:
    .quad 0x00CF9A000000FFFF
gdt_data_kernel:
    .quad 0x00CF92000000FFFF      
gdt_end:

gdtr:
    .word gdt_end - gdt_start - 1
    .long gdt_start

.set CODE_SEG, 0x08
.set DATA_SEG, 0x10

.section .text
.global _start

_start:
    movl %eax, multiboot_magic
    movl %ebx, multiboot_info
    cli
    
    
    movl $0x0010A000, %edi
    movl $gdt_start, %esi
    movl $(gdt_end - gdt_start), %ecx
    rep movsb
    
    
    movl $0x0010A000, gdtr + 2
    lgdt (gdtr)
    
    
    movl %cr0, %eax
    orl $0x1, %eax
    movl %eax, %cr0
    
    
    ljmp $CODE_SEG, $reload_segments

reload_segments:
    
    movw $DATA_SEG, %ax
    movw %ax, %ds
    movw %ax, %es
    movw %ax, %fs
    movw %ax, %gs
    movw %ax, %ss
    
    
    movl $stack_top, %esp
    andl $-16, %esp
    
    
    pushl multiboot_info
    pushl multiboot_magic
    
    
    call k_init

kernel_halt:
    cli
    hlt
    jmp kernel_halt