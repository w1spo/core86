#include "serial.h"
#include "types.h"

const uint32 baud_rate = 115200;

void serial_init(uint16 serial_port)
{

    uint16 divisor = 115200 / baud_rate;
    uint8 divisor_low  = divisor & 0xFF;
    uint8 divisor_high = (divisor >> 8) & 0xFF;
    outb(serial_port + 1, 0x00);
    outb(serial_port + 3, 0x80);
    outb(serial_port + 0, divisor_low);
    outb(serial_port + 1, divisor_high);
    outb(serial_port + 2, 0xC7);
    outb(serial_port + 4, 0x0B);
    outb(serial_port + 3, 0x03);
}

void serial_write_char(uint16 serial_port, char c)
{
    while ((inb(serial_port + 5) & 0x20) == 0)
        ;
    outb(serial_port + 0, c);

}

void serial_write_line(uint16 serial_port, string str)
{
    int i = 0;
    while (str[i] != '\0')
    {
        serial_write_char(serial_port, str[i]);
        i++;
    }
    serial_write_char(serial_port, '\r');
    serial_write_char(serial_port, '\n');
}