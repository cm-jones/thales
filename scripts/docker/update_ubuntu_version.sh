#!/bin/bash
set -e

# Colors for output
GREEN='\033[0;32m'
RED='\033[0;31m'
NC='\033[0m' # No Color

# Find the repository root directory using git
if ! command -v git &> /dev/null; then
    echo -e "${RED}Error: Git is not installed${NC}"
    exit 1
fi

REPO_ROOT="$(git rev-parse --show-toplevel)"
VERSION_FILE="${REPO_ROOT}/.version"
ENV_FILE="${REPO_ROOT}/docker/.env"

# Check if files exist
if [ ! -f "$VERSION_FILE" ]; then
    echo -e "${RED}Error: .version file not found at $VERSION_FILE${NC}"
    exit 1
fi

if [ ! -f "$ENV_FILE" ]; then
    echo -e "${RED}Error: .env file not found at $ENV_FILE${NC}"
    exit 1
fi

# Extract Ubuntu version from .version file
UBUNTU_VERSION=$(grep "UBUNTU_VERSION" "$VERSION_FILE" | cut -d= -f2)

if [ -z "$UBUNTU_VERSION" ]; then
    echo -e "${RED}Error: Could not extract UBUNTU_VERSION from $VERSION_FILE${NC}"
    exit 1
fi

echo "Found Ubuntu version: $UBUNTU_VERSION"

# Update the .env file
sed -i.bak "s/^UBUNTU_VERSION=.*/UBUNTU_VERSION=$UBUNTU_VERSION/" "$ENV_FILE"
rm -f "${ENV_FILE}.bak"

echo -e "${GREEN}Successfully updated Ubuntu version to $UBUNTU_VERSION in $ENV_FILE${NC}"
