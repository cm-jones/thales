#!/bin/bash
set -e

# Print with colors for better visibility
GREEN='\033[0;32m'
RED='\033[0;31m'
NC='\033[0m' # No Color

# Check if git is installed
if ! command -v git &> /dev/null; then
    echo -e "${RED}Error: Git is not installed${NC}"
    exit 1
fi

# Find the repository root directory using git
REPO_ROOT="$(git rev-parse --show-toplevel)"
DOCKER_DIR="${REPO_ROOT}/docker"

# Navigate to the docker directory and stop containers
cd "${DOCKER_DIR}" && docker-compose down

echo "Thales trading containers have been stopped."
