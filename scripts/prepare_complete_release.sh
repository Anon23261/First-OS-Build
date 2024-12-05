#!/bin/bash

# Colors for output
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m'

VERSION="1.1.1"
RELEASE_DIR="complete-release-v$VERSION"

echo -e "${YELLOW}Preparing complete release package...${NC}"

# Create directory structure
mkdir -p "$RELEASE_DIR"/{src,docs,tools,examples}

# Copy source files if they exist
for dir in kernel scripts; do
    [ -d "../$dir" ] && cp -r "../$dir" "$RELEASE_DIR/src/"
done
[ -d "../tools" ] && cp -r "../tools" "$RELEASE_DIR/tools/"
for file in Makefile CMakeLists.txt linker.ld; do
    [ -f "../$file" ] && cp "../$file" "$RELEASE_DIR/src/"
done

# Copy documentation if it exists
for file in README.md INSTALL.md CHANGELOG.md LICENSE RELEASE_NOTES.md; do
    [ -f "../$file" ] && cp "../$file" "$RELEASE_DIR/docs/"
done

# Create examples directory
mkdir -p "$RELEASE_DIR/examples/hello_world"
cat > "$RELEASE_DIR/examples/hello_world/hello.txt" << EOF
Hello World!
This is an example file created in our custom OS.
Try editing this file using our text editor!
EOF

# Create quick start guide
cat > "$RELEASE_DIR/QUICK_START.md" << EOF
# Quick Start Guide

1. Build the OS:
   \`\`\`bash
   ./tools/build.sh
   \`\`\`

2. Run in QEMU:
   \`\`\`bash
   ./tools/run.sh
   \`\`\`

3. Try the editor:
   - Press Ctrl+O to open a file
   - Press Ctrl+S to save
   - Press Ctrl+Q to quit

4. Example commands:
   - Open example: \`open examples/hello_world/hello.txt\`
   - Save file: Press Ctrl+S
   - Exit editor: Press Ctrl+Q
EOF

# Create archive
tar -czf "$RELEASE_DIR.tar.gz" "$RELEASE_DIR"

echo -e "${GREEN}Complete release package created: $RELEASE_DIR.tar.gz${NC}"
