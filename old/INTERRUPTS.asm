[BITS 32]

global safe_irq_handler_asm
global ps2_irq_handler_asm

extern PS2_KB_HANDLER  ; <-- WAŻNE: to musi być w C!

; -------------------------------------------------
; SAFE HANDLER dla reszty przerwań
; -------------------------------------------------
safe_irq_handler_asm:
    push eax
    mov al, 0x20
    out 0x20, al      ; EOI
    pop eax
    iret

; -------------------------------------------------
; KEYBOARD HANDLER - TEN MUSI DZIAŁAĆ
; -------------------------------------------------
ps2_irq_handler_asm:
    ; 1. Zapisz WSZYSTKIE rejestry
    pushad            ; EAX, ECX, EDX, EBX, ESP, EBP, ESI, EDI
    push ds
    push es
    push fs
    push gs
    
    ; 2. Załaduj kernel data segments
    mov ax, 0x10      ; Data segment selector
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    
    ; 3. Wywołaj C funkcję
    call PS2_KB_HANDLER
    
    ; 4. EOI
    mov al, 0x20
    out 0x20, al
    
    ; 5. Przywróć rejestry
    pop gs
    pop fs
    pop es
    pop ds
    popad
    
    ; 6. Wróć
    iret