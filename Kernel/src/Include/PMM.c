#include "PMM.h"
#include "config.h"
#include "print.h"
#include "types.h"
#include "utils.h"

static uint8* bitmap = 0;
static uint32 total_pages = 0;
static uint32 used_pages = 0;
static uint32 last_alloc_index = 4096;

void pmm_init(uint32 total_mem)
{
    total_pages = (total_mem * 1024 * 1024) / PAGE_SIZE;
    uint32 bitmap_size = total_pages;
    bitmap = (uint8*)BITMAP_START;

    for (uint32 i = 0; i < total_pages; i++)
    {
        bitmap[i] = 0;
    }

    uint32 kernel_pages = KERNEL_END / PAGE_SIZE;
    print("[INFO] [PMM] RESERVING ");
    char buf[12];
    itoa(kernel_pages, buf, 10);
    print(buf);
    print(" FOR KERNEL"); printc('\r'); printc('\n');

    for (uint32 i = 0; i < kernel_pages; i++)
    {
        bitmap[i] = 1;
    }
    
    used_pages = kernel_pages;

    printl("[INFO] [PMM] Ready");
    print("[INFO] [PMM] Used Pages: ");
    itoa(used_pages, buf, 10);
    print(buf);
    printc('\r'); printc('\n');

}


uint32 alloc_page(void) 
{
    uint32 start = last_alloc_index;
    uint32 i = start;
    
    for(i = start; i < total_pages; i++) 
    {
        if(bitmap[i] == 0) 
        {
            bitmap[i] = 1;
            used_pages++;
            last_alloc_index = i + 1;
            
            if(last_alloc_index >= total_pages) {
                last_alloc_index = 4096;
            }
            
            uint32 addr = i * PAGE_SIZE;
            kprintf("[INFO] [PMM] Allocated: i=%d, addr=0x%x, new_used=%d, new_last_idx=%d\r\n",
                    i, addr, used_pages, last_alloc_index);
            return addr;
        }

    }
    
    
    for(i = 4096; i < start; i++) 
    {
        if(bitmap[i] == 0) 
        {
            bitmap[i] = 1;
            used_pages++;
            last_alloc_index = i + 1;
            if(last_alloc_index >= total_pages) {
                last_alloc_index = 4096;
            }
            
            uint32 addr = i * PAGE_SIZE;
            return addr;
        }
    }

    printl("[ERROR] [PMM] OUT OF MEMORY - NO FREE PAGES");
    return 0;
}


void pmm_free_page(uint32 physical_addr)
{
    uint32 page_index = physical_addr / PAGE_SIZE;

    if(page_index < 4096)
    {
        printl("[ERROR] [PMM] CANNOT FREE KERNEL PAGES");
        return;
    }

    if(bitmap[page_index] == 1)
    {
        bitmap[page_index] = 0;
        used_pages--;
    }

    if (page_index < last_alloc_index)
    {
        last_alloc_index = page_index;
    }
}

void pmm_test()
{
    printl("[INFO] [PMM] PMM TEST");
    uint32 p1 = alloc_page();
    uint32 p2 = alloc_page();
    uint32 p3 = alloc_page();

    kprintf("[INFO] [PMM] Allocated: %x \r\n", p1);
    kprintf("[INFO] [PMM] Allocated: %x \r\n", p2);
    kprintf("[INFO] [PMM] Allocated: %x \r\n", p3);

    printl("[INFO] [PMM] Freeing Middle Page");
    kprintf("[INFO] [PMM] Freeing: %x \r\n", p2);
    pmm_free_page(p2);

    printl("[INFO] [PMM] Allocating Middle Page");
    uint32 p4 = alloc_page();
    kprintf("[INFO] [PMM] Allocated: %x \r\n", p4);
    kprintf("[INFO] [PMM] Should Be: %x \r\n", p2);
}