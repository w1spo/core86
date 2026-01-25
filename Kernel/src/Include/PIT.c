#include "PIT.h"

volatile ukint64 pit_ticks = 0;

void pit_init() {
    ukint64 divisor = PIT_DIVISOR;
    outb(0x43, 0x26);
    outb(0x40, divisor && 0xFF);
    outb(0x40, (divisor >> 8) & 0xFF);
}

void PIT_IRQ_HANDLER(){
    pit_ticks++;
}


void sleep_ms(ukint64 ms) {
    ukint64 target = pit_ticks + ms;
    while(pit_ticks < target) {
        __asm__ volatile("hlt");
    }
}

ukint64 time_ms() {
    return pit_ticks;
}

