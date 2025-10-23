NASM = nasm
ASMFLAGS = -f bin

SRC = $(wildcard src/*.asm)
BOOTLOADER = boot.bin

all: $(BOOTLOADER)

$(BOOTLOADER): $(SRC)
	$(NASM) $(ASMFLAGS) src/boot.asm -o ../build/$@

clean:
	rm -f ../build/$(BOOTLOADER)
