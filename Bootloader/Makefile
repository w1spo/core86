NASM = nasm
ASMFLAGS = -f bin -g

SRC = src/boot.asm
BIN = boot.bin

all: $(BIN)

$(BIN): $(SRC)
	$(NASM) $(ASMFLAGS) $< -o ../build/$@

clean:
	rm -f $(BIN)

.PHONY: all clean