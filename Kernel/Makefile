# Kompilator i linker
CC = i686-elf-gcc
LD = i686-elf-ld

# Flagi kompilatora
CFLAGS = -ffreestanding -m32 -Wall -O2 -Iinclude
LDFLAGS = -T kernel.ld

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
