#!/bin/bash

# Colors for output
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m'

# Configuration
VERSION="1.1.1"
RELEASE_TAG="v${VERSION}"
RELEASE_TITLE="Custom OS ${RELEASE_TAG}"
RELEASE_PACKAGE="custom-os-v${VERSION}.tar.gz"
REPO_URL="https://github.com/Anon23261/First-OS-Build"
RELEASE_FILES=(
    "INSTALL.md"
    "RELEASE_NOTES.md"
    "README.md"
)

echo -e "${YELLOW}Creating GitHub Release v$VERSION${NC}"

# 1. Ensure we're on main branch
git checkout main

# 2. Pull latest changes
git pull origin main

# 3. Create and push tag if it doesn't exist
if ! git tag | grep -q "${RELEASE_TAG}"; then
    git tag -a "${RELEASE_TAG}" -m "Release ${RELEASE_TAG}"
    git push origin "${RELEASE_TAG}"
fi

# 4. Create release directory
RELEASE_DIR="github-release-v$VERSION"
mkdir -p "$RELEASE_DIR"

# 5. Copy release files
for file in "${RELEASE_FILES[@]}"; do
    cp "../$file" "$RELEASE_DIR/"
done

# 6. Create release archive
tar -czf "$RELEASE_DIR.tar.gz" "$RELEASE_DIR"

echo -e "${GREEN}Release files prepared!${NC}"
echo -e "Next steps:"
echo "1. Go to: $REPO_URL/releases/new"
echo "2. Select tag: ${RELEASE_TAG}"
echo "3. Upload files from: $RELEASE_DIR"
echo "4. Copy and paste release notes from GITHUB_RELEASE_NOTES.md"
echo "5. Click 'Publish release'"
