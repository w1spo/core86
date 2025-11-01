#include "HEAP.h"
#include "VGA_H.h"
#include "UTILS.h"

static ukint32 heap_pointer = HEAP_START;

void HEAP_INIT(void) {
    heap_pointer = HEAP_START;
    vga_print("[HEAP] Initialized at 0x200000 (2MB)\n");
}

void* KMALLOC(ukint32 size) {
    // Simple bump allocator - no free for now
    size = (size + 15) & ~15;  // Align to 16 bytes
    
    if(heap_pointer + size > HEAP_END) {
        vga_print("[HEAP] OUT OF MEMORY!\n");
        return 0;
    }
    
    void* ptr = (void*)heap_pointer;
    heap_pointer += size;
    
    return ptr;
}

void KFREE(void* ptr) {
    // For now - do nothing (we'll implement proper free later)
}

void HEAP_PRINT_STATUS(void) {
    ukint32 used = heap_pointer - HEAP_START;
    ukint32 free = HEAP_SIZE - used;
    
    char buffer[12];
    vga_print("\n[HEAP] STATUS\n");
    vga_print("[HEAP] Start: 0x200000\n");
    vga_print("[HEAP] End: 0x400000\n");
    vga_print("[HEAP] Used: ");
    int_to_str(used, buffer);
    vga_print(buffer);
    vga_print(" bytes\n");
    vga_print("[HEAP] Free: ");
    int_to_str(free, buffer);
    vga_print(buffer);
    vga_print(" bytes\n");
}