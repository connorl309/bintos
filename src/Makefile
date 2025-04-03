# Source directory
SRC_DIR := .

# Build directory for object files
BUILD_DIR := build
ISO_DIR := $(BUILD_DIR)/iso_root

RES_DIR := resources

# Generated binary name
# If you change this you have to change the grub.cfg as well.
PROJ_NAME = bintos

# Find all .asm and .c files in the source directory
SRCS := $(shell find $(SRC_DIR) -name '*.asm' -o -name '*.c')

# Generate list of object files, preserving source directory structure
OBJS := $(SRCS:$(SRC_DIR)/%.asm=$(BUILD_DIR)/%.o)
OBJS := $(OBJS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

# Compiler, assembler, and linker
AS := nasm
CC := ${cc_TARGET}-gcc
ASFLAGS := -felf32 # Important!
CFLAGS := -m64 -std=gnu99 -ffreestanding -Wall -Wextra -Wno-int-conversion -fno-stack-protector -fno-stack-check -fno-PIC -mno-mmx -mno-sse -mno-red-zone -mcmodel=kernel
QEMU_FLAGS = -cdrom $(BUILD_DIR)/KERNEL_ISO.iso -serial file:debug.log
LDFLAGS := -T linker.ld -o $(BUILD_DIR)/$(PROJ_NAME).bin -ffreestanding -nostdlib -m64 -static -z max-page-size=0x1000

# Default target
all: exec

debug: CFLAGS += -g3
debug: QEMU_FLAGS += -s -S
debug: clean all

exec: $(BUILD_DIR) $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJS) -o $(BUILD_DIR)/$(PROJ_NAME).bin
	mkdir -p $(ISO_DIR)
	mkdir -p $(ISO_DIR)/boot
# Limine config
	cp -v $(BUILD_DIR)/$(PROJ_NAME).bin $(ISO_DIR)/boot/
	mkdir -p $(ISO_DIR)/boot/limine
	cp -v $(SRC_DIR)/limine_bin/limine-bios-cd.bin $(ISO_DIR)/boot/limine/
	cp -v $(SRC_DIR)/limine_bin/limine-uefi-cd.bin $(ISO_DIR)/boot/limine/
	cp -v $(SRC_DIR)/limine_bin/limine-bios.sys $(ISO_DIR)/boot/limine/
	cp -v $(SRC_DIR)/limine_bin/limine.conf $(ISO_DIR)/boot/limine/
# Limine boot
	mkdir -p $(ISO_DIR)/EFI/BOOT
	cp -v $(SRC_DIR)/limine_bin/BOOTIA32.EFI $(ISO_DIR)/EFI/BOOT/
	cp -v $(SRC_DIR)/limine_bin/BOOTX64.EFI $(ISO_DIR)/EFI/BOOT/
# Make disk image
	xorriso -as mkisofs -R -r -J -b /boot/limine/limine-bios-cd.bin \
        -no-emul-boot -boot-load-size 4 -boot-info-table -hfsplus \
        -apm-block-size 2048 --efi-boot /boot/limine/limine-uefi-cd.bin \
        -efi-boot-part --efi-boot-image --protective-msdos-label \
        $(ISO_DIR) -o $(PROJ_NAME).iso
# Need the Limine tool built on your system. If it doesn't exist then go get it lol
	$(SRC_DIR)/limine_bin/limine_tool bios-install $(PROJ_NAME).iso
	
# the below line is for debugging CPU activity
# qemu-system-i386 -s -D ./debug.log -d int,cpu_reset -cdrom KERNEL_ISO.iso

# Rule to create build directory
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Rule to compile .s files into .o, maintaining path structure
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.asm
	mkdir -p $(dir $@)
	$(AS) $(ASFLAGS) -o $@ $<

# Rule to compile .c files into .o, maintaining path structure
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c -o $@ $<

# Clean up generated files
clean:
	rm -rf $(BUILD_DIR) $(PROJ_NAME).bin


.PHONY: all clean