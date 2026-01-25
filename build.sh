#!/bin/bash
set -e

CFLAGS="-std=gnu99 -ffreestanding -O0 -g -Wall -Wextra -nostdlib -nostdinc"
CC=i686-elf-gcc


rm -rf Build
mkdir -p Build/Compile
# Compile Bootloader
i686-elf-as Mandatory/boot.s -o Build/Compile/boot.o

# Compile Kernel
echo "[*] Compiling C files..."

for file in Kernel/src/*.c Kernel/src/Include/*.c; do
    obj=Build/Compile/$(basename "$file" .c).o
    echo "    CC $file"
    $CC $CFLAGS -c "$file" -o "$obj"
done

echo "[*] Compiling ASM..."

i686-elf-as Mandatory/boot.s -o Build/Compile/boot.o

# Link
echo "[*] Linking kernel..."

i686-elf-gcc -T Mandatory/linker.ld -o Build/kernel.bin \
    -ffreestanding -nostdlib -g \
    Build/Compile/*.o

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
cp Mandatory/RunQemu_D.sh Build/

# Create ISO image
grub-mkrescue -o Build/system.iso Build/isodir

echo "✅ Build completed: Build/system.iso"
