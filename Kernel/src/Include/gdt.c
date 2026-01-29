#include "gdt.h"
#include "serial.h"
#include "types.h"


struct gdt_entry gdt[3];
struct gdtr gdt_ptr;

void gdt_set(int i, uint32 base, uint32 limit, uint8 access, uint8 gran)
{
    gdt[i].base_low = base & 0xFFFF;
    gdt[i].base_middle = (base >> 16) & 0xFF;
    gdt[i].base_high = (base >> 24) & 0xFF;
    gdt[i].limit_low = limit & 0xFFFF;
    gdt[i].granularity = ((limit >> 16) & 0x0F) | (gran & 0xF0);
    serial_write_line(0x3F8, "[GDT] Set");
}

void gdt_init()
{

    gdt_ptr.limit = sizeof(gdt) - 1;
    gdt_ptr.base = (uintptr)&gdt;
    gdt_set(1, 0, 0xFFFFFFFF, 0x9A, 0xCF);
    gdt_set(2, 0, 0xFFFFFFFF, 0x92, 0xCF);
    gdt_flush((uintptr)&gdt_ptr);
    serial_write_line(0x3F8, "[GDT] Initialize");
}

int gdt_flush(uintptr gdtr_adr)
{
    serial_write_line(0x3F8, "[GDT] Flushing Started");
    asm volatile("lgdt (%0)" : : "r" (gdtr_adr));
    serial_write_line(0x3F8, "[GDT] Flushed OK");
    return 0;
}
