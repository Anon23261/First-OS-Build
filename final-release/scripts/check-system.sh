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

# Check system requirements
check_system() {
    log_info "Checking system requirements..."
    
    # Check CPU architecture
    arch=$(uname -m)
    if [[ "$arch" != "x86_64" && "$arch" != "i686" ]]; then
        log_error "Unsupported CPU architecture: $arch"
        log_error "This OS requires x86 or x86_64 architecture"
        exit 1
    fi
    
    # Check available memory
    mem_kb=$(grep MemTotal /proc/meminfo | awk '{print $2}')
    if (( mem_kb < 1024000 )); then  # Less than 1GB
        log_warn "Low memory detected: $(( mem_kb / 1024 ))MB"
        log_warn "Building might be slow. Recommended: at least 1GB RAM"
    fi
    
    # Check disk space
    space_kb=$(df -k . | awk 'NR==2 {print $4}')
    if (( space_kb < 1024000 )); then  # Less than 1GB
        log_warn "Low disk space: $(( space_kb / 1024 ))MB free"
        log_warn "Recommended: at least 1GB free space"
    fi
}

# Check required packages
check_packages() {
    log_info "Checking required packages..."
    
    local missing_packages=()
    local packages=(
        "build-essential"
        "clang"
        "lld"
        "nasm"
        "xorriso"
        "grub-pc-bin"
        "grub-common"
        "cmake"
        "make"
        "qemu-system-x86"
        "gdb"
    )
    
    for pkg in "${packages[@]}"; do
        if ! dpkg -l "$pkg" &>/dev/null; then
            missing_packages+=("$pkg")
        fi
    done
    
    if (( ${#missing_packages[@]} > 0 )); then
        log_warn "Missing required packages: ${missing_packages[*]}"
        read -p "Would you like to install them now? (y/n) " -n 1 -r
        echo
        if [[ $REPLY =~ ^[Yy]$ ]]; then
            sudo apt-get update
            sudo apt-get install -y "${missing_packages[@]}"
        else
            log_error "Required packages must be installed to continue"
            exit 1
        fi
    fi
}

# Check toolchain versions
check_versions() {
    log_info "Checking toolchain versions..."
    
    # Check clang version
    if ! clang++ --version | grep -q "version [89]"; then
        log_warn "Clang version might be too old"
        log_warn "Recommended: Clang 8.0 or newer"
    fi
    
    # Check NASM version
    if ! nasm -v | grep -q "version 2"; then
        log_warn "NASM version might be too old"
        log_warn "Recommended: NASM 2.0 or newer"
    fi
    
    # Check CMake version
    if ! cmake --version | grep -q "version 3"; then
        log_warn "CMake version might be too old"
        log_warn "Recommended: CMake 3.0 or newer"
    fi
}

# Fix common issues
fix_issues() {
    log_info "Fixing common issues..."
    
    # Fix file permissions
    log_info "Fixing file permissions..."
    chmod +x ../build.sh
    chmod +x build.sh
    
    # Fix line endings
    log_info "Fixing line endings..."
    if command -v dos2unix >/dev/null; then
        find .. -type f -name "*.sh" -exec dos2unix {} \;
        find .. -type f -name "*.cpp" -exec dos2unix {} \;
        find .. -type f -name "*.h" -exec dos2unix {} \;
        find .. -type f -name "*.asm" -exec dos2unix {} \;
    else
        log_warn "dos2unix not installed, skipping line ending fixes"
    fi
    
    # Create required directories
    log_info "Creating required directories..."
    mkdir -p ../build
    mkdir -p ../build/iso/boot/grub
}

# Main function
main() {
    log_info "Starting system check..."
    
    check_system
    check_packages
    check_versions
    
    if [[ "${1:-}" == "--fix" ]]; then
        fix_issues
    fi
    
    log_info "System check completed successfully!"
}

# Run main
main "$@"
