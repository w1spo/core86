#include "Include/VGA_H.h"

void kernel_main(void) {
    vga_init();
    vga_print("Hello World, Core86!");
}