#include "PMM.h"
#include "config.h"
#include "print.h"
#include "types.h"
#include "utils.h"

static uint8* bitmap = 0;
static uint32 total_pages = 0;
static uint32 used_pages = 0;


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
    print("Used Pages: ");
    itoa(used_pages, buf, 10);
    print(buf);
    printc('\r'); printc('\n');

}

uint32 alloc_page(void) 
{
    for(uint32 i = 4096; i < total_pages; i++)
    {
        if(bitmap[i] == 0)
        {
            bitmap[i] = 1;
            used_pages++;
            return used_pages;
        }
    }

    printl("[ERROR] [PMM] NO FREE PAGES");
    return 0;
}

void pmm_free_page(uint32 physical_addr)
{
    uint32 page_index = physical_addr / PAGE_SIZE;

    if(physical_addr < 4096)
    {
        printl("[ERROR] [PMM] CANNOT FREE KERNEL PAGES");
        return;
    }

    if(bitmap[page_index] == 1)
    {
        bitmap[page_index] = 0;
        used_pages--;
    }
}

void pmm_test()
{
    printl("[INFO] [PMM] PMM TEST");
    uint32 p1 = alloc_page();
    uint32 p2 = alloc_page();
    uint32 p3 = alloc_page();

    kprintf("Allocated: %h \n", p1);
    kprintf("Allocated: %h \n", p2);
    kprintf("Allocated: %h \n", p3);

    printl("[INFO] [PMM] Freeing Middle Page");
    kprintf("Freeing: %h \n", p2);
    pmm_free_page(p2);

    printl("[INFO] [PMM] Allocating Middle Page");
    uint32 p4 = alloc_page();
    kprintf("Allocated: %h \n", p4);
    kprintf("Should Be: %h \n", p2);

}