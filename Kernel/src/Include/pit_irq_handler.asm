global pit_irq_handler_asm

pit_irq_handler_asm:
    pusha                 ; zapisujemy wszystkie rejestry
    call pit_irq_handler   ; wywołujemy funkcję C, która zrobi tick
    popa                  ; przywracamy rejestry
    iret                  ; wracamy do przerwanego kodu
