# Compiler and tools
CXX = g++
CC = gcc
AS = nasm
GRUB_MKRESCUE = grub-mkrescue
LD = ld

# Compiler and linker flags
CXXFLAGS = -m32 -ffreestanding -O2 -Wall -Wextra -fno-exceptions -fno-rtti -g -I$(KERNEL_DIR)
CFLAGS = -m32 -ffreestanding -O2 -Wall -Wextra -g -I$(KERNEL_DIR)
ASFLAGS = -f elf32
LDFLAGS = -T linker.ld -m elf_i386 -nostdlib /usr/lib/gcc/i686-linux-gnu/10/libgcc.a

# Directories
BOOT_DIR = boot
KERNEL_DIR = kernel
BUILD_DIR = build
ISO_DIR = $(BUILD_DIR)/iso

# Source files
BOOT_SOURCES = $(wildcard $(BOOT_DIR)/*.asm)
KERNEL_ASM_SOURCES = $(KERNEL_DIR)/isr.asm
KERNEL_CPP_SOURCES = $(wildcard $(KERNEL_DIR)/*.cpp)
KERNEL_C_SOURCES = $(wildcard $(KERNEL_DIR)/*.c)

# Object files
BOOT_OBJECTS = $(patsubst $(BOOT_DIR)/%.asm,$(BUILD_DIR)/%.o,$(BOOT_SOURCES))
KERNEL_ASM_OBJECTS = $(BUILD_DIR)/isr.o
KERNEL_CPP_OBJECTS = $(patsubst $(KERNEL_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(KERNEL_CPP_SOURCES))
KERNEL_C_OBJECTS = $(patsubst $(KERNEL_DIR)/%.c,$(BUILD_DIR)/%.o,$(KERNEL_C_SOURCES))
KERNEL_OBJECTS = $(KERNEL_ASM_OBJECTS) $(KERNEL_CPP_OBJECTS) $(KERNEL_C_OBJECTS)

# Output files
KERNEL_BIN = $(BUILD_DIR)/MiniOS.bin
ISO_FILE = $(BUILD_DIR)/MiniOS.iso

# Check for required tools
REQUIRED_TOOLS = $(CXX) $(CC) $(AS) $(GRUB_MKRESCUE)

.PHONY: all clean check-tools run debug

all: check-tools $(ISO_FILE)

check-tools:
	@for tool in $(REQUIRED_TOOLS); do \
		which $$tool >/dev/null 2>&1 || { echo "$$tool not found. Please install it."; exit 1; } \
	done

$(BUILD_DIR)/%.o: $(BOOT_DIR)/%.asm
	@mkdir -p $(BUILD_DIR)
	$(AS) $(ASFLAGS) $< -o $@

$(BUILD_DIR)/%.o: $(KERNEL_DIR)/%.asm
	@mkdir -p $(BUILD_DIR)
	$(AS) $(ASFLAGS) $< -o $@

$(BUILD_DIR)/%.o: $(KERNEL_DIR)/%.cpp
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: $(KERNEL_DIR)/%.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(KERNEL_BIN): $(BOOT_OBJECTS) $(KERNEL_OBJECTS)
	$(LD) $(LDFLAGS) -o $@ $^

$(ISO_FILE): $(KERNEL_BIN)
	@mkdir -p $(ISO_DIR)/boot/grub
	cp $(KERNEL_BIN) $(ISO_DIR)/boot/
	cp grub.cfg $(ISO_DIR)/boot/grub/
	$(GRUB_MKRESCUE) -o $@ $(ISO_DIR)

clean:
	rm -rf $(BUILD_DIR)

run: $(ISO_FILE)
	qemu-system-i386 -cdrom $(ISO_FILE)

debug: $(ISO_FILE)
	qemu-system-i386 -cdrom $(ISO_FILE) -s -S
