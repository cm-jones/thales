#!/bin/bash
set -e

# Print with colors for better visibility
GREEN='\033[0;32m'
RED='\033[0;31m'
NC='\033[0m' # No Color

echo "Starting Thales..."

# Check if Docker is installed and running
if ! command -v docker &> /dev/null; then
    echo -e "${RED}Error: Docker is not installed${NC}"
    exit 1
fi


if ! docker info &> /dev/null; then
    echo -e "${RED}Error: Docker daemon is not running${NC}"
    exit 1
fi

# Check if docker-compose is installed
if ! command -v docker-compose &> /dev/null; then
    echo -e "${RED}Error: Docker Compose is not installed${NC}"
    exit 1
fi

# Check if git is installed
if ! command -v git &> /dev/null; then
    echo -e "${RED}Error: Git is not installed${NC}"
    exit 1
fi

# Find the repository root directory using git
REPO_ROOT="$(git rev-parse --show-toplevel)"
DOCKER_DIR="${REPO_ROOT}/docker"

# Check for .env file
if [ ! -f "$DOCKER_DIR/.env" ]; then
    echo -e "${RED}Error: .env file not found at $DOCKER_DIR/.env${NC}"
    exit 1
fi

# Source the .env file to get environment variables
source "$DOCKER_DIR/.env"

# Verify required environment variables
if [ -z "$HOST_IP" ]; then
    echo -e "${RED}Error: HOST_IP not set in .env file${NC}"
    exit 1
fi

if [ -z "$IB_GATEWAY_PORT" ]; then
    echo -e "${RED}Error: IB_GATEWAY_PORT not set in .env file${NC}"
    exit 1
fi

if [ -z "$DB_HOST" ]; then
    echo -e "${RED}Error: DB_HOST not set in .env file${NC}"
    exit 1
fi

if [ -z "$DB_USER" ]; then
    echo -e "${RED}Error: DB_USER not set in .env file${NC}"
    exit 1
fi

if [ -z "$DB_PASSWORD" ]; then
    echo -e "${RED}Error: DB_PASSWORD not set in .env file${NC}"
    exit 1
fi

if [ -z "$DB_NAME" ]; then
    echo -e "${RED}Error: DB_NAME not set in .env file${NC}"
    exit 1
fi

# Create required directories if they don't exist
mkdir -p "${REPO_ROOT}/logs"
mkdir -p "${REPO_ROOT}/data"
mkdir -p "${REPO_ROOT}/config"

# Update Ubuntu version from .version file
echo "Updating Ubuntu version from .version file..."
"${REPO_ROOT}/scripts/docker/update_ubuntu_version.sh"

# Build and start the containers
echo "Building and starting Docker containers..."
cd "${DOCKER_DIR}"

# Pull latest PostgreSQL image (without credential helpers)
echo "Pulling latest PostgreSQL image..."
DOCKER_CONFIG=/tmp docker-compose pull postgres || {
  echo -e "${RED}Warning: Could not pull latest postgres image. Will use cached version if available.${NC}"
  # Continue anyway, as we might have the image cached locally
}

# Build and start services with temporary Docker config to bypass credential helpers
if DOCKER_CONFIG=/tmp docker-compose up -d --build; then
    echo -e "${GREEN}Thales trading bot started successfully!${NC}"
    echo -e "${GREEN}PostgreSQL is running on port 5432${NC}"
    echo "Use 'docker-compose logs -f' to view logs"
    echo "Use 'docker-compose down' to stop the services"
else
    echo -e "${RED}Failed to start Docker containers${NC}"
    exit 1
fi

# Check if containers are running
if ! docker-compose ps | grep -q "thales.*Up"; then
    echo -e "${RED}Error: Thales container failed to start${NC}"
    echo "Checking container logs:"
    docker-compose logs thales
    exit 1
fi

if ! docker-compose ps | grep -q "thales-postgres.*Up"; then
    echo -e "${RED}Error: PostgreSQL container failed to start${NC}"
    echo "Checking container logs:"
    docker-compose logs postgres
    exit 1
fi

echo -e "\n${GREEN}All services are up and running!${NC}"
echo "Container status:"
docker-compose ps
