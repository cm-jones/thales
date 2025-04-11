#!/bin/bash
set -ex

# Create temporary Docker container
CONTAINER_ID=$(docker run -d --rm ubuntu:22.04 sleep 300)

# Install build dependencies inside the container
docker exec $CONTAINER_ID apt-get update
docker exec $CONTAINER_ID apt-get install -y build-essential cmake libboost-all-dev libpq-dev git pkg-config libyaml-cpp-dev libpqxx-dev

# Copy the project files inside the container
docker cp . $CONTAINER_ID:/app
docker exec $CONTAINER_ID bash -c "cd /app && mkdir -p build && cd build && cmake .. -DCMAKE_BUILD_TYPE=Release --log-level=VERBOSE 2>&1" | tee cmake_debug_log.txt

# Cleanup
docker kill $CONTAINER_ID
