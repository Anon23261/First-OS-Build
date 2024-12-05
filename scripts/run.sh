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
ISO_FILE="$BUILD_DIR/os.iso"
QEMU_OPTS="-m 128M -serial stdio -display sdl"
GDB_PORT=1234

# Check if ISO exists
check_iso() {
    if [[ ! -f "$ISO_FILE" ]]; then
        log_error "ISO file not found at $ISO_FILE"
        log_error "Please build the OS first with: ./build.sh"
        exit 1
    fi
}

# Run OS in QEMU
run_normal() {
    log_info "Running OS in QEMU..."
    qemu-system-i386 $QEMU_OPTS -cdrom "$ISO_FILE"
}

# Run OS in QEMU with GDB debugging
run_debug() {
    log_info "Starting OS in debug mode..."
    log_info "Run 'gdb' in another terminal to connect"
    qemu-system-i386 $QEMU_OPTS -cdrom "$ISO_FILE" -s -S
}

# Run OS in QEMU with KVM acceleration
run_kvm() {
    if ! command -v kvm-ok >/dev/null || ! kvm-ok >/dev/null 2>&1; then
        log_warn "KVM not available, falling back to normal mode"
        run_normal
        return
    fi
    
    log_info "Running OS with KVM acceleration..."
    qemu-system-i386 $QEMU_OPTS -cdrom "$ISO_FILE" -enable-kvm
}

# Show help
show_help() {
    echo "Usage: $0 [OPTIONS]"
    echo
    echo "Options:"
    echo "  normal    Run OS normally (default)"
    echo "  debug     Run OS with GDB debugging enabled"
    echo "  kvm       Run OS with KVM acceleration"
    echo "  help      Show this help message"
}

# Main function
main() {
    check_iso
    
    case "${1:-normal}" in
        normal)
            run_normal
            ;;
        debug)
            run_debug
            ;;
        kvm)
            run_kvm
            ;;
        help)
            show_help
            ;;
        *)
            log_error "Unknown option: $1"
            show_help
            exit 1
            ;;
    esac
}

# Run main
main "$@"
