#ifndef IDT_H
#define IDT_H

#include "types.h"
#include "serial.h"

typedef struct {
    uint32 ds;
    uint32 edi, esi, ebp, esp, ebx, edx, ecx, eax;
    uint32 int_no, err_code;
    uint32 eip, cs, eflags, useresp, ss;
} registers_t;

struct idt_entry 
{
    uint16 offset_low;
    uint16 selector;
    uint8 zero;
    uint8 type;
    uint16 offset_high;

} __attribute__((packed));

struct idt_ptr {
    uint16 limit;
    uint32 base;
} __attribute__((packed));


extern struct idt_entry idt[256];

void dummy_isr(void);
void divide_by_zero(void);
void idt_init(void);
void idt_load(void);
void enable_sti(void);
void idt_set(int num, void *handler);

#endif