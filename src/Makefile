# Source directory
SRC_DIR := .

# Build directory for object files
BUILD_DIR := build
ISO_DIR := $(BUILD_DIR)/iso_root
RES_DIR := $(SRC_DIR)/resources/

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
ASFLAGS := -felf64 -g # Important!
CFLAGS := -m64 -std=gnu99 -g -ffreestanding -Wall -Wextra -Wno-int-conversion -fno-stack-protector -fno-stack-check -mno-mmx -mno-sse -mno-red-zone -mcmodel=kernel
QEMU_FLAGS = -cpu qemu64 -cdrom $(PROJ_NAME).iso -s -D ./debug.log -d int,cpu_reset -serial file:output.log
LDFLAGS := -T linker.ld -o $(BUILD_DIR)/$(PROJ_NAME).elf -ffreestanding -nostdlib -m64 -z max-page-size=0x1000

# Default target
all: exec

super-gdb: CFLAGS += -DMORE_DEBUG 
super-gdb: gdb
gdb: QEMU_FLAGS += -S
gdb: debug
debug: CFLAGS += -g3
debug: QEMU_FLAGS += -no-reboot -no-shutdown
debug: all

exec: $(BUILD_DIR) $(OBJS)
	${cc_TARGET}-objcopy -O elf64-x86-64 -B i386:x86-64 -I binary $(RES_DIR)/Uni3-Terminus16.psf $(BUILD_DIR)/font.o
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJS) $(BUILD_DIR)/font.o -o $(BUILD_DIR)/$(PROJ_NAME).bin
	mkdir -p $(ISO_DIR)
	mkdir -p $(ISO_DIR)/boot
# Limine config
	cp -v $(BUILD_DIR)/$(PROJ_NAME).bin $(ISO_DIR)/boot/
	mkdir -p $(ISO_DIR)/boot/limine
	cp $(SRC_DIR)/limine_bin/limine-bios-cd.bin $(ISO_DIR)/boot/limine/
	cp $(SRC_DIR)/limine_bin/limine-uefi-cd.bin $(ISO_DIR)/boot/limine/
	cp $(SRC_DIR)/limine_bin/limine-bios.sys $(ISO_DIR)/boot/limine/
	cp $(SRC_DIR)/limine_bin/limine.conf $(ISO_DIR)/boot/limine/
# Limine boot
	mkdir -p $(ISO_DIR)/EFI/BOOT
	cp -v $(SRC_DIR)/limine_bin/BOOTIA32.EFI $(ISO_DIR)/EFI/BOOT/
	cp -v $(SRC_DIR)/limine_bin/BOOTX64.EFI $(ISO_DIR)/EFI/BOOT/
	cp $(SRC_DIR)/limine_bin/cheddar.jpg $(ISO_DIR)/
# Make disk image
	xorriso -as mkisofs -R -r -J -b /boot/limine/limine-bios-cd.bin \
        -no-emul-boot -boot-load-size 4 -boot-info-table -hfsplus \
        -apm-block-size 2048 --efi-boot /boot/limine/limine-uefi-cd.bin \
        -efi-boot-part --efi-boot-image --protective-msdos-label \
        $(ISO_DIR) -o $(PROJ_NAME).iso
# Need the Limine tool built on your system. If it doesn't exist then go get it lol
	$(SRC_DIR)/limine_bin/limine_tool bios-install $(PROJ_NAME).iso
# We want to give Qemu 8G of fake RAM to work with.
	qemu-system-x86_64 -m 8G $(QEMU_FLAGS)

	
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
	rm -rf $(BUILD_DIR) $(PROJ_NAME).* *.log


.PHONY: all clean