#include "PIC.h"

void pic_init() {
    outb(0x20, 0x11); // ICW1 master
    outb(0xA0, 0x11); // ICW1 slave
    outb(0x21, 0x20); // ICW2 master offset 0x20 (INT 32)
    outb(0xA1, 0x28); // ICW2 slave offset 0x28
    outb(0x21, 0x04); // ICW3 master
    outb(0xA1, 0x02); // ICW3 slave
    outb(0x21, 0x01); // ICW4 master
    outb(0xA1, 0x01); // ICW4 slave

    // na start wszystkie IRQ zablokowane
    outb(0x21, 0xFF);
    outb(0xA1, 0xFF);
}

void pic_unmask_irq(ukint_8 irq) {
    ukint_16 port = (irq < 8) ? 0x21 : 0xA1;
    ukint_8 mask = inb(port);
    mask &= ~(1 << (irq % 8));
    outb(port, mask);
}
