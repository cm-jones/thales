#!/bin/sh
set -e

# Navigate to the project root directory
cd "$(git rev-parse --show-toplevel)"

# Check if the build_asan directory exists
if [ ! -d "build_asan" ]; then
    echo "Error: build_asan directory not found. Please run scripts/build_with_asan.sh first."
    exit 1
fi

# Navigate to the build directory
cd build_asan

# Check if the executable exists
if [ ! -f "thales" ]; then
    echo "Error: thales executable not found in build_asan directory."
    exit 1
fi

# Run the application with Address Sanitizer
echo "Running thales with Address Sanitizer..."
ASAN_OPTIONS=halt_on_error=0:detect_stack_use_after_return=1 ./thales "$@"
