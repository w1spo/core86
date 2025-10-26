#!/bin/bash
set -e  # zatrzymaj skrypt przy błędzie

rm -rf Build
mkdir -p Build/Compile

# Kompilacja asemblera
i686-elf-as boot.s -o Build/Compile/boot.o

# Kompilacja C
i686-elf-gcc -c Kernel/src/kernel.c -o Build/Compile/kernel.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra -nostdlib -nostdinc

# Linkowanie
i686-elf-gcc -T linker.ld -o Build/kernel.bin -ffreestanding -O2 -nostdlib Build/Compile/boot.o Build/Compile/kernel.o

# Sprawdzenie multiboot
if grub-file --is-x86-multiboot Build/kernel.bin; then
    echo "Multiboot confirmed"
else
    echo "File is not multiboot"
    exit 1
fi

# Tworzenie struktury katalogów
mkdir -p Build/isodir/boot/grub
cp Build/kernel.bin Build/isodir/boot/kernel.bin
cp grub.cfg Build/isodir/boot/grub/grub.cfg

# Tworzenie obrazu ISO
grub-mkrescue -o Build/system.iso Build/isodir

echo "✅ Build completed: Build/system.iso"
