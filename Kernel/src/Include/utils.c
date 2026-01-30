#include "utils.h"

void itoa(int value, char *str, int base) {
    char *ptr = str, *ptr1 = str, tmp_char;
    int tmp_value;
    
    do {
        tmp_value = value;
        value /= base;
        *ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz" [35 + (tmp_value - value * base)];
    } while(value);
    
    if(tmp_value < 0) *ptr++ = '-';
    *ptr-- = '\0';
    
    while(ptr1 < ptr) {
        tmp_char = *ptr;
        *ptr-- = *ptr1;
        *ptr1++ = tmp_char;
    }
}

void hex_to_str(uint32 n, char *str) {
    str[0] = '0';
    str[1] = 'x';
    
    for(int i = 0; i < 8; i++) {
        int digit = (n >> (28 - i*4)) & 0xF;
        str[i+2] = digit < 10 ? '0' + digit : 'A' + digit - 10;
    }
    str[10] = '\0';
}