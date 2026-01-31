#include "kernel.h"
#include "Include/PMM.h"
#include "Include/idt.h"
#include "Include/pic.h"
#include "Include/pit.h"
#include "Include/print.h"
#include "Include/serial.h"
#include "Include/config.h"
#include "Include/VMM.h"
#include "Include/HEAP.h"
#include "Include/VGA.h"

void test_vmm_heap(void);

void k_init()
{
    serial_init(COM1_PORT);
    vga_init();
    vga_colors(VGA_LIGHT_GREY, VGA_BLACK);
    vga_print("[INFO] [VGA] VGA READY");

    serial_write_line(COM1_PORT, "[INFO] SERIAL READY");
    serial_write_line(COM1_PORT, "[INFO] GDT FLUSHED");
    idt_init();
    serial_write_line(COM1_PORT, "[INFO] Enabling PIC");
    remap_pic();
    serial_write_line(COM1_PORT, "[INFO] Enabling PIT");
    init_pit(1000);
    serial_write_line(COM1_PORT, "[INFO] Enabling Interrupts");
    enable_sti();
    serial_write_line(COM1_PORT, "[INFO] Enabled STI");
    pmm_init(512);
    pmm_test();
    vmm_init();
    vmm_enable();
    //heap_init();
    //test_vmm_heap();
    serial_write_line(COM1_PORT, "[INFO] Sleep Test");
    printl("[INFO] Sleep Test");
    ksleep(1000);
    printl("[INFO] Awake!");
    serial_write_line(COM1_PORT, "[INFO] Awake!");
    serial_write_line(COM1_PORT, "");
    serial_write_line(COM1_PORT, KERNEL_NAME);
    serial_write_line(COM1_PORT, KERNEL_VER);
    serial_write_line(COM1_PORT, BUILD_NO);
    printc('\r');
    printc('\n');
    printl(KERNEL_NAME);
    printl(KERNEL_VER);
    printl(BUILD_NO);


    while(1) {
        asm volatile("hlt");
    }
    int x = 1;
    int y = 0;
    int z = x / y;
}

void test_vmm_heap(void) {
    printl("\r\n=== HEAP TEST ===");

    heap_init();
    
    // 4. Test alokacji
    kprintf("\r\n[TEST] Allocating 256 bytes...\r\n");
    uint32* ptr1 = (uint32*)kmalloc(256);
    if(ptr1) {
        *ptr1 = 0xDEADBEEF;
        kprintf("[TEST] Wrote %x to %x\r\n", *ptr1, ptr1);
    }
    
    kprintf("\r\n[TEST] Allocating 512 bytes...\r\n");
    uint32* ptr2 = (uint32*)kmalloc(512);
    if(ptr2) {
        *ptr2 = 0xCAFEBABE;
        kprintf("[TEST] Wrote %x to %x\r\n", *ptr2, ptr2);
    }
    
    kprintf("\r\n[TEST] Allocating 1024 bytes...\r\n");
    uint32* ptr3 = (uint32*)kmalloc(1024);
    if(ptr3) {
        *ptr3 = 0x12345678;
        kprintf("[TEST] Wrote %x to %x\r\n", *ptr3, ptr3);
    }
    
    heap_print_stats();
    
    // 5. Test zwalniania
    kprintf("\r\n[TEST] Freeing middle block...\r\n");
    kfree(ptr2);
    
    heap_print_stats();
    
    // 6. Test ponownej alokacji (powinna użyć zwolnionego bloku)
    kprintf("\r\n[TEST] Re-allocating 512 bytes...\r\n");
    uint32* ptr4 = (uint32*)kmalloc(512);
    if(ptr4 == ptr2) {
        kprintf("[TEST] SUCCESS: Reused freed block!\r\n");
    } else {
        kprintf("[TEST] FAIL: Got different block %x != %x\r\n", 
                ptr4, ptr2);
    }
    
    heap_print_stats();
    
    // 7. Test VMM mapping
    kprintf("\r\n[TEST] Testing VMM mapping...\r\n");
    uint32 test_phys = alloc_page();
    vmm_map_page(0xA00000, test_phys, PG_PRESENT | PG_WRITABLE);
    
    uint32* test_virt = (uint32*)0xA00000;
    *test_virt = 0x12345678;
    
    uint32 translated = vmm_v_to_p(0xA00000);
    kprintf("[TEST] 0xA00000 -> %x (should be %x)\r\n", 
            translated, test_phys);
    
    if(translated == test_phys) {
        kprintf("[TEST] VMM translation OK!\r\n");
    }
    
    printl("=== TEST COMPLETE ===");
}