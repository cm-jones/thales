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

# Check for .env file
if [ ! -f "$(dirname "$0")/.env" ]; then
    echo -e "${RED}Error: .env file not found in docker directory${NC}"
    exit 1
fi

# Source the .env file to get environment variables
source "$(dirname "$0")/.env"

# Verify required environment variables
if [ -z "$HOST_IP" ]; then
    echo -e "${RED}Error: HOST_IP not set in .env file${NC}"
    exit 1
fi

if [ -z "$IB_PORT" ]; then
    echo -e "${RED}Error: IB_PORT not set in .env file${NC}"
    exit 1
fi

if [ -z "$DB_PASSWORD" ]; then
    echo -e "${RED}Error: DB_PASSWORD not set in .env file${NC}"
    exit 1
fi

# Create required directories if they don't exist
mkdir -p ../logs
mkdir -p ../data
mkdir -p ../config

# Build and start the containers
echo "Building and starting Docker containers..."
cd "$(dirname "$0")"

# Pull latest PostgreSQL image
docker-compose pull postgres

# Build and start services
if docker-compose up -d --build; then
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
