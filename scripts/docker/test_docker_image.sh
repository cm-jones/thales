#!/bin/bash
set -e

# Print with colors for better visibility
GREEN='\033[0;32m'
RED='\033[0;31m'
NC='\033[0m' # No Color

echo "Testing Docker image..."

# Find the repository root directory
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd "${SCRIPT_DIR}/../.." && pwd)"
DOCKER_DIR="${REPO_ROOT}/docker"

# Get the image name or use default
IMAGE_NAME="${1:-thales:latest}"

echo "Testing image: ${IMAGE_NAME}"

# Run basic health check
echo "Running health check..."
if docker run --rm ${IMAGE_NAME} /bin/bash -c "command -v thales && echo -e '${GREEN}Thales binary exists${NC}'" 2>/dev/null; then
    echo -e "${GREEN}Health check passed${NC}"
else
    echo -e "${RED}Health check failed: thales binary not found in image${NC}"
    exit 1
fi

# Check directories exist
echo "Checking required directories..."
if docker run --rm ${IMAGE_NAME} /bin/bash -c "[ -d /usr/local/thales/logs ] && [ -d /usr/local/thales/data ] && [ -d /etc/thales/config ] && echo -e '${GREEN}Directories exist${NC}'" 2>/dev/null; then
    echo -e "${GREEN}Directory check passed${NC}"
else
    echo -e "${RED}Directory check failed: Required directories not found${NC}"
    exit 1
fi

# Check for required libraries
echo "Checking required libraries..."
required_libs=("libboost_system" "libpq")
all_libs_found=true

for lib in "${required_libs[@]}"; do
    if docker run --rm ${IMAGE_NAME} /bin/bash -c "ldconfig -p | grep -q ${lib}" 2>/dev/null; then
        echo -e "${GREEN}Found library: ${lib}${NC}"
    else
        echo -e "${RED}Missing library: ${lib}${NC}"
        all_libs_found=false
    fi
done

if [ "$all_libs_found" = false ]; then
    echo -e "${RED}Library check failed${NC}"
    exit 1
fi

echo -e "${GREEN}All tests passed successfully!${NC}"
echo "Docker image ${IMAGE_NAME} is ready for deployment."
