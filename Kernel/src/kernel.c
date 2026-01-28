#include "Include/serial.h"

void kmain() {
    serial_write_line(0x3F8, "Hello World!");
}