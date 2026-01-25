#ifndef PS2_KB
#define PS2_KB

#include "KSTINT.h"

#define OUTPUT_BUFFER_FULL 1
#define INPUT_BUFFER_FULL (1<<1)

#define PS2_DATA_PORT 0x60
#define PS2_STATUS_PORT 0x64
#define PS2_CMD_PORT 0x64

void PS2_KB_INIT(void);
void PS2_KB_INSTALL(void);
char PS2_KB_GETC(void);
int PS2_KB_HAS_INPUT(void);
char* PS2_KB_READ_LN(void);
void PS2_KB_HANDLER();
void POLLING_HANDLER(void);
void PS2_IRQ_HANDLER(void);

#endif