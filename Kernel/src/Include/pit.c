#include "pit.h"
#include "serial.h"
#include "io.h"
#include "utils.h"

volatile uint32 system_ticks = 0;

void init_pit(uint32 hz)  /* Zmień nazwę parametru! */
{
    uint32 divisor = 1193182 / hz;  /* Użyj hz, nie freq! */
    
    serial_write_line(0x3F8, "[PIT] Setting frequency: ");
    char buf[20];
    itoa(hz, buf, 10);
    serial_write_line(0x3F8, buf);
    serial_write_line(0x3F8, " Hz");
    
    outb(0x43, 0x36);
    outb(0x40, divisor & 0xFF);
    outb(0x40, (divisor >> 8) & 0xFF);
}

uint64 get_ticks(void)
{
    return system_ticks;
}

void ksleep(uint32 ms)
{
    uint32 target = system_ticks + ms;  /* Użyj uint32! */
    while(target > system_ticks)
    {
        asm volatile("hlt");
    } 
}

void pit_handler(void) 
{
    static uint32 counter = 0;
    static uint8 first_three_done = 0;
    
    /* 1. INKREMENT - atomic? */
    uint32 current = system_ticks + 1;
    system_ticks = current;
    
    /* 2. EOI */
    outb(0x20, 0x20);
    
    /* 3. DEBUG - tylko pierwsze 3 RAZ */
    if(current <= 3 && !first_three_done) {
        serial_write_line(0x3F8, "[PIT] Tick ");
        char buf[20];
        itoa(current, buf, 10);
        serial_write_line(0x3F8, buf);
        
        if(current == 3) first_three_done = 1;
    }
    
    /* 4. 100ms counter */
    counter++;
    if(counter >= 100) {
        counter = 0;
        serial_write_line(0x3F8, "[PIT] 100ms");
    }
}