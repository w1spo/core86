// OUTB Support
// Copyright © Grzegorz Patryk Dytko, w1spo
// Created 28/10/2025
//
// Provides basic OUTB support for Core86 Kernel
// Supports custom OUTB & INB values

void outb(unsigned short port, unsigned char value) {
    __asm__ volatile ("outb %0, %1" : : "a"(value), "Nd"(port));
}

unsigned char inb(unsigned short port) {
    unsigned char ret;
    __asm__ volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}
