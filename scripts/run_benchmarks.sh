#!/bin/sh

# Navigate to the project root directory
cd "$(git rev-parse --show-toplevel)"

# Create build directory if it doesn't exist
mkdir -p build
cd build

# Configure and build in Release mode for better benchmark results
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build . --target thales_benchmarks

# Run the benchmarks
./benchmarks/thales_benchmarks
