#include "kernel.h"
#include "Include/idt.h"
#include "Include/pic.h"
#include "Include/pit.h"
#include "Include/serial.h"

void k_init()
{
    serial_init(0x3F8);
    serial_write_line(0x3F8, "[INFO] SERIAL READY");
    serial_write_line(0x3F8, "[INFO] GDT FLUSHED");
    idt_init();
    serial_write_line(0x3F8, "[INFO] Enabling PIC");
    remap_pic();
    serial_write_line(0x3F8, "[INFO] Enabling PIT");
    init_pit(1000);
    serial_write_line(0x3F8, "[INFO] Enabling Interrupts");
    enable_sti();
    serial_write_line(0x3F8, "[INFO] Enabled STI");
    serial_write_line(0x3F8, "[INFO] Sleep Test");
    ksleep(1000);
    serial_write_line(0x3F8, "[INFO] Awake!");

    while(1) {
        asm volatile("hlt");
    }
    int x = 1;
    int y = 0;
    int z = x / y;
}