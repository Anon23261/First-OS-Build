#!/bin/bash

# Exit on error and undefined variables
set -eu
set -o pipefail

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
PARALLEL_JOBS=${JOBS:-$(nproc)}

# Clean build directory
clean() {
    log_info "Cleaning build directory..."
    rm -rf "$BUILD_DIR"
}

# Build the project
build() {
    log_info "Building project..."
    mkdir -p "$BUILD_DIR"
    cd "$BUILD_DIR"
    cmake .. -DCMAKE_BUILD_TYPE=Release
    make -j"$PARALLEL_JOBS"
    cd ..
}

# Create ISO
create_iso() {
    log_info "Creating ISO..."
    if [ -f "$BUILD_DIR/kernel.bin" ]; then
        grub-mkrescue -o "$BUILD_DIR/os.iso" "$BUILD_DIR/kernel.bin"
        log_info "ISO created at $BUILD_DIR/os.iso"
    else
        log_error "kernel.bin not found. Build failed?"
        exit 1
    fi
}

# Show usage
usage() {
    echo "Usage: $0 [clean|rebuild|iso|all]"
    echo "  clean   - Clean build directory"
    echo "  rebuild - Clean and rebuild project"
    echo "  iso     - Create ISO image"
    echo "  all     - Clean, rebuild, and create ISO"
}

# Main function
main() {
    case "${1:-all}" in
        clean)
            clean
            ;;
        rebuild)
            clean
            build
            ;;
        iso)
            create_iso
            ;;
        all)
            clean
            build
            create_iso
            ;;
        *)
            log_error "Unknown target: $1"
            usage
            exit 1
            ;;
    esac
}

# Handle errors
trap 'log_error "An error occurred on line $LINENO. Exit code: $?"' ERR

# Run main with arguments
main "$@"
