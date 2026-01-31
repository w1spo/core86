#include "print.h"
#include "config.h"
#include "serial.h"
#include "types.h"
#include "utils.h"

char buf[12];

void kprintf(const char* format, ...) 
{
    va_list args;
    va_start(args, format);
    
    char buffer[16];
    
    while(*format) 
    {
        if(*format == '%') 
        {
            format++;
            
            switch(*format) 
            {
                case 'd':
                case 'i': 
                {
                    int value = va_arg(args, int);
                    itoa(value, buffer, 10);
                    serial_write_string(COM1_PORT, buffer);
                    break;
                }
                
                case 'u': 
                {
                    uint32 value = va_arg(args, uint32);
                    itoa(value, buffer, 10);
                    serial_write_string(COM1_PORT, buffer);
                    break;
                }
                
                case 'x': 
                {
                    uint32 value = va_arg(args, uint32);
                    hex_to_str(value, buffer);
                    serial_write_string(COM1_PORT, buffer);
                    break;
                }
             
                case 'p': 
                {
                    void* ptr = va_arg(args, void*);
                    serial_write_string(COM1_PORT, "0x");
                    hex_to_str((uint32) ptr, buffer);
                    serial_write_string(COM1_PORT, buffer);
                    break;
                }
                
                case 's': 
                {
                    char* str = va_arg(args, char*);
                    serial_write_string(COM1_PORT, str);
                    break;
                }
                
                case 'c': 
                {
                    char c = (char)va_arg(args, int);
                    serial_write_char(COM1_PORT, c);
                    break;
                }
                
                case '%': 
                {
                    serial_write_char(COM1_PORT, '%');
                    break;
                }
                
                default: 
                {
                    serial_write_char(COM1_PORT, '%');
                    serial_write_char(COM1_PORT, *format);
                    break;
                }
            }
        } 
        else 
        {
            serial_write_char(COM1_PORT, *format);
        }
        
        format++;
    }
    
    va_end(args);
}

void print(char* string)
{
    serial_write_string(COM1_PORT, string);
    serial_write_string(COM2_PORT, string);
}

void printl(char* string)
{
    serial_write_line(COM1_PORT, string);
    serial_write_line(COM2_PORT, string);
}

void printc(char charact)
{
    serial_write_char(COM1_PORT, charact);
    serial_write_char(COM2_PORT, charact);
}

void printh(uint32 hex)
{
    hex_to_str(hex, buf);
    print(buf);
}