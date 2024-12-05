#!/bin/bash

VERSION="2.0.1"
RELEASE_NAME="ghost-sec-v${VERSION}"
BUILD_DIR="build"
RELEASE_DIR="releases/${RELEASE_NAME}"

# Create release directory structure
mkdir -p ${RELEASE_DIR}

# Build the project
echo "Building GHOST Sec v${VERSION}..."
make clean
make

# Copy necessary files
cp ${BUILD_DIR}/kernel.bin ${RELEASE_DIR}/
cp ${BUILD_DIR}/os.img ${RELEASE_DIR}/

# Create release info
cat > ${RELEASE_DIR}/README.txt << EOL
GHOST Sec v${VERSION}
===================

Security-focused operating system with integrated development environment.

Changes in v2.0.1:
- Fixed memory allocation and deallocation
- Fixed terminal dimensions handling
- Improved filesystem error handling
- Fixed keyboard input handling
- Fixed type mismatches in editor
- General code cleanup and optimization

To run:
1. Boot os.img in your preferred emulator
2. Use Ctrl+R to compile and run code
3. Use Ctrl+S to save files
4. Use Ctrl+Q to exit editor

For support and updates, contact the GHOST Sec team.
EOL

# Create checksum
cd ${RELEASE_DIR}
sha256sum kernel.bin os.img README.txt > SHA256SUMS

echo "Release v${VERSION} prepared successfully in ${RELEASE_DIR}"
echo "Don't forget to tag the release in git:"
echo "git tag -a v${VERSION} -m \"GHOST Sec v${VERSION}\""
echo "git push origin v${VERSION}"
