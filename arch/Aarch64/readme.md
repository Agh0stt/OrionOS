## OrionArm OS Aarch64 ##
# Files:
1) start.S - Stage 2 bootloader
2) kernel.c - Kernel and Shell
3) linker.ld - linker script
4) bootloader.S - Bootloader
## dependencies:
1) Qemu Aarch64
2) Clang
3) llvm
4) lld
5) nasm
6) gcc
7) python
8) python-pip
## Build commands:

## 1. Compile kernel (C)
```bash
clang \
  --target=aarch64-none-elf \
  -mcpu=cortex-a53 \
  -ffreestanding \
  -nostdlib \
  -fno-builtin \
  -O2 \
  -c kernel.c \
  -o kernel.o

```
## 2. Compile Stage 2 bootloader (Assembly)
```bash
clang \
  --target=aarch64-none-elf \
  -mcpu=cortex-a53 \
  -ffreestanding \
  -nostdlib \
  -c start.S \
  -o start.o
```
## 3. Compile Bootloader (Assembly)
```bash
clang \
  --target=aarch64-none-elf \
  -mcpu=cortex-a53 \
  -ffreestanding \
  -nostdlib \
  -c bootloader.S \
  -o bootloader.o
```

## 4. Link (lld)
```bash
ld.lld \
  -T linker.ld \
 bootloader.o start.o kernel.o \
  -o kernel.elf

```
## 4. objcopy (llvm-objcopy)
```bash
llvm-objcopy -O binary kernel.elf kernel.bin
```
## 5. Run in qemu-system-aarch64 (run)
```bash

qemu-system-aarch64 \
  -M virt \
  -cpu cortex-a53 \
  -nographic \
  -kernel kernel.elf
```
## Commands Supported:
1) version
2) halt
3) reboot
4) kpanic
5) help
6) poweroff
7) shutdown
8) echo
9) calc
10) clear
## Copyright OrionOS - 2026 - present Abhigyan Ghosh Project Orion All Rights Reserved. Lisenced under the MIT lisence 


