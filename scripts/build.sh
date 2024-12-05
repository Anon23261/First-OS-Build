#!/bin/bash

# Exit on error and undefined variables
set -eu
set -o pipefail

# Enable debug mode if DEBUG=1
[[ "${DEBUG:-0}" == "1" ]] && set -x

# Colors for logging
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m'

# Logging functions
log_info() { echo -e "${GREEN}[INFO]${NC} $1" >&2; }
log_warn() { echo -e "${YELLOW}[WARN]${NC} $1" >&2; }
log_error() { echo -e "${RED}[ERROR]${NC} $1" >&2; }

# Configuration
KERNEL_DIR="kernel"
BUILD_DIR="build"
ISO_DIR="$BUILD_DIR/iso"
BOOT_DIR="$ISO_DIR/boot"
GRUB_DIR="$BOOT_DIR/grub"
PARALLEL_JOBS=${JOBS:-$(nproc)}

# Compiler and linker configuration
CXX="${CXX:-clang++}"
AS="${AS:-nasm}"
LD="${LD:-ld.lld}"

CXXFLAGS="-target i686-pc-none-elf -ffreestanding -fno-rtti -fno-exceptions -nostdlib -O2 -Wall -Wextra -Werror"
ASFLAGS="-f elf32"
LDFLAGS="-T linker.ld -nostdlib"

# Check for required tools and files
check_dependencies() {
    local tools=("$CXX" "$AS" "$LD" "xorriso" "grub-mkrescue")
    for tool in "${tools[@]}"; do
        if ! command -v "$tool" >/dev/null 2>&1; then
            log_error "Required tool '$tool' not found in PATH"
            exit 1
        fi
    done

    if [[ ! -d "$KERNEL_DIR" ]]; then
        log_error "Kernel directory not found"
        exit 1
    fi

    if [[ ! -f "linker.ld" ]]; then
        log_error "Linker script not found"
        exit 1
    fi
}

# Find source files
find_sources() {
    log_info "Finding source files..."
    CPP_SOURCES=$(find "$KERNEL_DIR" -name "*.cpp")
    ASM_SOURCES=$(find "$KERNEL_DIR" -name "*.asm")
    HEADERS=$(find "$KERNEL_DIR" -name "*.h")
    
    if [[ -z "$CPP_SOURCES" && -z "$ASM_SOURCES" ]]; then
        log_error "No source files found in $KERNEL_DIR"
        exit 1
    fi
    
    # Generate object file paths
    CPP_OBJECTS=""
    for src in $CPP_SOURCES; do
        obj=${src//$KERNEL_DIR/$BUILD_DIR}
        obj=${obj%.cpp}.o
        CPP_OBJECTS="$CPP_OBJECTS $obj"
    done

    ASM_OBJECTS=""
    for src in $ASM_SOURCES; do
        obj=${src//$KERNEL_DIR/$BUILD_DIR}
        obj=${obj%.asm}.o
        ASM_OBJECTS="$ASM_OBJECTS $obj"
    done
    ALL_OBJECTS="$CPP_OBJECTS $ASM_OBJECTS"
}

# Create build directory structure
setup_dirs() {
    log_info "Creating build directories..."
    mkdir -p "$BUILD_DIR" "$ISO_DIR" "$BOOT_DIR" "$GRUB_DIR"
}

# Compile C++ files
compile_cpp() {
    local source=$1
    local object=$2
    local dir=$(dirname "$object")
    
    mkdir -p "$dir"
    log_info "Compiling $source..."
    
    if ! "$CXX" $CXXFLAGS -I"$KERNEL_DIR" -c "$source" -o "$object"; then
        log_error "Failed to compile $source"
        return 1
    fi
}

# Compile assembly files
compile_asm() {
    local source=$1
    local object=$2
    local dir=$(dirname "$object")
    
    mkdir -p "$dir"
    log_info "Assembling $source..."
    
    if ! "$AS" $ASFLAGS "$source" -o "$object"; then
        log_error "Failed to assemble $source"
        return 1
    fi
}

# Build kernel
build_kernel() {
    # Compile all source files in parallel
    log_info "Compiling source files..."
    
    local pids=()
    for source in $CPP_SOURCES; do
        object=$BUILD_DIR/$(basename "${source%.*}").o
        compile_cpp "$source" "$object" &
        pids+=($!)
    done

    for source in $ASM_SOURCES; do
        object=$BUILD_DIR/$(basename "${source%.*}").o
        compile_asm "$source" "$object" &
        pids+=($!)
    done

    # Wait for all compilations to finish
    for pid in "${pids[@]}"; do
        if ! wait "$pid"; then
            log_error "A compilation process failed"
            exit 1
        fi
    done

    # Link kernel
    log_info "Linking kernel..."
    if ! "$LD" $LDFLAGS -o "$BOOT_DIR/kernel.bin" $ALL_OBJECTS; then
        log_error "Failed to link kernel"
        exit 1
    fi
}

# Create ISO
create_iso() {
    # Create GRUB config
    log_info "Creating GRUB configuration..."
    cat > "$GRUB_DIR/grub.cfg" << EOF
menuentry "OS" {
    multiboot /boot/kernel.bin
    boot
}
EOF

    # Create ISO
    log_info "Creating ISO..."
    if ! grub-mkrescue -o "$BUILD_DIR/os.iso" "$ISO_DIR"; then
        log_error "Failed to create ISO"
        exit 1
    fi
    
    log_info "Created ISO at $BUILD_DIR/os.iso"
}

# Clean build directory
clean() {
    log_info "Cleaning build directory..."
    rm -rf "$BUILD_DIR"/*
}

# Main build process
main() {
    local target=${1:-all}
    
    case "$target" in
        clean)
            clean
            ;;
        rebuild)
            clean
            check_dependencies
            find_sources
            setup_dirs
            build_kernel
            create_iso
            ;;
        iso)
            if [[ ! -f "$BOOT_DIR/kernel.bin" ]]; then
                log_error "Kernel binary not found. Run build first."
                exit 1
            fi
            create_iso
            ;;
        all)
            check_dependencies
            find_sources
            setup_dirs
            build_kernel
            create_iso
            log_info "Build completed successfully!"
            ;;
        *)
            log_error "Unknown target: $target"
            echo "Usage: $0 [clean|rebuild|iso|all]"
            exit 1
            ;;
    esac
}

# Handle errors
trap 'log_error "An error occurred on line $LINENO. Exit code: $?"' ERR

# Run main with arguments
main "$@"
