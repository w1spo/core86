#include "Include/VGA_H.h"
#include "Include/PS2KB.h"
#include "Include/KSTINT.h"
#include "Include/OUTB_H.h"
#include "Include/PMM.h"
#include "Include/UTILS.h"
#include "Include/VMM.h"
#include "Include/HEAP.h"
#include "Include/DEBUG.h"
#include "PIT.h"
#include "PIC.h"
#include "IDT.h"

ukint32 detect_memory_simple(void) {
    return 512 * 1024 * 1024;  // 512MB
}


void kernel_main(void) {

    idt_init();
    pic_init();
    pic_unmask_irq(0);
    pic_unmask_irq(1);
    pit_init();
    asm volatile("sti");


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
    
    // 3. Run ultimate test (it should clean up after itself)
    ultimate_test();
    
    // 4. Verify heap is clean
    verify_heap_clean();
    
    // 5. Show memory status
    PMM_PRINT_STATUS();
    HEAP_PRINT_STATUS();
    
    // 6. Run memory tests
    test_address_space();
    test_real_memory();
    test_memory_content();
    
    // 7. Debug info
    debug_print("Core86 kernel started\n");
    check_page_tables();
    
    // 8. Initialize keyboard
    PS2_KB_INSTALL();
    
    vga_print("\n=== SYSTEM READY ===\n");
    vga_print("Keyboard ready. Start typing...\n> ");
    
    // 9. Quick allocation test
    vga_print("\n[TEST] Testing heap allocation...\n");
    void* test1 = KMALLOC(256);
    void* test2 = KMALLOC(512);
    void* test3 = KMALLOC(1024);
    
    if(test1 && test2 && test3) {
        char hex_buf[12];
        vga_print("Allocated: 0x");
        vga_print(hex_to_str((ukint32)test1, hex_buf));
        vga_print(", 0x");
        vga_print(hex_to_str((ukint32)test2, hex_buf));
        vga_print(", 0x");
        vga_print(hex_to_str((ukint32)test3, hex_buf));
        vga_print("\n");
    }
    
    HEAP_PRINT_STATUS();
    
    // 10. Run heap test
    test_heap();
    
    // 11. Show detailed heap info
    HEAP_PRINT_DETAILED();
    
    // Main loop
    while (1) {
        asm.volatile("hlt");
    }
}