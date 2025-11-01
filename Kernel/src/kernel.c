#include "Include/VGA_H.h"
#include "Include/PS2KB.h"
#include "Include/kstint.h"
#include "Include/OUTB_H.h"
#include "Include/PMM.h"
#include "Include/UTILS.h"
#include "Include/VMM.h"
#include "Include/HEAP.h"

ukint32 detect_memory_simple(void) {
    return 512 * 1024 * 1024;  // 512MB
}

void kernel_main(void) {
    vga_init();
    vga_print("Welcome to Core86\n");
    
    // 1. Initialize memory management
    ukint32 total_memory = detect_memory_simple();
    vga_print("[SYSTEM] Memory: 512MB\n");
    
    PMM_INIT(total_memory);
    VMM_INIT();
    HEAP_INIT();
    
    // 2. Enable paging
    VMM_ENABLE_PAGING();
    
    // 3. Show status
    PMM_PRINT_STATUS();
    HEAP_PRINT_STATUS();
    
    // 4. Initialize keyboard
    PS2_KB_INSTALL();
    
    vga_print("\n=== SYSTEM READY ===\n");
    vga_print("Keyboard ready. Start typing...\n> ");
    
    // 5. Test memory allocation
    vga_print("\n[TEST] Testing heap allocation...\n");
    void* test1 = KMALLOC(256);
    void* test2 = KMALLOC(512);
    void* test3 = KMALLOC(1024);
    
    vga_print("Allocated: 0x");
    print_hex((ukint32)test1);
    vga_print(", 0x");
    print_hex((ukint32)test2);
    vga_print(", 0x");
    print_hex((ukint32)test3);
    vga_print("\n");
    
    HEAP_PRINT_STATUS();
    
    // Main loop
    while (1) {
        if (inb(PS2_STATUS_PORT) & OUTPUT_BUFFER_FULL) {
            PS2_KB_HANDLER();
        }
        
        if (PS2_KB_HAS_INPUT()) {
            char c = PS2_KB_GETC();
            vga_char(c);
        }
        
        for(int i = 0; i < 1000; i++) asm volatile("nop");
    }
}