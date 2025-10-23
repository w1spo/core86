.PHONY: all clean run bootloader kernel image

# Ścieżki
BOOTLOADER_DIR = Bootloader
KERNEL_DIR = Kernel
BUILD_DIR = build

BOOTLOADER_BIN = $(BUILD_DIR)/boot.bin
KERNEL_BIN = $(BUILD_DIR)/kernel.bin
OS_IMAGE = $(BUILD_DIR)/os-image.bin

# Domyślny target
all: bootloader kernel image

# Budowa bootloadera
bootloader:
	$(MAKE) -C $(BOOTLOADER_DIR)

# Budowa kernela
kernel:
	$(MAKE) -C $(KERNEL_DIR)

# Tworzenie surowego obrazu
image: bootloader kernel
	@echo "Łączenie bootloadera i kernela w os-image.bin"
	cat $(BOOTLOADER_BIN) $(KERNEL_BIN) > $(OS_IMAGE)

# Uruchomienie w QEMU
run: image
	qemu-system-i386 -fda $(OS_IMAGE)

# Sprzątanie build
clean:
	$(MAKE) -C $(BOOTLOADER_DIR) clean
	$(MAKE) -C $(KERNEL_DIR) clean
	rm -f $(OS_IMAGE)
