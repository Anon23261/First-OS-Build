#!/bin/bash

# Exit on error
set -e

# Colors for output
GREEN='\033[0;32m'
RED='\033[0;31m'
NC='\033[0m'

echo -e "${GREEN}Running pre-release tests...${NC}"

# Test 1: Clean Build
echo "Test 1: Clean Build"
make clean
make
if [ $? -eq 0 ]; then
    echo -e "${GREEN}✓ Build successful${NC}"
else
    echo -e "${RED}✗ Build failed${NC}"
    exit 1
fi

# Test 2: Check ISO Creation
echo "Test 2: Check ISO Creation"
if [ -f "build/os.iso" ]; then
    echo -e "${GREEN}✓ ISO file created${NC}"
else
    echo -e "${RED}✗ ISO file missing${NC}"
    exit 1
fi

# Test 3: Test in QEMU
echo "Test 3: Testing in QEMU"
timeout 10s qemu-system-i386 -cdrom build/os.iso -display none &
QEMU_PID=$!
sleep 5
if kill -0 $QEMU_PID 2>/dev/null; then
    echo -e "${GREEN}✓ OS boots successfully${NC}"
    kill $QEMU_PID
else
    echo -e "${RED}✗ OS failed to boot${NC}"
    exit 1
fi

# Test 4: Check Required Files
echo "Test 4: Checking Required Files"
required_files=(
    "README.md"
    "LICENSE"
    "CHANGELOG.md"
    "VERSION"
    "kernel/kernel.cpp"
    "kernel/keyboard.cpp"
    "kernel/editor.cpp"
)

for file in "${required_files[@]}"; do
    if [ -f "$file" ]; then
        echo -e "${GREEN}✓ Found $file${NC}"
    else
        echo -e "${RED}✗ Missing $file${NC}"
        exit 1
    fi
done

echo -e "\n${GREEN}All tests passed! Ready for release.${NC}"
