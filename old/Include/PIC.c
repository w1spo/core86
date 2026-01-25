#include "PIC.h"

// W pic_init():
void pic_init(void) {
    // Wyłącz IOAPIC jeśli jest aktywny
    outb(0x70, 0x22);  // CMOS register
    outb(0x71, 0x70);  // Disable IOAPIC
    
    outb(0xA1, 0xFF);  // Mask slave PIC
    outb(0x21, 0xFF);  // Mask master PIC
    
    // Standard PIC initialization
    outb(0x20, 0x11);  // ICW1: edge triggered, cascade, ICW4 needed
    outb(0xA0, 0x11);
    
    outb(0x21, 0x20);  // ICW2 Master: IRQ0-7 -> INT 0x20-0x27
    outb(0xA1, 0x28);  // ICW2 Slave: IRQ8-15 -> INT 0x28-0x2F
    
    outb(0x21, 0x04);  // ICW3 Master: slave at IRQ2
    outb(0xA1, 0x02);  // ICW3 Slave: cascade identity
    
    outb(0x21, 0x01);  // ICW4: 8086 mode
    outb(0xA1, 0x01);
    
    // Mask all interrupts except keyboard (IRQ1)
    outb(0x21, 0xFD);  // 11111101 - tylko IRQ1 enabled
    outb(0xA1, 0xFF);  // 11111111 - wszystkie slave disabled
    
    vga_print("[PIC] Legacy PIC initialized (IRQ1 enabled)\n");
}

void pic_unmask_irq(ukint_8 irq) {
    ukint_16 port = (irq < 8) ? 0x21 : 0xA1;
    ukint_8 mask = inb(port);
    mask &= ~(1 << (irq % 8));
    outb(port, mask);
}

void pic_enable_irq(int irq) {
    ukint_16 port;
    ukint_8 value;
    
    if(irq < 8) {
        port = 0x21;
    } else {
        port = 0xA1;
        irq -= 8;
    }
    
    value = inb(port) & ~(1 << irq);
    outb(port, value);
}


