#ifndef PMM_H
#define PMM_H

#include "kstint.h"

#define PAGE_SIZE 4096
#define PMM_BITMAP_SIZE 32768

void PMM_INIT();
ukint32 PMM_ALLOC_PAGE(void);
void PMM_FREE_PAGE(ukint32 page);
void PMM_FREE_SIZE(void);
ukint32 PMM_GET_FREE_PAGES(void);
void PMM_PRINT_STATUS(void);

#endif