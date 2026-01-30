#include "pic.h"
#include "io.h"
#include "serial.h"

void remap_pic()
{
    uint8 mask1 = inb(PIC1_DATA);
    uint8 mask2 = inb(PIC2_DATA);

    outb(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4);
    outb(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);
    outb(PIC1_DATA, 32);
    outb(PIC2_DATA, 40);
    outb(PIC1_DATA, 0x04);
    outb(PIC2_DATA, 0x02);
    outb(PIC1_DATA, ICW4_8086);
    outb(PIC2_DATA, ICW4_8086);
    outb(PIC1_DATA, mask1);
    outb(PIC2_COMMAND, mask2);
    outb(0x21, 0xFE);
    outb(0xA1, 0xFF);
    serial_write_line(0x3F8, "[INFO] PIC remapped (IRQ0->32)");

}