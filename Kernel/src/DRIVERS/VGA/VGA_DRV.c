#include "VGA_H.h"
#include "kstint.h"
#include "OUTB_H.h"
// Usuń #include <string.h>

static ukint_16* const VGA_BUFFER = (ukint_16*) VGA_MEM;
static ukint_8 vga_color = 0;
static int cursorx = 0;
static int cursory = 0;
static int scroll_offset = 0;

// Bufor historii (okrągły)
static ukint_16 vga_history[VGA_HISTORY_SIZE * VGA_WIDTH];
static int history_start = 0;
static int history_count = 0;

/**
 * Prosta funkcja kopiowania dla VGA
 */
static void vga_copy_line(ukint_16* dest, const ukint_16* src) {
    for(int x = 0; x < VGA_WIDTH; x++) {
        dest[x] = src[x];
    }
}

/**
 * Zwraca indeks w buforze historii dla danej linii
 */
static int history_line_index(int line) {
    return (history_start + line) % VGA_HISTORY_SIZE;
}

/**
 * Dodaje linię do historii
 */
static void add_line_to_history(int y) {
    int target_idx;
    
    if (history_count < VGA_HISTORY_SIZE) {
        target_idx = history_count;
        history_count++;
    } else {
        target_idx = history_start;
        history_start = (history_start + 1) % VGA_HISTORY_SIZE;
    }
    
    // Kopiuj linię do historii (bez memcpy)
    ukint_16* dest = &vga_history[target_idx * VGA_WIDTH];
    const ukint_16* src = &VGA_BUFFER[y * VGA_WIDTH];
    vga_copy_line(dest, src);
}

/**
 * Przewija ekran w dół (jak teraz)
 */
void vga_scroll(void) {
    // Dodaj górną linię do historii przed scrollowaniem
    add_line_to_history(0);
    
    // Przesuń linie w górę
    for (int y = 1; y < VGA_HEIGHT; y++) {
        for (int x = 0; x < VGA_WIDTH; x++) {
            VGA_BUFFER[(y-1)*VGA_WIDTH + x] = VGA_BUFFER[y*VGA_WIDTH + x];
        }
    }
    
    // Wyczyść ostatnią linię
    for (int x = 0; x < VGA_WIDTH; x++) {
        VGA_BUFFER[(VGA_HEIGHT-1)*VGA_WIDTH + x] = vga_entry(' ', vga_color);
    }
    
    cursorx = 0;
    cursory = VGA_HEIGHT - 1;
    scroll_offset = 0;
    update_cursor();
}

/**
 * Przewija ekran w górę (do historii)
 */
void vga_scroll_up(void) {
    int max_offset = history_count - VGA_HEIGHT;
    if (max_offset < 0) max_offset = 0;
    
    if (scroll_offset < max_offset) {
        scroll_offset++;
        redraw_screen_from_history();
    }
}

/**
 * Przewija ekran w dół (do aktualnych danych)
 */
void vga_scroll_down(void) {
    if (scroll_offset > 0) {
        scroll_offset--;
        redraw_screen_from_history();
    }
}

/**
 * Przeładowuje ekran z historii
 */
void redraw_screen_from_history(void) {
    // Oblicz które linie historii wyświetlić
    int start_line = 0;
    
    if (history_count > VGA_HEIGHT) {
        start_line = history_count - VGA_HEIGHT - scroll_offset;
        if (start_line < 0) start_line = 0;
    }
    
    int lines_to_show = VGA_HEIGHT;
    if (history_count < VGA_HEIGHT) {
        lines_to_show = history_count;
        start_line = 0;
    }
    
    // Wypełnij ekran z historii
    for (int screen_y = 0; screen_y < lines_to_show; screen_y++) {
        int history_y = start_line + screen_y;
        if (history_y >= history_count) {
            // Wypełnij puste linie
            for (int x = 0; x < VGA_WIDTH; x++) {
                VGA_BUFFER[screen_y * VGA_WIDTH + x] = vga_entry(' ', vga_color);
            }
            continue;
        }
        
        int history_idx = history_line_index(history_y);
        ukint_16* src = &vga_history[history_idx * VGA_WIDTH];
        
        for (int x = 0; x < VGA_WIDTH; x++) {
            VGA_BUFFER[screen_y * VGA_WIDTH + x] = src[x];
        }
    }
    
    // Wypełnij pozostałe linie jeśli historia jest za krótka
    for (int screen_y = lines_to_show; screen_y < VGA_HEIGHT; screen_y++) {
        for (int x = 0; x < VGA_WIDTH; x++) {
            VGA_BUFFER[screen_y * VGA_WIDTH + x] = vga_entry(' ', vga_color);
        }
    }
    
    // Ustaw kursor
    if (scroll_offset == 0) {
        cursorx = 0;
        cursory = VGA_HEIGHT - 1;
    } else {
        cursorx = 0;
        cursory = 0;
    }
    
    update_cursor();
}

/**
 * Zwraca aktualny offset scrolla
 */
int vga_get_scroll_offset(void) {
    return scroll_offset;
}

/**
 * Ustawia offset scrolla
 */
void vga_set_scroll_offset(int offset) {
    if (offset < 0) offset = 0;
    int max_offset = history_count - VGA_HEIGHT;
    if (max_offset < 0) max_offset = 0;
    
    if (offset > max_offset) {
        offset = max_offset;
    }
    
    scroll_offset = offset;
    redraw_screen_from_history();
}

void vga_init(void) {
    vga_color = vga_entry_color(VGA_LIGHT_GREY, VGA_BLACK);
    history_start = 0;
    history_count = 0;
    scroll_offset = 0;
    vga_clear();
}

void vga_clear(void) {
    for (int y = 0; y < VGA_HEIGHT; y++) {
        for (int x = 0; x < VGA_WIDTH; x++) {
            VGA_BUFFER[y * VGA_WIDTH + x] = vga_entry(' ', vga_color);
        }
    }
    cursorx = cursory = 0;
    scroll_offset = 0;
    history_start = 0;
    history_count = 0;
    update_cursor();
}

int vga_get_history_count(void) {
    return history_count;
}

int vga_get_scroll_max(void) {
    int max = history_count - VGA_HEIGHT;
    return max > 0 ? max : 0;
}

// --- Zachowaj oryginalne funkcje ---
void update_cursor(void) {
    ukint_16 pos = cursory * VGA_WIDTH + cursorx;
    outb(0x3D4, 0x0F);
    outb(0x3D5, (ukint_8)(pos & 0xFF));
    outb(0x3D4, 0x0E);
    outb(0x3D5, (ukint_8)((pos >> 8) & 0xFF));
}

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

void vga_print(const char *str) {
    while (*str) {
        vga_char(*str);
        str++;
    }
}

void vga_colors(enum VGA_COLOUR fg, enum VGA_COLOUR bg) {
    vga_color = vga_entry_color(fg, bg);
}