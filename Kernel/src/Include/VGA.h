#ifndef VGA
#define VGA

#include "stdh.h"


static inline uint8 vga_entry_color(enum VGA_COLOUR fg, enum VGA_COLOUR bg) {
    return fg | (bg << 4);
}

static inline uint16 vga_entry(unsigned char uc, uint8 color) {
    return (uint16) uc | ((uint16) color << 8);
}

void vga_init(void);
void vga_clear(void);
void vga_char(char c);
void vga_print(const char* str);
void vga_colors(enum VGA_COLOUR fg, enum VGA_COLOUR bg);
void vga_scroll_up(void);
void vga_scroll_down(void);
void update_cursor(void);
void redraw_screen_from_history(void);
void redraw_screen_from_history(void);
int vga_get_scroll_offset(void);
void vga_set_scroll_offset(int offset);
int vga_get_history_count(void);
int vga_get_scroll_max(void);

static void vga_memcpy(uint16* dest, const uint16* src, int count) {
    for(int i = 0; i < count; i++) {
        dest[i] = src[i];
    }
}

#endif
