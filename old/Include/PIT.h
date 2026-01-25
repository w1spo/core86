#ifndef PIT_H
#define PIT_H

#include "KSTINT.h"
#include "OUTB_H.h"
#include "PIC.h"

#define PIT_FREQ 1000
#define PIT_BASE 1193180
#define PIT_DIVISOR (PIT_BASE / PIT_FREQ)

void pit_init(void);
void PIT_IRQ_HANDLER(void);

void scheduler_tick(void);
void pit_send_eoi(void);
ukint64 time_ms(void);


#endif