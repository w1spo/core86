// ==================== CORE86 KERNEL INTERFACE ====================
//
// Copyright © Grzegorz Patryk Dytko, w1spo  
// Created 27/10/2025 | Revamped 31/10/2025
//
// CORE86 KERNEL - The Heart of Tomorrow's Legacy
// Advanced communication interface bridging hardware and software
// Supporting DOS compatibility with modern 32-bit architecture
//
// =================================================================



#include "Include/PS2KB.h"
#include "Include/VGA_H.h"
#include "Include/kstint.h" // dla ukint_32
#include "Include/OUTB_H.h"


void kernel_main(void) {
    vga_init();
    vga_print("Welcome to Core86\n");
    PS2_KB_INSTALL();

    while (1) {
        POLLING_HANDLER();
        if (PS2_KB_HAS_INPUT()) {
            char c = PS2_KB_GETC();
            vga_char(c);
        }
        
        for(int i = 0; i < 1000; i++) asm volatile("nop");
    }
}


//TODO FIX SHIFT KEYBOARD HANDLER