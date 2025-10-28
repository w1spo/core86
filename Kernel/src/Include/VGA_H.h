#ifndef VGA_DRV
#define VGA_DRV

#include "kstint.h"
#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define VGA_MEM 0xB8000

enum VGA_COLOUR {
    VGA_BLACK = 0,
    VGA_BLUE = 1,
    VGA_GREEN = 2,
    VGA_CYAN = 3,
    VGA_RED = 4,
    VGA_MAGENTA = 5,
    VGA_BROWN = 6,
    VGA_LIGHT_GREY = 7,
    VGA_DARK_GREY = 8,
    VGA_LIGHT_BLUE = 9,
    VGA_LIGHT_GREEN = 10,
    VGA_LIGHT_CYAN = 11,
    VGA_LIGHT_RED = 12,
    VGA_LIGHT_MAGENTA = 13,
    VGA_LIGHT_BROWN = 14,
    VGA_WHITE = 15,
};

static inline ukint_8 vga_entry_color(enum VGA_COLOUR fg, enum VGA_COLOUR bg) {
    return fg | (bg << 4);
}

static inline ukint_16 vga_entry(unsigned char uc, ukint_8 color) {
    return (ukint_16) uc | ((ukint_16) color << 8);
}

void vga_init(void);
void vga_clear(void);
void vga_char(char c);
void vga_print(const char* str);
void vga_colors(enum VGA_COLOUR fg, enum VGA_COLOUR bg);

#endif
