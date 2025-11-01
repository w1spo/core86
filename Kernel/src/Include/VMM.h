#ifndef VMM_H
#define VMM_H

#include "kstint.h"
#include "PMM.h"

// UŻYJ WYŻSZYCH ADRESÓW - POZA KERNEL ZONE!
#define PAGE_DIRECTORY_ADDRESS 0x1000000  // 16MB - koniec kernel zone
#define PAGE_TABLE_ADDRESS     0x1001000  // 16MB + 4KB

#define PAGE_PRESENT    (1 << 0)
#define PAGE_WRITABLE   (1 << 1)
#define PAGE_USER       (1 << 2)

void VMM_INIT(void);
void VMM_MAP_PAGE(ukint32 virtual_addr, ukint32 physical_addr, ukint32 flags);
void VMM_ENABLE_PAGING(void);

#endif