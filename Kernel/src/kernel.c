#include "Include/VGA_H.h"
#include "Include/kstint.h" // dla ukint_32

void int_to_str(ukint32 value, char* buffer) {
    char temp[12]; // max dla 32-bit liczby dziesiętnej + '\0'
    int i = 0;
    if (value == 0) {
        buffer[0] = '0';
        buffer[1] = '\0';
        return;
    }
    while (value > 0) {
        temp[i++] = '0' + (value % 10);
        value /= 10;
    }
    int j;
    for (j = 0; j < i; j++) {
        buffer[j] = temp[i - j - 1];
    }
    buffer[i] = '\0';
}

void kernel_main(void) {
    vga_init();
    vga_print("Hello World, Core86!\n");

    char buffer[32];
    for (int i = 0; i < 100; i++) {
        vga_print("New Line: ");
        int_to_str(i, buffer);
        vga_print(buffer);
        vga_print("\n");
    }
}
