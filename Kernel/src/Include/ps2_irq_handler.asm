global ps2_irq_handler

ps2_irq_handler:
    pusha
    call ps2_kb_handler
    popa
    mov al, 0x20
    out 0x20, al
    iret