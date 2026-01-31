#ifndef SERIAL_H
#define SERIAL_H

#include "io.h"
#include "types.h"
#include "config.h"


void serial_init(uint16 port);
void serial_write_char(uint16 port, char);
void serial_write_line(uint16 port, string);
void serial_write_string(uint16 port, string);

#endif