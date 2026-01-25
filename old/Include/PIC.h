#ifndef PIC_H
#define PIC_H

#include "KSTINT.h"
#include "OUTB_H.h"
#include "VGA_H.h"
#include "IDT.h"

#define IRQ1 0x01
void pic_init(void);
void pic_unmask_irq(ukint_8 irq);
void pic_enable_irq(int irq);

#endif