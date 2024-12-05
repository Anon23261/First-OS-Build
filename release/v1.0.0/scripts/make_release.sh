#!/bin/bash

# Exit on error
set -e

# Colors for output
GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
NC='\033[0m'

VERSION=$(cat VERSION)
echo -e "${GREEN}Preparing release v$VERSION${NC}"

# Step 1: Setup toolchain if needed
if ! command -v i686-elf-g++ &> /dev/null; then
    echo -e "${YELLOW}Setting up toolchain...${NC}"
    ./scripts/setup_toolchain.sh
fi

# Step 2: Run tests
echo -e "\n${GREEN}Running tests...${NC}"
./scripts/test_release.sh

# Step 3: Clean and rebuild
echo -e "\n${GREEN}Building fresh release...${NC}"
make clean
make

# Step 4: Prepare release package
echo -e "\n${GREEN}Creating release package...${NC}"
./scripts/prepare_release.sh

echo -e "\n${GREEN}Release v$VERSION is ready!${NC}"
echo "Release package: release-v$VERSION.tar.gz"
echo -e "\nNext steps:"
echo "1. Test the release package"
echo "2. Create a git tag: git tag -a v$VERSION -m 'Release v$VERSION'"
echo "3. Push the tag: git push origin v$VERSION"
