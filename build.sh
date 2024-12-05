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
BUILD_DIR="build"
BUILD_TYPE=${1:-Release}
PARALLEL_JOBS=${JOBS:-$(nproc)}

# Verify required tools and files
check_dependencies() {
    local required_tools=("cmake" "make" "clang++" "nasm" "ld.lld" "grub-mkrescue" "xorriso")
    for tool in "${required_tools[@]}"; do
        if ! command -v "$tool" &> /dev/null; then
            log_error "Required tool '$tool' is not installed"
            exit 1
        fi
    done

    local required_files=("CMakeLists.txt" "kernel" "linker.ld")
    for file in "${required_files[@]}"; do
        if [[ ! -e "$file" ]]; then
            log_error "Required file/directory '$file' not found"
            exit 1
        fi
    done
}

# Create build directory
setup_build_dir() {
    log_info "Setting up build directory..."
    mkdir -p "$BUILD_DIR"
}

# Configure CMake
configure_cmake() {
    log_info "Configuring CMake with build type: $BUILD_TYPE"
    if ! cd "$BUILD_DIR"; then
        log_error "Failed to change to build directory"
        exit 1
    fi
    
    if ! cmake -DCMAKE_BUILD_TYPE="$BUILD_TYPE" ..; then
        log_error "CMake configuration failed"
        exit 1
    fi
    
    cd ..
}

# Build the project
build_project() {
    log_info "Building project..."
    if ! cmake --build "$BUILD_DIR" -j"$PARALLEL_JOBS"; then
        log_error "Build failed"
        exit 1
    fi
}

# Create ISO
create_iso() {
    log_info "Creating ISO..."
    if [[ ! -f "$BUILD_DIR/kernel.bin" ]]; then
        log_error "Kernel binary not found"
        exit 1
    fi
    
    if ! grub-mkrescue -o "$BUILD_DIR/os.iso" "$BUILD_DIR/iso"; then
        log_error "ISO creation failed"
        exit 1
    fi
    
    log_info "Created ISO at $BUILD_DIR/os.iso"
}

# Clean build directory
clean() {
    log_info "Cleaning build directory..."
    if [[ -d "$BUILD_DIR" ]]; then
        rm -rf "$BUILD_DIR"
    fi
}

# Main function
main() {
    local target=${1:-all}
    
    case "$target" in
        clean)
            clean
            ;;
        rebuild)
            clean
            check_dependencies
            setup_build_dir
            configure_cmake
            build_project
            create_iso
            ;;
        iso)
            if [[ ! -d "$BUILD_DIR" ]]; then
                log_error "Build directory not found. Run build first."
                exit 1
            fi
            create_iso
            ;;
        all)
            check_dependencies
            setup_build_dir
            configure_cmake
            build_project
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
