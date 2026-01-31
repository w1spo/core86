#include "idt.h"
#include "pit.h"
#include "serial.h"
#include "types.h"
#include "utils.h"

struct idt_entry idt[256];
struct idt_ptr idtp;

void dummy_isr(void) {
    uint32 int_no, eip;
    
    
    __asm__ volatile(
        "mov 4(%%esp), %0\n"
        "mov 8(%%esp), %1"
        : "=r"(int_no), "=r"(eip)
    );
    
    
    serial_write_line(COM1_PORT, "[PANIC] Unexpected interrupt");
    
    
    char msg1[50];
    itoa(int_no, msg1, 10);
    serial_write_line(COM1_PORT, msg1);
    
    
    char msg2[50];
    serial_write_line(COM1_PORT, "EIP: ");
    hex_to_str(eip, msg2);
    serial_write_line(COM1_PORT, msg2);
    
    
    outb(0x20, 0x20);
    outb(0xA0, 0x20);
    
    while(1);
}

void divide_by_zero()
{
    serial_write_line(COM1_PORT, "[PANIC] Can't divide by zero");
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
    idt_set(32, pit_handler);

    idt_load();

}

void enable_sti()
{
    asm volatile("sti");
}