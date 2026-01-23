#include "Include/VGA_H.h"
#include "Include/PS2KB.h"
#include "Include/kstint.h"
#include "Include/OUTB_H.h"
#include "Include/PMM.h"
#include "Include/UTILS.h"
#include "Include/VMM.h"
#include "Include/HEAP.h"

// Function declarations
void test_heap(void); 
void test_address_space(void);
void test_real_memory(void);
void test_memory_content(void);
void debug_print(const char* str);
void check_page_tables(void);
void ultimate_test(void);
static void verify_heap_clean(void);

ukint32 detect_memory_simple(void) {
    return 512 * 1024 * 1024;  // 512MB
}

static void verify_heap_clean(void) {
    vga_print("\n[HEAP-VERIFY] Checking if heap is clean...\n");
    
    HEAP_PRINT_STATUS();
    
    void* test = KMALLOC(16);
    if(test) {
        vga_print("[HEAP-VERIFY] ✓ Can allocate on clean heap\n");
        KFREE(test);
    } else {
        vga_print("[HEAP-VERIFY] ✗ ERROR: Cannot allocate on clean heap!\n");
    }
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

void test_heap(void) {
    vga_print("\n[TEST] Testing heap allocation and free...\n");
    
    // Allocate 3 blocks
    void* ptr1 = KMALLOC(100);
    void* ptr2 = KMALLOC(200);
    void* ptr3 = KMALLOC(50);
    
    HEAP_PRINT_STATUS();
    HEAP_DEBUG_DUMP();
    
    // Free middle block
    vga_print("\n[TEST] Freeing middle block...\n");
    KFREE(ptr2);
    
    HEAP_PRINT_STATUS();
    HEAP_DEBUG_DUMP();
    
    // Allocate again
    vga_print("\n[TEST] Allocating 150 bytes...\n");
    void* ptr4 = KMALLOC(150);
    
    HEAP_PRINT_STATUS();
    HEAP_DEBUG_DUMP();
    
    // Cleanup
    if(ptr1) KFREE(ptr1);
    if(ptr3) KFREE(ptr3);
    if(ptr4) KFREE(ptr4);
    
    vga_print("[TEST] All blocks freed\n");
}

void test_real_memory(void) {
    vga_print("\n[TEST] Testing REAL memory access...\n");
    
    // Allocate memory
    int* numbers = (int*)KMALLOC(100 * sizeof(int));
    
    if(!numbers) {
        vga_print("[TEST] FAIL: Allocation failed!\n");
        return;
    }
    
    // Write data
    vga_print("[TEST] Writing data to allocated memory...\n");
    for(int i = 0; i < 100; i++) {
        numbers[i] = i * i;  // Write squares
    }
    
    // Read and verify
    vga_print("[TEST] Verifying data...\n");
    int errors = 0;
    for(int i = 0; i < 100; i++) {
        if(numbers[i] != i * i) {
            errors++;
        }
    }
    
    if(errors == 0) {
        vga_print("[TEST] PASS: Memory read/write works!\n");
    } else {
        char buf[12];
        vga_print("[TEST] FAIL: ");
        int_to_str(errors, buf);
        vga_print(buf);
        vga_print(" errors found!\n");
    }
    
    KFREE(numbers);
}

void test_address_space(void) {
    vga_print("\n[TEST] Checking address space...\n");
    
    void* ptr1 = KMALLOC(100);
    void* ptr2 = KMALLOC(100);
    
    if(ptr1 && ptr2) {
        char buf[12];
        
        vga_print("[TEST] ptr1 = 0x");
        vga_print(hex_to_str((ukint32)ptr1, buf));
        vga_print("\n[TEST] ptr2 = 0x");
        vga_print(hex_to_str((ukint32)ptr2, buf));
        vga_print("\n");
        
        // Check distance between pointers
        ukint32 distance = (ukint32)ptr2 - (ukint32)ptr1;
        vga_print("[TEST] Distance: ");
        int_to_str(distance, buf);
        vga_print(buf);
        vga_print(" bytes\n");
        
        // Should be > 100 (size + header)
        if(distance > 100) {
            vga_print("[TEST] PASS: Proper spacing between allocations\n");
        }
    }
    
    KFREE(ptr1);
    KFREE(ptr2);
}

void test_memory_content(void) {
    vga_print("\n[TEST] Checking memory initialization...\n");
    
    // Allocate and check content
    char* buffer = (char*)KMALLOC(256);
    
    if(buffer) {
        // Check if memory contains garbage or is zeroed
        int non_zero = 0;
        for(int i = 0; i < 256; i++) {
            if(buffer[i] != 0) {
                non_zero++;
            }
        }
        
        char buf[12];
        vga_print("[TEST] Non-zero bytes in fresh allocation: ");
        int_to_str(non_zero, buf);
        vga_print(buf);
        vga_print("\n");
        
        // Write data
        for(int i = 0; i < 256; i++) {
            buffer[i] = i % 256;
        }
        
        KFREE(buffer);
        
        // Reallocate - check if memory contains old data
        buffer = (char*)KMALLOC(256);
        if(buffer) {
            vga_print("[TEST] Memory reuse test\n");
            KFREE(buffer);
        }
    }
}

// Simple debug function via serial port
void debug_print(const char* str) {
    // Initialize serial port (optional)
    // For now just print to screen
    vga_print(str);
}

void check_page_tables(void) {
    vga_print("\n[DEBUG] Checking page tables...\n");
    
    // CR3 contains page directory address
    ukint32 cr3;
    __asm__ volatile ("mov %%cr3, %0" : "=r"(cr3));
    
    char buf[12];
    vga_print("[DEBUG] CR3 (Page Directory): 0x");
    vga_print(hex_to_str(cr3, buf));
    vga_print("\n");
    
    // Check mapping for heap address
    ukint32 heap_virt = 0x00200000;  // Your heap
    vga_print("[DEBUG] Heap virtual address: 0x");
    vga_print(hex_to_str(heap_virt, buf));
    vga_print("\n");
}

void ultimate_test(void) {
    vga_print("\n[ULTIMATE] Testing heap boundaries...\n");
    
    // Array to remember ALL allocations
    #define MAX_ALLOCS 512
    void* allocations[MAX_ALLOCS] = {0};
    int alloc_count = 0;
    ukint32 total_allocated = 0;
    void* last_alloc = 0;

    // --- TEST 1: Try to allocate too large block ---
    vga_print("[ULTIMATE] 1. Testing huge allocation...\n");
    void* huge = KMALLOC(HEAP_SIZE * 2);  // 2x larger than heap
    
    if(!huge) {
        vga_print("[ULTIMATE]   ✓ CORRECT: Huge allocation rejected\n");
    } else {
        vga_print("[ULTIMATE]   ✗ ERROR: Huge allocation should fail!\n");
        KFREE(huge);
    }
    
    // --- TEST 2: Fill entire heap ---
    vga_print("[ULTIMATE] 2. Filling entire heap...\n");
    
    while(1) {
        void* ptr = KMALLOC(4096);  // 4KB
        
        if(!ptr) {
            // Heap full
            char buf[12];
            vga_print("[ULTIMATE]   Heap full after ");
            int_to_str(total_allocated, buf);
            vga_print(buf);
            vga_print(" bytes (");
            int_to_str(alloc_count, buf);
            vga_print(buf);
            vga_print(" allocations)\n");
            break;
        }
        
        // Save pointer to array (if space)
        if(alloc_count < MAX_ALLOCS) {
            allocations[alloc_count] = ptr;
            alloc_count++;
        }
        
        // Write marker to check if memory works
        *(ukint32*)ptr = 0xDEADBEEF;
        
        total_allocated += 4096;
        last_alloc = ptr;
    }
    
    // Show heap status after filling
    HEAP_PRINT_STATUS();
    
    // --- TEST 3: Try to allocate when heap full ---
    vga_print("[ULTIMATE] 3. Testing allocation when heap full...\n");
    void* should_fail = KMALLOC(100);
    
    if(!should_fail) {
        vga_print("[ULTIMATE]   ✓ CORRECT: Allocation fails when heap full\n");
    } else {
        vga_print("[ULTIMATE]   ✗ ERROR: Should not allocate when heap full!\n");
        if(alloc_count < MAX_ALLOCS) {
            allocations[alloc_count] = should_fail;
            alloc_count++;
        }
    }
    
    // --- TEST 4: Free ONE and reallocate ---
    vga_print("[ULTIMATE] 4. Freeing one block and reallocating...\n");
    
    if(last_alloc) {
        // Check if marker is intact
        if(*(ukint32*)last_alloc == 0xDEADBEEF) {
            vga_print("[ULTIMATE]   ✓ Memory content preserved\n");
        }
        
        KFREE(last_alloc);
        
        // Remove from array (for cleanliness)
        for(int i = 0; i < alloc_count; i++) {
            if(allocations[i] == last_alloc) {
                allocations[i] = 0;
                break;
            }
        }
        
        // Try to allocate smaller block
        void* should_work = KMALLOC(100);
        if(should_work) {
            vga_print("[ULTIMATE]   ✓ Can allocate after free\n");
            
            // Save to array
            if(alloc_count < MAX_ALLOCS) {
                allocations[alloc_count] = should_work;
                alloc_count++;
            }
            
            // Check if memory is usable
            *(ukint32*)should_work = 0xCAFEBABE;
            if(*(ukint32*)should_work == 0xCAFEBABE) {
                vga_print("[ULTIMATE]   ✓ Freed memory is usable\n");
            }
        } else {
            vga_print("[ULTIMATE]   ✗ ERROR: Cannot allocate after free!\n");
        }
    }
    
    // --- TEST 5: Fragmentation ---
    vga_print("[ULTIMATE] 5. Testing fragmentation...\n");
    
    // Free every other block
    int freed_count = 0;
    for(int i = 0; i < alloc_count; i += 2) {
        if(allocations[i]) {
            KFREE(allocations[i]);
            allocations[i] = 0;
            freed_count++;
        }
    }
    
    char buf[12];
    vga_print("[ULTIMATE]   Freed ");
    int_to_str(freed_count, buf);
    vga_print(buf);
    vga_print(" blocks (every other)\n");
    
    HEAP_PRINT_STATUS();
    
    // Try to allocate medium block - should use merged fragments
    vga_print("[ULTIMATE]   Trying to allocate medium block (2048 bytes)...\n");
    void* medium = KMALLOC(2048);
    if(medium) {
        vga_print("[ULTIMATE]   ✓ Can allocate medium block after fragmentation\n");
        
        if(alloc_count < MAX_ALLOCS) {
            allocations[alloc_count] = medium;
            alloc_count++;
        }
    } else {
        vga_print("[ULTIMATE]   ✗ ERROR: Cannot allocate medium block!\n");
    }
    
    // --- MOST IMPORTANT: Clean up EVERYTHING ---
    vga_print("[ULTIMATE] 6. Cleaning up ALL allocations...\n");
    
    int cleaned = 0;
    for(int i = 0; i < alloc_count; i++) {
        if(allocations[i]) {
            KFREE(allocations[i]);
            allocations[i] = 0;
            cleaned++;
        }
    }
    
    vga_print("[ULTIMATE]   Freed ");
    int_to_str(cleaned, buf);
    vga_print(buf);
    vga_print(" blocks total\n");
    
    // --- TEST 7: Check if heap returned to initial state ---
    vga_print("[ULTIMATE] 7. Verifying heap is clean...\n");
    HEAP_PRINT_STATUS();
    
    // Final test: allocation on clean heap
    vga_print("[ULTIMATE]   Final allocation test on clean heap...\n");
    void* final_test = KMALLOC(128);
    
    if(final_test) {
        // Check if address is at START of heap
        ukint32 addr = (ukint32)final_test;
        if(addr >= HEAP_START && addr < HEAP_START + 1000) {
            vga_print("[ULTIMATE]   ✓ Allocation at START of heap: 0x");
            vga_print(hex_to_str(addr, buf));
            vga_print("\n");
        } else {
            vga_print("[ULTIMATE]   ⚠ Warning: Allocation not at heap start\n");
        }
        
        KFREE(final_test);
        vga_print("[ULTIMATE]   ✓ Cleanup successful\n");
    } else {
        vga_print("[ULTIMATE]   ✗ ERROR: Cannot allocate on clean heap!\n");
    }
    
    vga_print("[ULTIMATE] ✓ All tests completed!\n");
}