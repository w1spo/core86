#include "Include/VGA_H.h"
#include "Include/PS2KB.h"
#include "Include/KSTINT.h"
#include "Include/OUTB_H.h"
#include "Include/PMM.h"
#include "Include/UTILS.h"
#include "Include/VMM.h"
#include "Include/HEAP.h"
#include "Include/DEBUG.h"

#include "Include/PIT.h"
#include "Include/PIC.h"
#include "Include/IDT.h"

ukint32 detect_memory_simple(void) {
    return 512 * 1024 * 1024;  // 512MB
}

void check_gdt_selectors(void) {
    ukint_16 cs, ds, es, fs, gs, ss;
    
    asm volatile("mov %%cs, %0" : "=r"(cs));
    asm volatile("mov %%ds, %0" : "=r"(ds));
    asm volatile("mov %%es, %0" : "=r"(es));
    asm volatile("mov %%fs, %0" : "=r"(fs));
    asm volatile("mov %%gs, %0" : "=r"(gs));
    asm volatile("mov %%ss, %0" : "=r"(ss));
    
    vga_print("\n[GDT] Current selectors:\n");
    vga_print("CS: 0x"); print_hex(cs); vga_print("\n");
    vga_print("DS: 0x"); print_hex(ds); vga_print("\n");
    vga_print("ES: 0x"); print_hex(es); vga_print("\n");
    vga_print("FS: 0x"); print_hex(fs); vga_print("\n");
    vga_print("GS: 0x"); print_hex(gs); vga_print("\n");
    vga_print("SS: 0x"); print_hex(ss); vga_print("\n");
}

// W kernel_main:
void kernel_main(void) {
    vga_init();
    vga_print("Core86 - IRQ MODE\n\n");
    void __attribute__((cdecl)) ps2_irq_handler_asm_wrapper(void);
    __asm__ volatile("cli");
    
    // INICJALIZUJ IDT i PIC!

    
    // Inicjalizacja pamięci
    ukint32 total_memory = detect_memory_simple();
    PMM_INIT(total_memory);
    VMM_INIT();
    HEAP_INIT();
    VMM_ENABLE_PAGING();
    
    IDT_INIT();     // ODKOMENTUJ
    pic_init();     // ODKOMENTUJ
    void __attribute__((cdecl)) ps2_irq_handler_asm_wrapper(void);
    // Klawiatura z IRQ
    PS2_KB_INSTALL();  // To powinno ustawić IRQ handler
    void __attribute__((cdecl)) ps2_irq_handler_asm_wrapper(void);
    // Włącz przerwania
    __asm__ volatile("sti");
    
    vga_print("\n=== IRQ MODE ===\n");
    vga_print("Using PS2_IRQ_HANDLER()\n");
    vga_print("Type something: ");
    
    // Główna pętla - NIE używaj POLLING_HANDLER!
    while(1) {
        // Tylko czekaj na przerwania
        //__asm__ volatile("hlt");  // HLT czeka na przerwanie
        
        // Jeśli potrzebujesz coś robić w tle:
        // check_for_background_tasks();
    }
}