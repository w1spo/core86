#ifndef VMM_H
#define VMM_H

#include "stdh.h"
#include "PMM.h"

void vmm_init(void);
void vmm_map_page(uint32 virt, uint32 phys, uint32 vmm_flags);
void vmm_unmap_page(uint32 virt);
uint32 vmm_v_to_p(uint32 virt);
void vmm_enable(void);
void vmm_status(void);

#endif