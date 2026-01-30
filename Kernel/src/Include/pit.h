#ifndef PIT_H
#define PIT_H

#include "types.h"
#include "io.h"

extern volatile uint32 system_ticks;

void init_pit(uint32 hz);
void pit_handler(void);
void ksleep(uint32 ms);
uint64 get_ticks(void);

#endif