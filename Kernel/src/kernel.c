// Definicje typów
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;

// Definicje kolorów dla VGA (DOS palette)
#define VGA_ADDRESS 0xB8000
#define VGA_WIDTH 80
#define VGA_HEIGHT 25

// Kolory DOS - klasyczna kombinacja
enum vga_color {
    VGA_COLOR_BLACK = 0,
    VGA_COLOR_BLUE = 1,
    VGA_COLOR_GREEN = 2,
    VGA_COLOR_CYAN = 3,
    VGA_COLOR_RED = 4,
    VGA_COLOR_MAGENTA = 5,
    VGA_COLOR_BROWN = 6,
    VGA_COLOR_LIGHT_GREY = 7,
    VGA_COLOR_DARK_GREY = 8,
    VGA_COLOR_LIGHT_BLUE = 9,
    VGA_COLOR_LIGHT_GREEN = 10,
    VGA_COLOR_LIGHT_CYAN = 11,
    VGA_COLOR_LIGHT_RED = 12,
    VGA_COLOR_LIGHT_MAGENTA = 13,
    VGA_COLOR_YELLOW = 14,
    VGA_COLOR_WHITE = 15,
};

// Tworzenie koloru VGA
static inline uint8_t vga_entry_color(enum vga_color fg, enum vga_color bg) {
    return fg | bg << 4;
}

// Tworzenie znaku VGA
static inline uint16_t vga_entry(unsigned char uc, uint8_t color) {
    return (uint16_t) uc | (uint16_t) color << 8;
}

// Tablica bufora VGA
volatile uint16_t* vga_buffer = (volatile uint16_t*)VGA_ADDRESS;

// Czyszczenie ekranu z określonym kolorem
void clear_screen(uint8_t color) {
    for (int y = 0; y < VGA_HEIGHT; y++) {
        for (int x = 0; x < VGA_WIDTH; x++) {
            const int index = y * VGA_WIDTH + x;
            vga_buffer[index] = vga_entry(' ', color);
        }
    }
}

// Wyświetlanie pojedynczego znaku
void put_char(char c, uint8_t color, int x, int y) {
    const int index = y * VGA_WIDTH + x;
    vga_buffer[index] = vga_entry(c, color);
}

// Wyświetlanie stringa
void print_string(const char* str, uint8_t color, int x, int y) {
    int i = 0;
    while (str[i] != '\0') {
        put_char(str[i], color, x + i, y);
        i++;
    }
}

// Główna funkcja kernela
void kernel_main(void) {
    // DOS-like colors: Szary tekst na niebieskim tle
    uint8_t dos_color = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    
    // Czyszczenie ekranu na niebiesko (jak DOS)
    clear_screen(dos_color);
    
    // Hello World! w górnym lewym rogu
    const char* hello = "Hello World!";
    print_string(hello, dos_color, 0, 0);  // x=0, y=0 - górny lewy róg
    
    // Dodatkowy tekst żeby pokazać więcej DOS vibe
    const char* dos_text = "C:\\>_";
    print_string(dos_text, dos_color, 0, 1);
    
    // Nieskończona pętla
    while (1) {
        __asm__ volatile ("hlt");
    }
}