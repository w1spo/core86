#include "Include/gdt.h"
#include "Include/serial.h"

void kmain() {
    serial_write_line(0x3F8, "Hello World!");
    gdt_init();
}