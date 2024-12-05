# Compiler and tools
CXX = i686-elf-g++
AS = nasm
GRUB_MKRESCUE = grub-mkrescue
LD = i686-elf-ld

# Compiler and linker flags
CXXFLAGS = -ffreestanding -O2 -Wall -Wextra -fno-exceptions -fno-rtti -g -I$(KERNEL_DIR)
ASFLAGS = -f elf32
LDFLAGS = -T linker.ld -ffreestanding -O2 -nostdlib -lgcc

# Directories
BOOT_DIR = boot
KERNEL_DIR = kernel
BUILD_DIR = build
ISO_DIR = $(BUILD_DIR)/iso

# Source files
BOOT_SOURCES = $(wildcard $(BOOT_DIR)/*.asm)
KERNEL_SOURCES = $(wildcard $(KERNEL_DIR)/*.cpp)
BOOT_OBJECTS = $(patsubst $(BOOT_DIR)/%.asm,$(BUILD_DIR)/%.o,$(BOOT_SOURCES))
KERNEL_OBJECTS = $(patsubst $(KERNEL_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(KERNEL_SOURCES))

# Output files
KERNEL_BIN = $(BUILD_DIR)/MiniOS.bin
ISO_FILE = $(BUILD_DIR)/MiniOS.iso

# Check for required tools
REQUIRED_TOOLS = $(CXX) $(AS) $(GRUB_MKRESCUE)

.PHONY: all clean check-tools run debug

all: check-tools $(ISO_FILE)

check-tools:
	@for tool in $(REQUIRED_TOOLS); do \
		if ! command -v $$tool >/dev/null 2>&1; then \
			echo "Error: Required tool '$$tool' is not installed."; \
			exit 1; \
		fi \
	done

# Create build directories
$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)
	@mkdir -p $(ISO_DIR)/boot/grub

# Compile assembly files
$(BUILD_DIR)/%.o: $(BOOT_DIR)/%.asm | $(BUILD_DIR)
	@echo "Assembling $<..."
	@$(AS) $(ASFLAGS) $< -o $@

# Compile C++ files
$(BUILD_DIR)/%.o: $(KERNEL_DIR)/%.cpp | $(BUILD_DIR)
	@echo "Compiling $<..."
	@$(CXX) $(CXXFLAGS) -c $< -o $@

# Link the kernel
$(KERNEL_BIN): $(BOOT_OBJECTS) $(KERNEL_OBJECTS)
	@echo "Linking kernel..."
	@$(CXX) -T linker.ld -o $@ $(BOOT_OBJECTS) $(KERNEL_OBJECTS) $(LDFLAGS)
	@echo "Kernel binary created at $@"

# Create bootable ISO
$(ISO_FILE): $(KERNEL_BIN)
	@echo "Creating bootable ISO..."
	@mkdir -p $(ISO_DIR)/boot/grub
	@cp $(KERNEL_BIN) $(ISO_DIR)/boot/
	@cp grub.cfg $(ISO_DIR)/boot/grub/
	@$(GRUB_MKRESCUE) -o $@ $(ISO_DIR)
	@echo "ISO created at $@"

# Clean build files
clean:
	@echo "Cleaning build files..."
	@rm -rf $(BUILD_DIR)
	@echo "Clean complete"

# Run in QEMU
run: $(ISO_FILE)
	@echo "Running MiniOS in QEMU..."
	@qemu-system-i386 -cdrom $(ISO_FILE)

# Debug with QEMU and GDB
debug: $(ISO_FILE)
	@echo "Starting MiniOS in debug mode..."
	@qemu-system-i386 -cdrom $(ISO_FILE) -s -S
