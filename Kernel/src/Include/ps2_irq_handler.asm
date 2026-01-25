global ps2_irq_asm_handler
extern PS2_IRQ_HANDLER

ps2_irq_asm_handler:
    pusha
    call PS2_IRQ_HANDLER
    popa
    mov al, 0x20
    out 0x20, al
    iret