#include "VMM.h"
#include "config.h"
#include "print.h"

static uint32* page_dir = (uint32*)VMM_PD_ADDR;
static uint32 total_mapped = 0;

void vmm_init()
{
    for (int i = 0; i < 1024; i++)
    {
        page_dir[i] = PG_WRITABLE;
    }

    uint32 pt_phys = alloc_page();

    if (!pt_phys)
    {
        kprintf("%s %s No Memory for page", LOG_ERROR, LOG_VMM);
        return;
    }
    uint32* first_pt = (uint32*)pt_phys;

    for (int i = 0; i < 1024; i++)
    {
        first_pt[i] = (i * PAGE_SIZE) | PG_PRESENT | PG_WRITABLE;
    }

    page_dir[0] = pt_phys | PG_PRESENT | PG_WRITABLE;
    page_dir[768] = pt_phys | PG_PRESENT | PG_WRITABLE;
    total_mapped = 1024;
    kprintf("%s %s Identity mapped first 4MB\r\n", LOG_INFO, LOG_VMM);
    kprintf("%s %s Page table at phys: %x\r\n", LOG_INFO, LOG_VMM, pt_phys);
}

void vmm_map_page(uint32 virt, uint32 phys, uint32 flags) {

    uint32 pd_index = (virt >> 22) & 0x3FF;
    uint32 pt_index = (virt >> 12) & 0x3FF;
    
    kprintf("%s %s Mapping: %x -> %x (PD:%d, PT:%d)\r\n",
            LOG_INFO, LOG_VMM, virt, phys, pd_index, pt_index);
    
    if(!(page_dir[pd_index] & PG_PRESENT)) {
        uint32 new_pt_phys = alloc_page();
        if(!new_pt_phys) {
            kprintf("%s %s Cannot allocate page table!\r\n", 
                    LOG_ERROR, LOG_VMM);
            return;
        }
        
        uint32* new_pt = (uint32*)new_pt_phys;
        
        for(int i = 0; i < 1024; i++) {
            new_pt[i] = 0;
        }
        
        page_dir[pd_index] = new_pt_phys | PG_PRESENT | PG_WRITABLE;
        
        kprintf("%s %s Created new page table at PD[%d]\r\n", 
                LOG_INFO, LOG_VMM, pd_index);
    }
    
    uint32 pt_phys = page_dir[pd_index] & ~0xFFF;
    uint32* page_table = (uint32*)pt_phys;
    
    if(page_table[pt_index] & PG_PRESENT) {
        kprintf("%s %s Page %x already mapped!\r\n",
                LOG_WARN, LOG_VMM, virt);
        return;
    }
    
    page_table[pt_index] = phys | flags;
    total_mapped++;
    
    asm volatile("invlpg (%0)" : : "r"(virt));
    
    kprintf("%s %s Mapped successfully\r\n", LOG_INFO, LOG_VMM);
    uint32* test = (uint32*)virt;
    kprintf("%s %s Testing write to 0x%x...\r\n", LOG_DEBUG, LOG_VMM, virt);
    *test = 0xDEADBEEF;
    kprintf("%s %s Write test passed\r\n", LOG_DEBUG, LOG_VMM);
}

void vmm_unmap_page(uint32 virt) {
    uint32 pd_index = (virt >> 22) & 0x3FF;
    uint32 pt_index = (virt >> 12) & 0x3FF;
    
    if(!(page_dir[pd_index] & PG_PRESENT)) {
        kprintf("%s %s Page %x not mapped\r\n",
                LOG_WARN, LOG_VMM, virt);
        return;
    }
    
    uint32 pt_phys = page_dir[pd_index] & ~0xFFF;
    uint32* page_table = (uint32*)pt_phys;
    
    if(!(page_table[pt_index] & PG_PRESENT)) {
        kprintf("%s %s Page %x already unmapped\r\n",
                LOG_WARN, LOG_VMM, virt);
        return;
    }
    
    uint32 phys_addr = page_table[pt_index] & ~0xFFF;
    
    page_table[pt_index] = 0;
    total_mapped--;
    
    asm volatile("invlpg (%0)" : : "r"(virt));
    
    kprintf("%s %s Unmapped: %x\r\n", LOG_INFO, LOG_VMM, virt);
}

uint32 vmm_v_to_p(uint32 virt) {
    uint32 pd_index = (virt >> 22) & 0x3FF;
    uint32 pt_index = (virt >> 12) & 0x3FF;
    
    if(!(page_dir[pd_index] & PG_PRESENT)) {
        return 0;
    }
    
    uint32 pt_phys = page_dir[pd_index] & ~0xFFF;
    uint32* page_table = (uint32*)pt_phys;
    
    if(!(page_table[pt_index] & PG_PRESENT)) {
        return 0;
    }
    
    uint32 phys_base = page_table[pt_index] & ~0xFFF;
    uint32 offset = virt & 0xFFF;
    
    return phys_base + offset;
}

void vmm_enable(void) {
    kprintf("%s %s Enabling paging...\r\n", LOG_INFO, LOG_VMM);
    
    asm volatile("mov %0, %%cr3" : : "r"((uint32)page_dir));

    uint32 cr0;
    asm volatile("mov %%cr0, %0" : "=r"(cr0));
    cr0 |= 0x80000000;
    asm volatile("mov %0, %%cr0" : : "r"(cr0));
    
    kprintf("%s %s Paging enabled!\r\n", LOG_INFO, LOG_VMM);
}

void vmm_print_stats(void) {
    kprintf("%s %s Stats: %d pages mapped\r\n", 
            LOG_INFO, LOG_VMM, total_mapped);
}