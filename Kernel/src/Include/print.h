#ifndef PRINT_H
#define PRINT_H

#include "serial.h"
#include "types.h"
#include "config.h"

typedef __builtin_va_list va_list;
#define va_start(ap, last) __builtin_va_start(ap, last)
#define va_arg(ap, type)   __builtin_va_arg(ap, type)
#define va_end(ap)         __builtin_va_end(ap)
#define va_copy(dest, src) __builtin_va_copy(dest, src)

void kprintf(const char* format, ...);
void print(string);
void printc(char charact);
void printl(string);
void printh(uint32 hex);

#endif