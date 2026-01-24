#ifndef DEBUG_H
#define DEBUG_H

#include "VGA_H.h"
#include "PS2KB.h"
#include "KSTINT.h"
#include "OUTB_H.h"
#include "PMM.h"
#include "UTILS.h"
#include "VMM.h"
#include "HEAP.h"

void test_heap(void); 
void test_address_space(void);
void test_real_memory(void);
void test_memory_content(void);
void debug_print(const char* str);
void check_page_tables(void);
void ultimate_test(void);
void verify_heap_clean(void);

#endif