#ifndef GDT_H
#define GDT_H

#include "types.h"

struct gdt_entry {

    uint16 limit_low;
    uint16 base_low;
    uint8 base_middle;
    uint8 access;
    uint8 granularity;
    uint8 base_high;
}__attribute__((packed));

struct gdtr {
    uint16 limit;
    uint32 base;
}__attribute__((packed));

void gdt_init(void);
void gdt_set(int i, uint32 base, uint32 limit, uint8 access, uint8 gran);

#endif