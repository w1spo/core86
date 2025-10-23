NASM = nasm
ASMFLAGS = -f bin

SRC = $(wildcard src/*.asm)
BOOTLOADER = boot.bin

all: $(BOOTLOADER)

$(BOOTLOADER): $(SRC)
	$(NASM) $(ASMFLAGS) src/boot.asm -o ../build/$@
	qemu-system-i386 -fda ../build/$(BOOTLOADER)


clean:
	rm -f ../build/$(BOOTLOADER)

run:
	qemu-system-i386 -fda ../build/$(BOOTLOADER)