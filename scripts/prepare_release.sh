#!/bin/bash

VERSION="1.1.1"
RELEASE_DIR="release"
FINAL_PACKAGE="custom-os-v${VERSION}.tar.gz"

# Create release directory
rm -rf ${RELEASE_DIR}
mkdir -p ${RELEASE_DIR}

# Copy necessary files
cp -r kernel ${RELEASE_DIR}/
[ -d docs ] && cp -r docs ${RELEASE_DIR}/
[ -d tools ] && cp -r tools ${RELEASE_DIR}/
[ -d examples ] && cp -r examples ${RELEASE_DIR}/

# Copy documentation files if they exist
for file in VERSION CHANGELOG.md RELEASE_NOTES.md README.md LICENSE; do
    [ -f "$file" ] && cp "$file" ${RELEASE_DIR}/
done

# Create the release package
tar czf ${FINAL_PACKAGE} ${RELEASE_DIR}

echo "Release package created: ${FINAL_PACKAGE}"
echo "Version: ${VERSION}"

# Verify the package
if [ -f "${FINAL_PACKAGE}" ]; then
    echo "Package creation successful!"
    echo "Size: $(du -h ${FINAL_PACKAGE} | cut -f1)"
else
    echo "Error: Package creation failed!"
    exit 1
fi
