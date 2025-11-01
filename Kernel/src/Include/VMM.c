#include "VMM.h"
#include "VGA_H.h"
#include "UTILS.h"

ukint32* page_directory = (ukint32*)PAGE_DIRECTORY_ADDRESS;
ukint32* first_page_table = (ukint32*)PAGE_TABLE_ADDRESS;

void VMM_INIT(void) {
    vga_print("[VMM] Step 1: Clearing page directory...\n");
    
    // Clear page directory
    for(ukint32 i = 0; i < 1024; i++) {
        page_directory[i] = PAGE_PRESENT | PAGE_WRITABLE;
    }
    vga_print("[VMM] Step 1: OK\n");
    
    vga_print("[VMM] Step 2: Mapping first 4MB...\n");
    // Identity map first 4MB
    for(ukint32 i = 0; i < 1024; i++) {
        first_page_table[i] = (i * PAGE_SIZE) | PAGE_PRESENT | PAGE_WRITABLE;
    }
    vga_print("[VMM] Step 2: OK\n");
    
    vga_print("[VMM] Step 3: Setting page directory entry...\n");
    // Set first page directory entry
    page_directory[0] = (ukint32)first_page_table | PAGE_PRESENT | PAGE_WRITABLE;
    vga_print("[VMM] Step 3: OK\n");
    
    vga_print("[VMM] Initialization complete\n");
}

void VMM_ENABLE_PAGING(void) {
    vga_print("[VMM] Step 4: Loading CR3...\n");
    
    // Load page directory to CR3
    asm volatile("mov %0, %%cr3" :: "r"((ukint32)page_directory));
    vga_print("[VMM] Step 4: OK - CR3 loaded\n");
    
    vga_print("[VMM] Step 5: Enabling paging bit...\n");
    ukint32 cr0;
    asm volatile("mov %%cr0, %0" : "=r"(cr0));
    vga_print("[VMM] CR0 before: 0x");
    print_hex(cr0);
    vga_print("\n");
    
    cr0 |= 0x80000000; // Enable paging
    
    asm volatile("mov %0, %%cr0" :: "r"(cr0));
    vga_print("[VMM] Step 5: OK - Paging enabled\n");
    
    // Verify
    asm volatile("mov %%cr0, %0" : "=r"(cr0));
    vga_print("[VMM] CR0 after: 0x");
    print_hex(cr0);
    vga_print("\n");
    
    vga_print("[VMM] Paging successfully enabled!\n");
}