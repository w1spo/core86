#!/bin/bash
qemu-system-i386 -kernel isodir/boot/kernel.bin -S -s
gdb isodir/boot/kernel.bin
