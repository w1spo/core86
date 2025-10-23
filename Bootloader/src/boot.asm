[bits 16]
[org 0x7C00]

; -----------------------------
; Bootloader x86 Real -> Protected Mode
; Author: Grzegorz
; -----------------------------

jmp start                   ; Pomijamy dane przy starcie

; -----------------------------
; Data Section
; -----------------------------
welcome_msg db 'Welcome to core86 Bootloader...', 0x0D, 0x0A, 0
boot_msg db 'Booting Kernel...', 0x0D, 0x0A, 0
err_msg db 'Boot Failed!', 0x0D, 0x0A, 0

; -----------------------------
; Main Start
; -----------------------------
start:
    cli                     ; Wyłącz przerwania
    
    ; Inicjalizacja segmentów
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7C00          ; Ustaw stos poniżej bootloadera

    ; Wyczyść rejestry
    xor bx, bx
    xor cx, cx
    xor dx, dx

    mov si, welcome_msg
    call print_string
    ; -----------------------------
    ; Print Boot Message
    ; -----------------------------
    mov si, boot_msg
    call print_string

    ; -----------------------------
    ; Enable A20 line
    ; -----------------------------
    call enable_a20

    ; -----------------------------
    ; Load Kernel into 0x10000 (64KB)
    ; -----------------------------
    mov ah, 0x02            ; BIOS read sector
    mov al, 16              ; number of sectors to read
    mov ch, 0               ; cylinder
    mov cl, 2               ; start sector (after boot sector)
    mov dh, 0               ; head
    mov dl, 0x80            ; drive 0x80 = first HDD
    mov bx, 0x0000          ; offset
    mov es, bx              ; segment = 0x0000
    mov bx, 0x1000          ; load at 0x0000:0x1000 = 0x1000 (64KB)
    int 0x13
    jc load_error           ; jump if error

    ; -----------------------------
    ; Disable interrupts and setup GDT
    ; -----------------------------
    cli
    lgdt [gdt_descriptor]

    ; -----------------------------
    ; Enable Protected Mode
    ; -----------------------------
    mov eax, cr0
    or eax, 1
    mov cr0, eax
    
    ; Far jump to flush pipeline and load CS with CODE_SEG
    jmp CODE_SEG:pm_start

; -----------------------------
; GDT Table
; -----------------------------
gdt_start:
    dq 0x0000000000000000        ; null descriptor
    
    ; code segment: base=0, limit=0xFFFFF, exec/read, 4GB, 32-bit
    dq 0x00CF9A000000FFFF        ; CODE_SEG
    
    ; data segment: base=0, limit=0xFFFFF, read/write, 4GB
    dq 0x00CF92000000FFFF        ; DATA_SEG
gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1   ; size
    dd gdt_start                 ; base

CODE_SEG equ 0x08
DATA_SEG equ 0x10

; -----------------------------
; Protected Mode Start (32-bit)
; -----------------------------
[bits 32]
pm_start:
    ; Initialize segment registers with DATA_SEG
    mov ax, DATA_SEG
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    mov esp, 0x90000          ; Ustaw stos wyżej w pamięci

    ; Jump to kernel at 1MB (0x100000)
    jmp 0x100000

; -----------------------------
; Print String Routine (16-bit)
; -----------------------------
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

; -----------------------------
; Load Error Handler
; -----------------------------
load_error:
    mov si, err_msg
    call print_string
.halt:
    cli
    hlt
    jmp .halt               ; Infinite loop

; -----------------------------
; A20 Enable Routine
; -----------------------------
enable_a20:
    push ax
    ; Method 1: Keyboard Controller
    call .a20_wait
    mov al, 0xAD
    out 0x64, al
    
    call .a20_wait
    mov al, 0xD0
    out 0x64, al
    
    call .a20_wait2
    in al, 0x60
    push eax
    
    call .a20_wait
    mov al, 0xD1
    out 0x64, al
    
    call .a20_wait
    pop eax
    or al, 2
    out 0x60, al
    
    call .a20_wait
    mov al, 0xAE
    out 0x64, al
    
    call .a20_wait
    pop ax
    ret

.a20_wait:
    in al, 0x64
    test al, 2
    jnz .a20_wait
    ret

.a20_wait2:
    in al, 0x64
    test al, 1
    jz .a20_wait2
    ret

; -----------------------------
; Bootloader Signature
; -----------------------------
times 510-($-$$) db 0
dw 0xAA55