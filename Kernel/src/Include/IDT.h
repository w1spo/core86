#ifndef IDT_H
#define IDT_H

#include "KSTINT.h"

typedef struct {
    ukint_16 offset_low;
    ukint_16 selector;
    ukint_8 zero;
    ukint_8 type_attr;
    ukint_16 offset_high;

} __attribute__((packed)) idt_entry_t;

typedef struct {
    ukint_16 limit;   // rozmiar IDT - 1
    ukint32 base;    // adres tablicy IDT
} __attribute__((packed)) idt_ptr_t;

void idt_set_gate(int n, ukint32 handler);
void idt_init(void);
void dummy_handler(void);


#endif