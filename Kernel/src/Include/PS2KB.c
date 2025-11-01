// BIOS PS2 Driver
// Copyright © Grzegorz Patryk Dytko, w1spo
// Created 31/10/2025
//
// Provides PS/2 Keyboard Input via BIOS
// Handles Function Keys, Smart Keys and Keyboard Shortcuts
// Spookyy hehe


#include "PS2KB.h"
#include "OUTB_H.h"
#include "VGA_H.h"
#include "kstint.h"

// Keyboard maps
static char keyboard_map[128] = {
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 
    0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0,
    '*', 0, ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    '-', 0, 0, 0, '+', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

static char keyboard_shift_map[128] = {
    0,  27, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b',
    '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',
    0, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~',
    0, '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0,
    '*', 0, ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    '-', 0, 0, 0, '+', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

static ukint_8 altPressed = 0;
static ukint_8 shiftPressed = 0;
static ukint_8 ctrlPressed = 0;

#define KB_BUFFER_SIZE 256
#define INPUT_KB_BUFFER 128

static char KB_BUFFER[KB_BUFFER_SIZE];
static ukint32 bufferStart = 0;
static ukint32 bufferEnd = 0;
static ukint32 bufferCount = 0;

void KB_WAIT(void) {
    while (inb(PS2_STATUS_PORT) & (unsigned char) INPUT_BUFFER_FULL)
    {
        //KB WAIT
    }
}

void KB_SEND_CMD(ukint_8 cmd) {
    KB_WAIT();
    outb(PS2_CMD_PORT, cmd);

}

ukint_8 KB_READ(void){
    KB_WAIT();
    return inb(PS2_DATA_PORT);
}

void PS2_KB_INIT(void) {
    while (inb(PS2_STATUS_PORT) & OUTPUT_BUFFER_FULL) {
        inb(PS2_DATA_PORT);
    }

    KB_SEND_CMD(0xAE);
    KB_SEND_CMD(0x20);
    ukint_8 status = KB_READ();
    status |= 0x01;
    KB_SEND_CMD(0x60);
    outb(PS2_DATA_PORT, status);
    outb(PS2_DATA_PORT, 0xF4);
    KB_READ();

}

void KB_PUTC(char c) {
    if (bufferCount < KB_BUFFER_SIZE){
        KB_BUFFER[bufferEnd] = c;
        bufferEnd = (bufferEnd + 1) % KB_BUFFER_SIZE;
        bufferCount++;
    }
}

char PS2_KB_GETC(void) {
    while (bufferCount == 0) {
        asm volatile("hlt");
    }

    char c = KB_BUFFER[bufferStart];
    bufferStart = (bufferStart + 1) % KB_BUFFER_SIZE;
    bufferCount--;

    return c;
}

int PS2_KB_HAS_INPUT(){
    return bufferCount > 0;
}

void PS2_KB_HANDLER(void) {
    ukint_8 scancode = inb(PS2_DATA_PORT);
    ukint_8 keyPressed = !(scancode & 0x80);
    

    if (keyPressed) {
        switch (scancode) {
            case 0x2A: //Left Shift
                shiftPressed = 1;
                break;
            case 0x36: //Right Shift
                shiftPressed = 1;
                break;
            case 0x1D: //Ctrl
                ctrlPressed = 1;
                break;
            case 0x38: //Alt
                altPressed = 1;
                break;
            default:
                ukint_8 keyCode = scancode & 0x7F;
                char c = shiftPressed ? keyboard_shift_map[keyCode] : keyboard_map[keyCode];
                if (c) {
                    KB_PUTC(c);
                }
                break;
        }
    } else {
        switch (scancode) {
            case 0xAA: //Left Shift release (0x2A + 0x80)
                shiftPressed = 0;
                break;
            case 0xB6: //Right Shift release (0x36 + 0x80)
                shiftPressed = 0;
                break;
            case 0x9D: //Ctrl release (0x1D + 0x80)
                ctrlPressed = 0;
                break;
            case 0xB8: //Alt release (0x38 + 0x80)
                altPressed = 0;
                break;
        }
    }
}

char* PS2_KB_READ_LN(void) {
    static char input_buffer[INPUT_KB_BUFFER];
    ukint32 pos = 0;

    while (1) {
        char c = PS2_KB_GETC();

        switch (c) {
            case '\n': //Enter Key
                input_buffer[pos] = '\n';
                vga_char('\n');
                return input_buffer;
            case '\b': //Backspace Key
                if (pos > 0) {
                    pos--;
                    vga_char('\b');
                    vga_char(' ');
                    vga_char('\b');

                }

                break;
            default:
                if (pos < INPUT_KB_BUFFER - 1 && c >= 32 && c <= 126)
                {   
                    input_buffer[pos++] = c;
                    vga_char(c);
                    break;
                }
        }
    }

}

void PS2_KB_INSTALL(void) {
    PS2_KB_INIT();
}

void POLLING_HANDLER(void) {

    if ((inb(PS2_STATUS_PORT) & OUTPUT_BUFFER_FULL) && (bufferCount < KB_BUFFER_SIZE - 10)) {
            PS2_KB_HANDLER();
    }
    
}