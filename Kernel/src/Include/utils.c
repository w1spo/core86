#include "kstint.h"
#include "UTILS.h"
#include "VGA_H.h"
void int_to_str(ukint32 num, char* buffer) {
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

void print_hex(ukint32 num) {
    char hex_chars[] = "0123456789ABCDEF";
    
    vga_print("0x");
    
    // Print each hex digit
    for(int i = 28; i >= 0; i -= 4) {
        ukint32 digit = (num >> i) & 0xF;
        vga_char(hex_chars[digit]);
    }
}