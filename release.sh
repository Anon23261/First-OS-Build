#!/bin/bash

# Exit on error
set -e

# Colors for output
GREEN='\033[0;32m'
RED='\033[0;31m'
NC='\033[0m'

echo -e "${GREEN}Creating release package...${NC}"

# Create release directory
VERSION="1.0.0"
RELEASE_DIR="release/v$VERSION"
mkdir -p "$RELEASE_DIR"

# Copy files
cp -r README.md LICENSE CHANGELOG.md VERSION "$RELEASE_DIR/"
cp -r kernel scripts docs "$RELEASE_DIR/"
cp Makefile CMakeLists.txt linker.ld "$RELEASE_DIR/"

# Create archive
tar -czf "release-v$VERSION.tar.gz" -C release "v$VERSION"

echo -e "${GREEN}Release package created: release-v$VERSION.tar.gz${NC}"
echo "Contents:"
tar -tvf "release-v$VERSION.tar.gz"
