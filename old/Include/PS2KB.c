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
#include "KSTINT.h"
#include "UTILS.h"
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

extern void safe_irq_handler_asm(void);
extern void pit_irq_handler_asm(void);  
extern void ps2_irq_handler_asm(void);
void __attribute__((cdecl)) ps2_irq_handler_asm_wrapper(void);

// W PS2KB.c - DODAJ TĘ FUNKCJĘ NA GÓRZE
void __attribute__((cdecl)) ps2_irq_handler_asm_wrapper(void) {
    vga_print("[IRQ] IRQ1 triggered!\n");  // DEBUG
    
    // Wywołaj handler
    PS2_KB_HANDLER();
    
    // EOI do PIC
    outb(0x20, 0x20);
}


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
    vga_print("[PS2] Initializing keyboard...\n");
    
    // 1. Wyczyść bufor wyjściowy
    while (inb(PS2_STATUS_PORT) & OUTPUT_BUFFER_FULL) {
        inb(PS2_DATA_PORT);
    }
    
    // 2. Wyłącz klawiaturę (disable first PS/2 port)
    outb(PS2_CMD_PORT, 0xAD);
    
    // 3. Odczytaj Configuration Byte
    outb(PS2_CMD_PORT, 0x20);  // Read controller command byte
    while(!(inb(PS2_STATUS_PORT) & OUTPUT_BUFFER_FULL)) {
        // Wait for data
    }
    ukint_8 config = inb(PS2_DATA_PORT);
    
    // 4. Włącz IRQ1 (bit 0)
    config |= 0x01;
    
    // 5. Zapisz Configuration Byte
    outb(PS2_CMD_PORT, 0x60);  // Write controller command byte
    outb(PS2_DATA_PORT, config);
    
    // 6. Włącz klawiaturę
    outb(PS2_CMD_PORT, 0xAE);
    
    // 7. Wyczyść bufor znowu
    while (inb(PS2_STATUS_PORT) & OUTPUT_BUFFER_FULL) {
        inb(PS2_DATA_PORT);
    }
    
    // 8. Włącz scanning na klawiaturze
    outb(PS2_DATA_PORT, 0xF4);  // Enable scanning command
    
    // Czekaj na ACK (0xFA)
    int timeout = 100000;
    while(!(inb(PS2_STATUS_PORT) & OUTPUT_BUFFER_FULL) && timeout--);
    
    if(timeout > 0) {
        ukint_8 response = inb(PS2_DATA_PORT);
        vga_print("[PS2] Enable scanning response: 0x");
        char buf[12];
        vga_print(hex_to_str(response, buf));
        vga_print(" (should be 0xFA)\n");
    } else {
        vga_print("[PS2] Enable scanning timeout\n");
    }
    
    vga_print("[PS2] Keyboard initialized\n");
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

static ukint_8 e0_prefix = 0;

void PS2_KB_HANDLER(void) {
    ukint_8 scancode = inb(PS2_DATA_PORT);
    
    // DEBUG - możesz włączyć na początku
    // vga_print("Scancode: ");
    // vga_print(hex_to_str(scancode, buf));
    // vga_print("\n");
    
    if (scancode == E0_ESCAPE) {
        e0_prefix = 1;
        return;
    }
    
    ukint_8 keyPressed = !(scancode & KEY_RELEASED);
    ukint_8 keyCode = scancode & 0x7F;
    
    if (e0_prefix) {
        // Obsługa extended keys
        if (keyPressed) {
            switch(keyCode) {
                case 0x49: // Page Up (E0 49)
                    vga_scroll_up();
                    break;
                case 0x51: // Page Down (E0 51)
                    vga_scroll_down();
                    break;
                case 0x47: // Home (E0 47)
                    if (ctrlPressed) {
                        vga_set_scroll_offset(vga_get_scroll_max());
                    }
                    break;
                case 0x4F: // End (E0 4F)
                    if (ctrlPressed) {
                        vga_set_scroll_offset(0);
                    }
                    break;
                // ... inne extended keys
            }
        }
        e0_prefix = 0;
        return;
    }
    
    if (keyPressed) {
        switch (scancode) {
            case 0x2A: //Left Shift
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
                if (keyCode < 128) {
                    char c = shiftPressed ? keyboard_shift_map[keyCode] : keyboard_map[keyCode];
                    if (c) {
                        KB_PUTC(c);
                    }
                }
                break;
        }
    } else {
        switch (scancode & 0x7F) {
            case 0x2A: //Left Shift release
            case 0x36: //Right Shift release
                shiftPressed = 0;
                break;
            case 0x1D: //Ctrl release
                ctrlPressed = 0;
                break;
            case 0x38: //Alt release
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
    idt_set_gate(0x21, (ukint32)ps2_irq_handler_asm);
    
    // Włącz IRQ1 w PIC
    pic_enable_irq(IRQ1);  // Musisz mieć tę funkcję
    
    vga_print("[PS2] IRQ handler installed\n");
}

void POLLING_HANDLER(void) {
    // Sprawdź czy są dane
    if (inb(PS2_STATUS_PORT) & OUTPUT_BUFFER_FULL) {
        ukint_8 scancode = inb(PS2_DATA_PORT);
        
        // DEBUG - zawsze wypisz scancode
        static int debug_count = 0;
        if(debug_count < 20) {  // Tylko pierwsze 20
            char buf[12];
            vga_print("[POLL] 0x");
            vga_print(hex_to_str(scancode, buf));
            vga_print(" ");
            debug_count++;
        }
        
        // Przetwórz jeśli jest miejsce
        if(bufferCount < KB_BUFFER_SIZE) {
            PS2_KB_HANDLER();
        }
        // Jeśli bufor pełny, scancode jest już odczytany i utracony
    }
}

// Zmień funkcję PS2_IRQ_HANDLER na:
void PS2_IRQ_HANDLER(void) {
    // 1. Sprawdź czy Output Buffer ma dane (bit 0)
    if (!(inb(PS2_STATUS_PORT) & OUTPUT_BUFFER_FULL))
        return;
    
    // 2. Odczytaj scancode
    ukint_8 scancode = inb(PS2_DATA_PORT);
    
    // 3. Przetwórz klawisz
    PS2_KB_HANDLER(); // Możesz zmodyfikować funkcję aby przyjmowała scancode
    
    // 4. Wyślij EOI (End of Interrupt) do PIC
    outb(0x20, 0x20); // Master PIC
    
    // Jeśli masz Slave PIC (IRQ 8-15), wyślij też do niego:
    // outb(0xA0, 0x20); // Dla IRQ >= 8
}
