#ifndef PMM_H
#define PMM_H

#include "types.h"
#include "config.h"
#include "print.h"

void pmm_init(uint32 total_mem);
uint32 alloc_page(void);
void pmm_free_page(uint32 physical_addr);
void pmm_status(void);
void pmm_test(void);

#endif