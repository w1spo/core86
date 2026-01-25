#include "IDT.h"

idt_entry_t idt[256];   // 256 możliwych przerwań
idt_ptr_t idt_ptr;

extern void idt_load(ukint32);          // ASM do wczytania IDT
extern void pit_irq_handler_asm(void);   // ASM wrapper dla PIT
extern void ps2_irq_asm_handler(void);

void idt_set_gate(int n, ukint32 handler) {
    idt[n].offset_low  = handler & 0xFFFF;
    idt[n].selector    = 0x08;  // kernel code segment w GDT
    idt[n].zero        = 0;
    idt[n].type_attr   = 0x8E;  // present, ring 0, interrupt gate
    idt[n].offset_high = (handler >> 16) & 0xFFFF;
}

void dummy_handler() {
    asm volatile("iret");
}

void IDT_INIT() {
    idt_ptr.limit = sizeof(idt) - 1;
    idt_ptr.base  = (ukint32)&idt;

    // wszystkie przerwania ustawiamy na dummy
    for (int i = 0; i < 256; i++) {
        idt_set_gate(i, (ukint32)dummy_handler);
    }

    // PIT IRQ0 → INT 32
    idt_set_gate(32, (ukint32)pit_irq_handler_asm);
    idt_set_gate(33, (ukint32)ps2_irq_asm_handler);

    // wczytujemy IDT do CPU
    idt_load((ukint32)&idt_ptr);
}

void fault_handler(struct regs *r) {
    vga_print("CPU FAULT!\n");
    vga_print("EIP: "); print_hex(r->eip); vga_print("\n");
    vga_print("CS: "); print_hex(r->cs); vga_print("\n");
    vga_print("EFLAGS: "); print_hex(r->eflags); vga_print("\n");

    while(1) { asm volatile("hlt"); }  // <-- CPU stoi w miejscu
}
