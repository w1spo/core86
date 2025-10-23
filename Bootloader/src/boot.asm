[bits 16]
[org 0x7C00]

jmp start

welcome_msg db '===== core86 Bootloader =====', 0x0D,0x0A,0
boot_msg    db 'Booting Kernel...',0x0D,0x0A,0
err_msg     db 'Boot Failed!',0x0D,0x0A,0
load_msg    db 'Loading kernel from disk...',0x0D,0x0A,0
jump_msg    db 'Jumping to kernel...',0x0D,0x0A,0

start:
    cli
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7C00

    mov si, welcome_msg
    call print_string
    mov si, boot_msg
    call print_string

    ; Load kernel to 0x1000:0x0000 (0x10000 physical)
    mov si, load_msg
    call print_string
    
    mov ah, 0x02        ; BIOS read sector
    mov al, 10          ; number of sectors (możesz zwiększyć)
    mov ch, 0           ; cylinder
    mov cl, 2           ; starting sector
    mov dh, 0           ; head
    mov dl, 0x80        ; first HDD
    mov bx, 0x1000      ; segment
    mov es, bx
    mov bx, 0x0000      ; offset
    int 0x13
    jc load_error

    mov si, jump_msg
    call print_string

    ; Jump to kernel at 0x1000:0x0000
    jmp 0x1000:0x0000

print_string:
    pusha
    mov ah, 0x0E
.loop:
    lodsb
    cmp al, 0
    je .done
    int 0x10
    jmp .loop
.done:
    popa
    ret

load_error:
    mov si, err_msg
    call print_string
.halt:
    cli
    hlt
    jmp .halt

times 510-($-$$) db 0
dw 0xAA55