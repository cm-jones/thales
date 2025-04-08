#!/bin/bash
set -e

cd "$(git rev-parse --show-toplevel)"

# Detect host IP for different platforms
if [[ "$OSTYPE" == "darwin"* ]]; then
  # macOS
  HOST_IP="host.docker.internal"
elif [[ "$OSTYPE" == "msys"* || "$OSTYPE" == "cygwin"* ]]; then
  # Windows
  HOST_IP="host.docker.internal"
else
  # Linux
  HOST_IP=$(ip route get 1 | awk '{print $7;exit}')
fi

# Update .env file
cd docker
sed -i.bak "s/HOST_IP=.*/HOST_IP=$HOST_IP/" .env
rm -f .env.bak 2>/dev/null || true
cd ..

# Create directories
mkdir -p logs data

echo "Starting Thales Trading Bot with host IP: $HOST_IP"
echo "Make sure IB Gateway is running on your host machine on port 4001"

# Start containers
cd docker && docker-compose up -d

echo "Containers started successfully!"
