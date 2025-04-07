#!/bin/sh
set -e

# Navigate to the project root directory
cd "$(git rev-parse --show-toplevel)"

# Check if build directory exists
if [ ! -d "build" ]; then
  echo "Error: Build directory not found. Please run build.sh first."
  exit 1
fi

# Navigate to the build directory
cd build

# Run tests
echo "Running tests..."
ctest --output-on-failure

echo "Tests completed successfully!"
