# Source directory
SRC_DIR := .

# Build directory for object files
BUILD_DIR := build

RES_DIR := resources

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
CFLAGS := -std=gnu99 -ffreestanding -Wall -Wextra -Wno-int-conversion
QEMU_FLAGS = -cdrom $(BUILD_DIR)/KERNEL_ISO.iso -serial file:output.log
LDFLAGS := -T linker.ld -o $(BUILD_DIR)/output.bin -ffreestanding -nostdlib -lgcc

# Default target
all: exec

debug: CFLAGS += -g
debug: QEMU_FLAGS += -s -S
debug: all

exec: $(BUILD_DIR) $(OBJS)
	i686-elf-objcopy -O elf32-i386 -B i386 -I binary $(RES_DIR)/Uni3-Terminus16.psf $(BUILD_DIR)/font.o
	$(CC) $(LDFLAGS) $(OBJS) $(BUILD_DIR)/font.o
	@grub-file --is-x86-multiboot $(BUILD_DIR)/$(PROJ_NAME) && echo "\033[95mMultiboot check passed!\033[0m" || { echo "\033[31mMultiboot check failed!\033[0m" >&2; exit 1; }
	mkdir -p $(BUILD_DIR)/isodir/boot/grub
	cp $(BUILD_DIR)/$(PROJ_NAME) $(BUILD_DIR)/isodir/boot/
	cp grub.cfg $(BUILD_DIR)/isodir/boot/grub/
	@grub-mkrescue -o $(BUILD_DIR)/KERNEL_ISO.iso build/isodir
	qemu-system-i386 $(QEMU_FLAGS)
	
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
	rm -rf $(BUILD_DIR) $(PROJ_NAME)


.PHONY: all clean