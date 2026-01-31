#include "kernel.h"
#include "Include/PMM.h"
#include "Include/idt.h"
#include "Include/pic.h"
#include "Include/pit.h"
#include "Include/serial.h"
#include "Include/config.h"

void k_init()
{
    serial_init(COM1_PORT);
    serial_write_line(COM1_PORT, "[INFO] SERIAL READY");
    serial_write_line(COM1_PORT, "[INFO] GDT FLUSHED");
    idt_init();
    serial_write_line(COM1_PORT, "[INFO] Enabling PIC");
    remap_pic();
    serial_write_line(COM1_PORT, "[INFO] Enabling PIT");
    init_pit(1000);
    serial_write_line(COM1_PORT, "[INFO] Enabling Interrupts");
    enable_sti();
    serial_write_line(COM1_PORT, "[INFO] Enabled STI");
    pmm_init(512);
    pmm_test();
    serial_write_line(COM1_PORT, "[INFO] Sleep Test");
    ksleep(1000);
    serial_write_line(COM1_PORT, "[INFO] Awake!");
    serial_write_line(COM1_PORT, "");
    serial_write_line(COM1_PORT, KERNEL_NAME);
    serial_write_line(COM1_PORT, KERNEL_VER);
    serial_write_line(COM1_PORT, BUILD_NO);

    while(1) {
        asm volatile("hlt");
    }
    int x = 1;
    int y = 0;
    int z = x / y;
}