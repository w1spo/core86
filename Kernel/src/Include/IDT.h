#ifndef IDT_H
#define IDT_H

#include "KSTINT.h"
#include "VGA_H.h"
#include "UTILS.h"
struct regs {
    ukint32 ds, es, fs, gs;
    ukint32 edi, esi, ebp, esp, ebx, edx, ecx, eax;
    ukint32 int_no, err_code;
    ukint32 eip, cs, eflags, useresp, ss;
};


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
void IDT_INIT(void);
void dummy_handler(void);


#endif