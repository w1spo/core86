#ifndef HEAP_H
#define HEAP_H

#include "stdh.h"

typedef struct block {
    uint32 size;        
    uint8 used;         
    struct block* next;
    struct block* prev;
} block_t;

static block_t* heap_start = 0;
static block_t* free_list = 0;
static uint32 heap_total = 0;
static uint32 heap_used = 0;
static uint32 heap_blocks = 0;

void heap_init(void);
void* kmalloc(uint32 size);
void kfree(void* ptr);
void heap_print_stats(void);

#endif