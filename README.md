<p align="center">
<pre>
 ██████  ██████  ██████  ███████  █████   ██████  
██      ██    ██ ██   ██ ██      ██   ██ ██       
██      ██    ██ ██████  █████    █████  ███████  
██      ██    ██ ██   ██ ██      ██   ██ ██    ██ 
 ██████  ██████  ██   ██ ███████  █████   ██████  
</pre>
</p>                                            
                                          
                                                                               

# 💾 Core86 Operating System

> ⚙️ A modern re-imagination of DOS — built entirely from scratch, with full low-level control, real-mode & protected-mode support, and an evolving kernel written in pure C and Assembly.  
> 🧠 Made by **w1spo_** — programmer, game developer & hardware nerd.

---

## 🧩 Overview

**Core86** is a handcrafted x86 operating system built to bring back the classic MS-DOS spirit 🖥️ — but redefined for the modern hacker mindset.  
It’s written completely from scratch in **x86 Assembly** and **C**, with a boot process that loads a protected-mode kernel, hardware abstraction layer, and a modular architecture designed for experimentation, performance, and nostalgia.

> “Not just another bootloader — Core86 is a full OS built from the silicon up.” 🔩

---

## 🚀 Vision & Goals

- 🧠 **Educational and experimental** – built to understand every level of the x86 architecture.
- 💾 **Fully modular kernel** – easily expandable and maintainable.
- 🧰 **Full MS-DOS compatibility layer** (planned) for running real DOS programs.
- ⚙️ **HAL (Hardware Abstraction Layer)** for managing BIOS, A20, interrupts, and I/O.
- 🏁 **Custom bootloader & kernel loading system**, compatible with GRUB and direct MBR boot.
- 🕹️ **Retro gaming / demo-scene potential** – perfect platform for low-level experiments.
- 🔧 **Written 100% from scratch** — no libraries, no dependencies, no external bootloaders needed (but supports GRUB for testing).
- 🎯 **Goal:** build a stable, self-hosting OS capable of running DOS-like applications and C programs compiled for Core86.

---

## 🧠 Key Features (Current + Planned)

### 🖥️ Boot & Kernel
- Custom 16-bit bootloader (NASM)
- Transition to 32-bit Protected Mode
- GRUB + Multiboot2 compatible ELF kernel support
- A20 line enabling + memory checks
- Real-mode and protected-mode interop
- Text-mode VGA output driver

### ⚙️ Kernel Core
- Written in **C + Assembly**
- Real multitasking system (planned)
- HAL (Hardware Abstraction Layer)
  - Keyboard, VGA, PIT, CMOS, A20
  - I/O port management
- Physical & virtual memory management
- Modular architecture (drivers, tasks, user programs)

### 💽 File System
- FAT12/FAT16/FAT32 read/write support (planned)
- In-memory VFS layer
- Simple file API for DOS-like access

### 🧱 DOS Compatibility
- **MS-DOS system call emulation**
- `.COM` and `.EXE` support (planned)
- DOS interrupts (INT 21h, INT 10h, INT 13h)
- 8.3 file naming convention
- Command interpreter similar to DOS shell

### 💡 Developer Experience
- Full source in **NASM** + **C (GCC)**  
- Clean **linker.ld** for custom segment mapping  
- QEMU, Bochs, and real hardware support
- Debug builds with GDB
- BIOS interrupt fallback for early stage debugging

### 🎨 UI & Experience (Planned)
- Text-based DOS-style shell
- Optional graphical UI (VGA 320x200 mode)
- Mouse and keyboard input drivers
- Windowing system (future project)
- Built-in utilities (`edit`, `mem`, `copy`, `coreinfo`)

---

## 🧰 Technical Details

| Component | Description |
|------------|--------------|
| **Bootloader** | Custom ASM + GRUB Bootloader |
| **Kernel Entry** | `_start` symbol in 32-bit C |
| **Compiler** | `i686-elf-gcc` or `gcc -m32 -ffreestanding` |
| **Linker** | Custom `linker.ld` aligning `.multiboot` before `.text` |
| **Kernel Format** | ELF32 with Multiboot header |
| **Memory Load Address** | `0x00100000` (1MB) |
| **Architecture** | x86 (Real + Protected Mode) |
| **Display** | VGA Text Mode (0xB8000) |

---

## 🧩 Future Plans

- 🧱 Build the **CoreDOS subsystem** for full DOS app compatibility  
- 🧠 Add **memory paging + virtual memory manager**
- ⌨️ Implement **keyboard interrupt driver**
- 🎵 Add **PC Speaker / SoundBlaster audio driver**
- 🛜 Add **Networking and HAL**
- 🪟 Create minimal **graphical interface (CoreShell GUI)**
- 📦 Implement **Core86 Package Manager (CPM)** for apps/drivers
- 🧵 Add **multitasking scheduler** (Round-Robin)
- ⚙️ Add **system calls layer** for user programs
- 🧑‍💻 Self-hosting goal: **compile Core86 from Core86**

---

## 🧪 Build Instructions (Quick Start)

```bash
#!/bin/bash
set -e

rm -rf Build
mkdir -p Build/Compile

# Compile Bootloader
i686-elf-as Mandatory/boot.s -o Build/Compile/boot.o

# Compile Kernel
i686-elf-gcc -c Kernel/src/kernel.c -o Build/Compile/kernel.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra -nostdlib -nostdinc

# Link
i686-elf-gcc -T Mandatory/linker.ld -o Build/kernel.bin -ffreestanding -O2 -nostdlib Build/Compile/boot.o Build/Compile/kernel.o

# Multiboot GRUB check
if grub-file --is-x86-multiboot Build/kernel.bin; then
    echo "Multiboot confirmed"
else
    echo "File is not multiboot"
    exit 1
fi

# Create Directories
mkdir -p Build/isodir/boot/grub
cp Build/kernel.bin Build/isodir/boot/kernel.bin
cp Mandatory/grub.cfg Build/isodir/boot/grub/grub.cfg
cp Mandatory/RunQemu.sh Build/

# Create ISO image
grub-mkrescue -o Build/system.iso Build/isodir

echo "✅ Build completed: Build/system.iso"

