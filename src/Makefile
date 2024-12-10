# Source directory
SRC_DIR := .

# Build directory for object files
BUILD_DIR := build

# Generated binary name
# If you change this you have to change the grub.cfg as well.
PROJ_NAME = output.bin

# Find all .asm and .c files in the source directory
SRCS := $(shell find $(SRC_DIR) -name '*.asm' -o -name '*.c')

# Generate list of object files, preserving source directory structure
OBJS := $(SRCS:$(SRC_DIR)/%.asm=$(BUILD_DIR)/%.o)
OBJS := $(OBJS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

# Compiler, assembler, and linker
AS := nasm
CC := i686-elf-gcc
ASFLAGS := -felf32 # Important!
CFLAGS := -std=gnu99 -ffreestanding -Wall -Wextra -O2
LDFLAGS := -T linker.ld -o output.bin -ffreestanding -nostdlib -lgcc

# Default target
all: $(BUILD_DIR) $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS)
	@grub-file --is-x86-multiboot $(PROJ_NAME) && echo "\033[95mMultiboot check passed!\033[0m" || { echo "\033[31mMultiboot check failed!\033[0m" >&2; exit 1; }
	mkdir -p build/isodir/boot/grub
	cp $(PROJ_NAME) build/isodir/boot/
	cp grub.cfg build/isodir/boot/grub/
	@grub-mkrescue -o KERNEL_ISO.iso build/isodir
	qemu-system-i386 -cdrom KERNEL_ISO.iso -serial file:serial_out.log
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
	rm -rf $(BUILD_DIR) $(PROJ_NAME) KERNEL_ISO.iso


.PHONY: all clean