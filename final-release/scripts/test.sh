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
TEST_DIR="tests"
COVERAGE_DIR="$BUILD_DIR/coverage"

# Run unit tests
run_unit_tests() {
    log_info "Running unit tests..."
    cd "$BUILD_DIR"
    ctest --output-on-failure
    cd ..
}

# Run kernel tests in QEMU
run_kernel_tests() {
    log_info "Running kernel tests in QEMU..."
    
    # Start QEMU with test kernel
    timeout 30s qemu-system-i386 \
        -kernel "$BUILD_DIR/kernel.bin" \
        -display none \
        -serial stdio \
        -append "test" || {
            code=$?
            if [ $code -eq 124 ]; then
                log_error "Kernel tests timed out"
                exit 1
            else
                log_error "Kernel tests failed with code $code"
                exit $code
            fi
        }
}

# Generate coverage report
generate_coverage() {
    if ! command -v gcovr &>/dev/null; then
        log_warn "gcovr not found, skipping coverage report"
        return
    }
    
    log_info "Generating coverage report..."
    mkdir -p "$COVERAGE_DIR"
    
    gcovr --html --html-details \
        --output "$COVERAGE_DIR/coverage.html" \
        --root . \
        --filter "kernel/*" \
        --exclude "tests/*" \
        --exclude "build/*"
    
    log_info "Coverage report generated at $COVERAGE_DIR/coverage.html"
}

# Main function
main() {
    case "${1:-all}" in
        unit)
            run_unit_tests
            ;;
        kernel)
            run_kernel_tests
            ;;
        coverage)
            generate_coverage
            ;;
        all)
            run_unit_tests
            run_kernel_tests
            generate_coverage
            ;;
        *)
            log_error "Unknown test type: $1"
            echo "Usage: $0 [unit|kernel|coverage|all]"
            exit 1
            ;;
    esac
    
    log_info "All tests completed successfully!"
}

# Run main
main "$@"
