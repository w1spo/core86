#!/bin/bash
set -e

rm -rf Build
mkdir -p Build/Compile
# Compile Bootloader
i686-elf-as Mandatory/boot.s -o Build/Compile/boot.o

# Compile Kernel
i686-elf-gcc -c Kernel/src/KERNEL.c -o Build/Compile/kernel.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra -nostdlib -nostdinc

#Compile VGA Driver
i686-elf-gcc -ffreestanding -c Kernel/src/Include/VGA_DRV.c -o Build/Compile/vga.o

#Compile OUTB Class
i686-elf-gcc -ffreestanding -c Kernel/src/Include/OUTB.c -o Build/Compile/outb.o

#Compile PS/2 Driver Class
i686-elf-gcc -ffreestanding -c Kernel/src/Include/PS2KB.c -o Build/Compile/ps2drv.o



#Compile UTILS
i686-elf-gcc -ffreestanding -c Kernel/src/Include/UTILS.c -o Build/Compile/UTILS.o

#Compile PMM/VMM/HEAP
i686-elf-gcc -ffreestanding -c Kernel/src/Include/PMM.c -o Build/Compile/pmm.o
i686-elf-gcc -ffreestanding -c Kernel/src/Include/VMM.c -o Build/Compile/vmm.o
i686-elf-gcc -ffreestanding -c Kernel/src/Include/HEAP.c -o Build/Compile/heap.o
i686-elf-gcc -ffreestanding -c Kernel/src/Include/DEBUG.c -o Build/Compile/debug.o

#Compile IDT & PIC & PIT & ASM CODE FOR IRQs

i686-elf-gcc -ffreestanding -c Kernel/src/Include/IDT.c -o Build/Compile/idt.o
i686-elf-gcc -ffreestanding -c Kernel/src/Include/PIC.c -o Build/Compile/pic.o
i686-elf-gcc -ffreestanding -c Kernel/src/Include/PIT.c -o Build/Compile/pit.o

#ASM
nasm -f elf32 pit_irq_handler.asm -o pit_handler.o
nasm -f elf32 ps2_irq_handler.asm -o ps2_asm_handler.o
nasm -f elf32 idt_load.asm -o idt_load.o

# Link
i686-elf-gcc -T Mandatory/linker.ld -o Build/kernel.bin -ffreestanding -O2 -nostdlib Build/Compile/boot.o Build/Compile/kernel.o Build/Compile/vga.o Build/Compile/outb.o Build/Compile/ps2drv.o Build/Compile/pmm.o Build/Compile/UTILS.o Build/Compile/heap.o Build/Compile/vmm.o Build/Compile/debug.o Build/Compile/idt_load.o Build/Compile/idt.o Build/Compile/pic.o Build/Compile/ps2_asm_handler.o Build/Compile/pit_handler.o Build/Compile/pit.o
# Multiboot GRUB check
if grub-file --is-x86-multiboot Build/kernel.bin; then
    echo "Multiboot confirmed"
else
    echo "File is not multiboot"
    exit 1
fi

# Create Directories
mkdir -p Build/isodir/boot/grub
cp Build/kernel.bin Build/isodir/boot/kernel.bin
cp Mandatory/grub.cfg Build/isodir/boot/grub/grub.cfg
cp Mandatory/RunQemu.sh Build/

# Create ISO image
grub-mkrescue -o Build/system.iso Build/isodir

echo "✅ Build completed: Build/system.iso"
