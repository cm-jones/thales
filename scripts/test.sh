#!/bin/sh
set -e

# Navigate to the project root directory
cd "$(git rev-parse --show-toplevel)"

# Navigate to the build directory
cd build

# Run tests
ctest --output-on-failure
