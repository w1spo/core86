# Kompilator i linker
CC = gcc
LD = ld

CFLAGS = -m16 -ffreestanding -fno-pic -nostdlib -nostartfiles -fno-builtin -Iinclude
LDFLAGS = -m elf_i386 -T kernel.ld -nostdlib

# Pliki źródłowe i obiekty
SRC = $(wildcard src/*.c)
OBJ = $(SRC:.c=.o)

# Nazwa wyjściowa kernela
KERNEL = ../build/kernel.bin

# Domyślny target
all: $(KERNEL)

# Kompilacja plików .c do .o
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Linkowanie do binarki kernela
$(KERNEL): $(OBJ)
	$(LD) $(LDFLAGS) -o $@ $(OBJ)

# Sprzątanie buildów
clean:
	rm -f src/*.o $(KERNEL)
