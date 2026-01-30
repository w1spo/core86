#include "idt.h"
#include "serial.h"
#include "types.h"

struct idt_entry idt[256];
struct idt_ptr idtp;
void dummy_isr()
{
    serial_write_line(0x3F8, "[PANIC] Unxpected Interrupt");
    while(1);
}

void divide_by_zero()
{
    serial_write_line(0x3F8, "[PANIC] Can't divide by zero");
    while(1);
}

void idt_set(int num, void *handler)
{
    idt[num].offset_low = (uint32)handler & 0xFFFF;
    idt[num].selector = 0x08;
    idt[num].zero = 0;
    idt[num].type = 0x8E;
    idt[num].offset_high = ((uint32)handler >> 16) & 0xFFFF;
}

void idt_load(void) {
    idtp.limit = (sizeof(struct idt_entry) * 256) - 1;
    idtp.base = (uint32)&idt;

    asm volatile("lidt (%0)" : : "r" (&idtp));
}

void idt_init()
{
    for (int i = 0; i < 256; i++)
    {
        idt_set(i, dummy_isr);
    }

    idt_set(0, divide_by_zero);

    idt_load();

}

void enable_sti()
{
    asm volatile("sti");
}