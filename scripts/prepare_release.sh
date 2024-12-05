#!/bin/bash

# Exit on error
set -e

VERSION=$(cat ../VERSION)
RELEASE_DIR="release-v$VERSION"

# Create release directory structure
mkdir -p "$RELEASE_DIR"/{bin,docs,iso}

# Copy necessary files
cp ../build/*.iso "$RELEASE_DIR/iso/"
cp ../README.md "$RELEASE_DIR/docs/"
cp ../CHANGELOG.md "$RELEASE_DIR/docs/"
cp ../LICENSE "$RELEASE_DIR/docs/"

# Create release archive
tar -czf "$RELEASE_DIR.tar.gz" "$RELEASE_DIR"

echo "Release package created: $RELEASE_DIR.tar.gz"
