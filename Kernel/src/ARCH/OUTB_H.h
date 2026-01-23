#ifndef OUTB_H
#define OUTB_H
#include "kstint.h"

void outb(ukint_16 port, ukint_8 value);
unsigned char inb(ukint_16 port);

#endif