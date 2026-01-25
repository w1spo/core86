#include "KSTINT.h"
#include "UTILS.h"
#include "VGA_H.h"

void int_to_str(uint32_t num, char* buffer) {
    char temp[12];
    int i = 0;
    
    if (num == 0) {
        buffer[0] = '0';
        buffer[1] = '\0';
        return;
    }
    
    // Konwertuj do tymczasowego buffera (odwrócona kolejność)
    while (num > 0) {
        temp[i++] = '0' + (num % 10);
        num /= 10;
    }
    
    // Odwróć kolejność
    for (int j = 0; j < i; j++) {
        buffer[j] = temp[i - j - 1];
    }
    buffer[i] = '\0';
}

void print_hex(uint32_t num) {
    char hex_chars[] = "0123456789ABCDEF";
    
    vga_print("0x");
    
    // Print each hex digit
    for(int i = 28; i >= 0; i -= 4) {
        uint32_t digit = (num >> i) & 0xF;
        vga_char(hex_chars[digit]);
    }
}

char* hex_to_str(uint32_t value, char* buffer) {
    const char* hex_digits = "0123456789ABCDEF";
    
    buffer[0] = '0';
    buffer[1] = 'x';
    
    for(int i = 0; i < 8; i++) {
        buffer[9 - i] = hex_digits[value & 0xF];
        value >>= 4;
    }
    buffer[10] = '\0';
    
    return buffer;
}

void* memcpy(void* dest, const void* src, uint32_t n) {
    char* d = (char*)dest;
    const char* s = (const char*)src;
    
    for(uint32_t i = 0; i < n; i++) {
        d[i] = s[i];
    }
    
    return dest;
}

void* memset(void* dest, int value, uint32_t n) {
    char* d = (char*)dest;
    
    for(uint32_t i = 0; i < n; i++) {
        d[i] = (char)value;
    }
    
    return dest;
}