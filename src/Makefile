# Source directory
SRC_DIR := .

# Build directory for object files
BUILD_DIR := build

# Generated binary name
# If you change this you have to change the grub.cfg as well.
PROJ_NAME = output.bin

# Find all .s and .c files in the source directory
SRCS := $(shell find $(SRC_DIR) -name '*.s' -o -name '*.c')

# Generate list of object files, preserving source directory structure
OBJS := $(SRCS:$(SRC_DIR)/%.s=$(BUILD_DIR)/%.o)
OBJS := $(OBJS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

# Compiler, assembler, and linker
AS := i686-elf-as
CC := i686-elf-gcc
ASFLAGS :=
CFLAGS := -std=gnu99 -ffreestanding -Wall -Wextra
LDFLAGS := -T linker.ld -o output.bin -ffreestanding -nostdlib -lgcc

# Default target
all: $(BUILD_DIR) $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS)
	@grub-file --is-x86-multiboot $(PROJ_NAME) && echo "Multiboot check passed!" || { echo "Multiboot check failed!" >&2; exit 1; }
	@echo "\033[95mKernel built as binary! Multiboot formatting not yet done.\033[0m"
	mkdir -p build/isodir/boot/grub
	cp $(PROJ_NAME) build/isodir/boot/
	cp grub.cfg build/isodir/boot/grub/
	@grub-mkrescue -o KERNEL_ISO.iso build/isodir
	qemu-system-i386 -cdrom KERNEL_ISO.iso

# Rule to create build directory
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Rule to compile .s files into .o, maintaining path structure
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.s
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