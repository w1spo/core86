#ifndef HEAP_H
#define HEAP_H

#include "KSTINT.h"

#define HEAP_START     0x200000  // 2MB
#define HEAP_END       0x400000  // 4MB  
#define HEAP_SIZE      (HEAP_END - HEAP_START)
#define BLOCK_MAGIC    0xDEADBEEF

typedef struct Block {
    ukint32 magic;      // Magic number for validation
    ukint32 size;       // Size of allocated data
    ukint_8 used;        // 1 = used, 0 = free
    struct Block* next; // Next block in list
    struct Block* prev; // Previous block in list
} Block;

#define BLOCK_SIZE     sizeof(Block)
#define MIN_BLOCK_SIZE 16

void HEAP_INIT(void);
void* KMALLOC(ukint32 size);
void KFREE(void* ptr);
void HEAP_PRINT_STATUS(void);
void HEAP_DEBUG_DUMP(void);  // Debug function
void HEAP_PRINT_DETAILED(void);
#endif