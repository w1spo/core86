#include "IDT.h"

idt_entry_t idt[256];
idt_ptr_t idt_ptr;

// EXTERN TYLKO dla funkcji z ASM
extern void safe_irq_handler_asm(void);
extern void pit_irq_handler_asm(void);  
extern void ps2_irq_handler_asm(void);

void idt_set_gate(int n, ukint32 handler) {
    ukint_16 cs_selector;
    asm volatile("mov %%cs, %0" : "=r"(cs_selector));
    
    idt[n].offset_low  = handler & 0xFFFF;
    idt[n].selector    = cs_selector;  // UŻYJ AKTUALNEGO CS!
    idt[n].zero        = 0;
    idt[n].type_attr   = 0x8E;
    idt[n].offset_high = (handler >> 16) & 0xFFFF;
    
    // Debug
    if(n == 0) {
        char buf[12];
        vga_print("[IDT] Using CS selector: 0x");
        vga_print(hex_to_str(cs_selector, buf));
        vga_print("\n");
    }
}

void IDT_INIT() {
    idt_ptr.limit = sizeof(idt) - 1;
    idt_ptr.base  = (ukint32)&idt;

    // Wypełnij zerami
    for (int i = 0; i < 256; i++) {
        idt[i].offset_low = 0;
        idt[i].selector = 0;
        idt[i].zero = 0;
        idt[i].type_attr = 0;
        idt[i].offset_high = 0;
    }

    // Ustaw default handler dla wszystkich
    for (int i = 0; i < 256; i++) {
        idt_set_gate(i, (ukint32)safe_irq_handler_asm);
    }
    
    // Ustaw konkretne handler - popraw offset!
    // Przerwanie 0x21 = IRQ1 (klawiatura)
    idt_set_gate(0x21, (ukint32)ps2_irq_handler_asm);
    
    // Załaduj IDT
    asm volatile("lidt (%0)" : : "r"(&idt_ptr));
    vga_print("[IDT] Loaded\n");
}