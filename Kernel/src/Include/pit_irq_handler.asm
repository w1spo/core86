global pit_irq_handler_asm
extern PIT_IRQ_HANDLER

pit_irq_handler_asm:
    pusha
    call PIT_IRQ_HANDLER
    jmp .done
    .error:
        cli
        hlt      ; <-- jeśli coś poszło źle, CPU stoi w hlt
    .done:
    popa
    mov al, 0x20
    out 0x20, al
    iret
