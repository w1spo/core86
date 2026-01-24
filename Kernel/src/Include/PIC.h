#ifndef PIC_H
#define PIC_H

#include "KSTINT.h"
#include "OUTB_H.h"
void pic_init(void);
void pic_unmask_irq(ukint_8 irq);


#endif