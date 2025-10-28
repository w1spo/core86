// BIOS VGA Driver
// Copyright © Grzegorz Patryk Dytko, w1spo
// Created 27/10/2025
//
// Provides basic text-mode VGA output functions for the Core86 Kernel.
// Supports 80x25 text mode via direct memory access to 0xB8000.

#include "VGA_H.h"
#include "kstint.h"
#include "OUTB_H.h"

static ukint_16* const VGA_BUFFER = (ukint_16*) VGA_MEM; // VGA text buffer base address
static ukint_8 vga_color = 0;  // Current VGA color attribute
static int cursorx = 0;        // BIOS cursor X position
static int cursory = 0;        // BIOS cursor Y position


/**
 * Updates the hardware cursor via BIOS ports 0x3D4/0x3D5.
 */
void update_cursor(void) {
    ukint_16 pos = cursory * VGA_WIDTH + cursorx;

    outb(0x3D4, 0x0F);
    outb(0x3D5, (ukint_8)(pos & 0xFF));
    outb(0x3D4, 0x0E);
    outb(0x3D5, (ukint_8)((pos >> 8) & 0xFF));
}

/**
 * Scrolls the screen up by one line if we reach the bottom.
 */
void vga_scroll(void) {
    for (int y = 1; y < VGA_HEIGHT; y++) {
        for (int x = 0; x < VGA_WIDTH; x++) {
            VGA_BUFFER[(y-1)*VGA_WIDTH + x] = VGA_BUFFER[y*VGA_WIDTH + x];
        }
    }
    // Clear last line
    for (int x = 0; x < VGA_WIDTH; x++) {
        VGA_BUFFER[(VGA_HEIGHT-1)*VGA_WIDTH + x] = vga_entry(' ', vga_color);
    }
    cursory = VGA_HEIGHT - 1;
}

// --- Core functions ---

/**
 * Initializes VGA output: sets default colors and clears the screen.
 */
void vga_init(void) {
    vga_color = vga_entry_color(VGA_LIGHT_GREY, VGA_BLACK);
    vga_clear();
}

/**
 * Clears the entire VGA text buffer and resets the cursor.
 */
void vga_clear(void) {
    for (int y = 0; y < VGA_HEIGHT; y++) {
        for (int x = 0; x < VGA_WIDTH; x++) {
            VGA_BUFFER[y * VGA_WIDTH + x] = vga_entry(' ', vga_color);
        }
    }
    cursorx = cursory = 0;
    update_cursor();
}

/**
 * Prints a single character to the screen, handling special chars and scrolling.
 */
void vga_char(char c) {
    if (c == '\n') {
        cursorx = 0;
        cursory++;
    } else if (c == '\r') {
        cursorx = 0;
    } else if (c == '\b') {
        if (cursorx > 0) {
            cursorx--;
            VGA_BUFFER[cursory * VGA_WIDTH + cursorx] = vga_entry(' ', vga_color);
        } else if (cursory > 0) {
            cursory--;
            cursorx = VGA_WIDTH - 1;
            VGA_BUFFER[cursory * VGA_WIDTH + cursorx] = vga_entry(' ', vga_color);
        }
    } else {
        const int index = cursory * VGA_WIDTH + cursorx;
        VGA_BUFFER[index] = vga_entry(c, vga_color);
        cursorx++;
        if (cursorx >= VGA_WIDTH) {
            cursorx = 0;
            cursory++;
        }
    }

    if (cursory >= VGA_HEIGHT) {
        vga_scroll();
    }

    update_cursor();
}

/**
 * Prints a null-terminated string to the screen.
 */
void vga_print(const char *str) {
    while (*str) {
        vga_char(*str);
        str++;
    }
}

/**
 * Changes current text color for subsequent characters.
 */
void vga_colors(enum VGA_COLOUR fg, enum VGA_COLOUR bg){
    vga_color = vga_entry_color(fg, bg);
}
