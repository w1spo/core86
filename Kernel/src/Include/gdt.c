#include "gdt.h"
#include "serial.h"

void gdt_init() 
{
    serial_write_line(0x3F8, "[GDT] Initialised");
}