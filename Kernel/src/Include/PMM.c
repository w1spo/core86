#include "UTILS.h"
#include "kstint.h"
#include "PMM.h"
#include "VGA_H.h"

static ukint32 pmm_bitmap[PMM_BITMAP_SIZE];
static ukint32 pmm_total_pages = 0;
static ukint32 pmm_used_pages = 0;
static ukint32 pmm_bitmap_size = 0;

void PMM_INIT(ukint32 total_memory) {
    pmm_total_pages = total_memory / PAGE_SIZE;
    pmm_bitmap_size = (pmm_total_pages + 31) / 32;
    pmm_used_pages = 0;
    
    char buffer[12];
    vga_print("[PMM] Total memory: ");
    int_to_str((total_memory / 1024 / 1024), buffer);
    vga_print(buffer);
    vga_print("MB (");
    int_to_str(pmm_total_pages, buffer);
    vga_print(buffer);
    vga_print(" pages)\n");
    
    // 1. Wyczyść bitmapę (WSZYSTKO WOLNE)
    for(ukint32 i = 0; i < pmm_bitmap_size; i++) {
        pmm_bitmap[i] = 0x00000000;  // WSZYSTKO WOLNE na start!
    }
    
    // 2. Zarezerwuj pierwsze 16MB dla kernela
    ukint32 kernel_pages = (16 * 1024 * 1024) / PAGE_SIZE; // 4096 pages
    
    vga_print("[PMM] Reserving ");
    int_to_str(kernel_pages, buffer);
    vga_print(buffer);
    vga_print(" pages (16MB) for kernel\n");
    
    // Zarezerwuj strony 0-4095
    for(ukint32 i = 0; i < kernel_pages; i++) {
        ukint32 bitmap_index = i / 32;
        ukint32 bit_index = i % 32;
        pmm_bitmap[bitmap_index] |= (1 << bit_index);
    }
    
    pmm_used_pages = kernel_pages;  // Ustaw poprawną liczbę
    
    vga_print("[PMM] Ready - ");
    int_to_str(PMM_GET_FREE_PAGES(), buffer);
    vga_print(buffer);
    vga_print(" free pages available\n");
}

ukint32 PMM_ALLOC_PAGE(void) 
{
    // Szukaj od strony 4096 (16MB) - unikaj kernel space
    for (ukint32 i = 4096; i < pmm_total_pages; i++) {
        ukint32 bitmap_index = i / 32;
        ukint32 bit_index = i % 32;
        
        if(!(pmm_bitmap[bitmap_index] & (1 << bit_index))) {
            pmm_bitmap[bitmap_index] |= (1 << bit_index);
            pmm_used_pages++;
            return i * PAGE_SIZE;
        }
    }
    return 0; // Out of memory
}

void PMM_FREE_PAGE(ukint32 page)
{
    ukint32 page_index = page / PAGE_SIZE;
    
    // Nie zwalniaj kernel pages (0-4095)
    if(page_index < 4096) {
        return;
    }
    
    ukint32 bitmap_index = page_index / 32;
    ukint32 bit_index = page_index % 32;

    if (bitmap_index < pmm_bitmap_size) 
    {
        // Sprawdź czy strona była rzeczywiście używana
        if(pmm_bitmap[bitmap_index] & (1 << bit_index)) {
            pmm_bitmap[bitmap_index] &= ~(1 << bit_index);
            if(pmm_used_pages > 4096) {  // Zawsze zostaw 4096 dla kernela
                pmm_used_pages--;
            }
        }
    }
}

ukint32 PMM_GET_FREE_PAGES(void)
{
    if (pmm_total_pages > pmm_used_pages) {
        return pmm_total_pages - pmm_used_pages;
    } else {
        return 0;
    }
}

void PMM_PRINT_STATUS()
{
    char buffer[12];
    vga_print("\n[PMM] STATUS\n");
    
    vga_print("[PMM] Used Pages: ");
    int_to_str(pmm_used_pages, buffer);
    vga_print(buffer);
    vga_print("\n");
    
    vga_print("[PMM] Total Pages: ");
    int_to_str(pmm_total_pages, buffer);
    vga_print(buffer);
    vga_print("\n");
    
    vga_print("[PMM] Free Pages: ");
    int_to_str(PMM_GET_FREE_PAGES(), buffer);
    vga_print(buffer);
    vga_print("\n");
}