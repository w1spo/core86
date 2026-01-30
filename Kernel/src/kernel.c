#include "kernel.h"
#include "Include/idt.h"
#include "Include/serial.h"

void k_init()
{
    serial_init(0x3F8);
    serial_write_line(0x3F8, "[INFO] SERIAL READY");
    serial_write_line(0x3F8, "[INFO] GDT FLUSHED");
    idt_init();
    enable_sti();

    int x = 1;
    int y = 0;
    int z = x / y;
}