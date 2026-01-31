#ifndef CONFIG_H
#define CONFIG_H

//KERNEL CONFIG

#define KERNEL_NAME "CoreNT"
#define KERNEL_VER "0.0.1"
#define BUILD_NO "26A1"

//PHYSICAL ADDR

#define PHYS_MEM_BASE  0x00000000
#define BIOS_DATA      0x00000400
#define VGA_TEXT       0x000B8000
#define EBDA_START     0x00080000
#define KERNEL_LOAD    0x00100000
#define KERNEL_END     0x01000000
#define HEAP_START     0x01000000
#define STACK_TOP      0xC0000000

//SERIAL

#define COM1_PORT 0x3F8
#define COM2_PORT 0x2F8
#define BAUD_RATE 115200

//IO

#define PIC1_COMMAND 0x20
#define PIC1_DATA    0x21
#define PIC2_COMMAND 0xA0
#define PIC2_DATA    0xA1

#define ICW1_INIT    0x10
#define ICW1_ICW4    0x01
#define ICW4_8086    0x01

// VIRTUAL ADRESSES

#define KERNEL_VIRT    0xC0000000
#define USER_SPACE     0x00000000
#define HEAP_VIRT      0x40000000
#define STACK_VIRT     0xBFFFFFFC

//PAGING

#define PAGE_SIZE      4096
#define PD_ENTRIES     1024
#define PT_ENTRIES     1024
#define BITMAP_START   0x01000000

//PAGE FLAGS
#define PG_PRESENT     (1 << 0)
#define PG_WRITABLE    (1 << 1)  
#define PG_USER        (1 << 2)
#define PG_WRITETHRU   (1 << 3)
#define PG_CACHE_DIS   (1 << 4)
#define PG_ACCESSED    (1 << 5)
#define PG_DIRTY       (1 << 6)
#define PG_GLOBAL      (1 << 8)



#endif
