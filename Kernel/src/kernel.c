// src/kernel.c
__asm__(".code16gcc\n");

void print(const char* str) {
    while (*str) {
        __asm__ volatile (
            "int $0x10"
            :
            : "a" ((0x0E << 8) | *str),
              "b" (0x0007)
        );
        str++;
    }
}

void _start() {
    print("KERNEL: 16-bit mode works!");
    
    while(1) {
        __asm__ volatile ("hlt");
    }
}