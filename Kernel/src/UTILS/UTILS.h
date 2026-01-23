#ifndef UTILS_H
#define UTILS_H

#include "kstint.h"

void int_to_str(ukint32 num, char* buffer);
char* hex_to_str(ukint32 value, char* buffer); 
void print_hex(ukint32 num);
void* memcpy(void* dest, const void* src, ukint32 n);
void* memset(void* dest, int value, ukint32 n);

#endif