#ifndef UTILS_H
#define UTILS_H

#include "KSTINT.h"

void int_to_str(uint32_t num, char* buffer);
char* hex_to_str(uint32_t value, char* buffer); 
void print_hex(uint32_t num);
void* memcpy(void* dest, const void* src, uint32_t n);
void* memset(void* dest, int value, uint32_t n);

#endif