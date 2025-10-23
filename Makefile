.PHONY: all clean run debug bootloader kernel image

# Konfiguracja ścieżek
BOOTLOADER_DIR = build
KERNEL_DIR = build
BUILD_DIR = build

# Pliki wyjściowe
BOOTLOADER_BIN = $(BOOTLOADER_DIR)/boot.bin
KERNEL_BIN = $(KERNEL_DIR)/kernel.bin
OS_IMAGE = $(BUILD_DIR)/os-image.img

# Narzędzia
DD = dd
QEMU = qemu-system-i386

# Domyślny cel
all: bootloader kernel image


# Tworzenie obrazu dyskietki
image: bootloader kernel $(BUILD_DIR)
	@echo "=== Tworzenie obrazu dyskietki ==="
	# Tworzenie pustego obrazu 1.44MB
	$(DD) if=/dev/zero of=$(OS_IMAGE) bs=512 count=2880 status=none
	# Kopiowanie bootloadera do pierwszego sektora
	$(DD) if=$(BOOTLOADER_BIN) of=$(OS_IMAGE) conv=notrunc status=none
	# Kopiowanie kernela zaczynając od sektora 2
	$(DD) if=$(KERNEL_BIN) of=$(OS_IMAGE) bs=512 seek=1 conv=notrunc status=none
	@echo "Obraz utworzony: $(OS_IMAGE)"
	@echo "Rozmiar bootloadera: $$(stat -c%s $(BOOTLOADER_BIN)) bajtów"
	@echo "Rozmiar kernela: $$(stat -c%s $(KERNEL_BIN)) bajtów"

# Uruchomienie w QEMU
run: image
	@echo "=== Uruchamianie w QEMU ==="
	$(QEMU) -drive format=raw,file=$(OS_IMAGE)

# Uruchomienie z debugowaniem
debug: image
	@echo "=== Uruchamianie w QEMU z debuggerem ==="
	@echo "Podłącz GDB: target remote localhost:1234"
	$(QEMU) -drive format=raw,file=$(OS_IMAGE) -s -S


# Informacje o projekcie
info:
	@echo "=== Informacje o projekcie ==="
	@echo "Struktura:"
	@echo "  Bootloader: $(BOOTLOADER_DIR)/"
	@echo "  Kernel:     $(KERNEL_DIR)/"
	@echo "  Build:      $(BUILD_DIR)/"
	@echo ""
	@echo "Pliki:"
	@[ -f "$(BOOTLOADER_BIN)" ] && echo "  Bootloader: $(BOOTLOADER_BIN) ($$(stat -c%s $(BOOTLOADER_BIN)) B)" || echo "  Bootloader: NIE ISTNIEJE"
	@[ -f "$(KERNEL_BIN)" ] && echo "  Kernel:     $(KERNEL_BIN) ($$(stat -c%s $(KERNEL_BIN)) B)" || echo "  Kernel:     NIE ISTNIEJE"
	@[ -f "$(OS_IMAGE)" ] && echo "  Obraz:      $(OS_IMAGE) ($$(stat -c%s $(OS_IMAGE)) B)" || echo "  Obraz:      NIE ISTNIEJE"
