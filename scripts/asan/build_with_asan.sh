#!/bin/sh
set -e

# Navigate to the project root directory
cd "$(git rev-parse --show-toplevel)"

# Create and navigate to the build directory
mkdir -p build_asan
cd build_asan

# Run CMake to configure the project with Address Sanitizer
cmake -DENABLE_ASAN=ON ..

# Build the project
make

echo ""
echo "To run the main application with ASan, use:"
echo "ASAN_OPTIONS=halt_on_error=0:detect_stack_use_after_return=1 ./thales"
echo ""
echo "To run the tests with ASan, use:"
echo "ASAN_OPTIONS=halt_on_error=0:detect_stack_use_after_return=1 ./tests/thales_tests"
echo ""
