#ifndef SERIAL_H
#define SERIAL_H

#include "io.h"
#include "types.h"

extern const uint16 serial_port;

void serial_init(uint16 port);
void serial_write_char(uint16 port, char);
void serial_write_line(uint16 port, char* str);

#endif