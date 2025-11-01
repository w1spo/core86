#ifndef HEAP_H
#define HEAP_H

#include "kstint.h"

#define HEAP_START     0x200000  // 2MB
#define HEAP_END       0x400000  // 4MB  
#define HEAP_SIZE      (HEAP_END - HEAP_START)

void HEAP_INIT(void);
void* KMALLOC(ukint32 size);
void KFREE(void* ptr);
void HEAP_PRINT_STATUS(void);

#endif