#include "pit.h"
#include "serial.h"
#include "io.h"
#include "utils.h"

volatile uint32 system_ticks = 0;



void init_pit(uint32 hz)  
{
    uint32 divisor = 1193182 / hz;  
    
    serial_write_line(COM1_PORT, "[PIT] Setting frequency: ");
    char buf[20];
    itoa(hz, buf, 10);
    serial_write_line(COM1_PORT, buf);
    serial_write_line(COM1_PORT, " Hz");
    
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
    uint32 target = system_ticks + ms;  
    while(target > system_ticks)
    {
        asm volatile("hlt");
    } 
}

__attribute__((naked)) 
void pit_handler(void) {
    __asm__ volatile(
        "pusha\n"
        "push %%ds\n"      
        "push %%es\n"
        "push %%fs\n"
        "push %%gs\n"
        
        "mov $0x10, %%ax\n"
        "mov %%ax, %%ds\n"
        "mov %%ax, %%es\n"
        "mov %%ax, %%fs\n"
        "mov %%ax, %%gs\n"
        
        
        "movl $system_ticks, %%ecx\n"
        "incl (%%ecx)\n"
        
        
        "movb $0x20, %%al\n"
        "outb %%al, $0x20\n"
        
        
        "pop %%gs\n"
        "pop %%fs\n"
        "pop %%es\n"
        "pop %%ds\n"
        "popa\n"
        
        "iret\n"
        : : : "memory"
    );
}