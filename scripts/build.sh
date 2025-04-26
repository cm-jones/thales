#!/bin/sh
set -e

# Default values
ASAN="OFF"
BUILD_TYPE="Debug"
CLEAN=false
RUN_TESTS=false

# Parse arguments
while [ "$#" -gt 0 ]; do
  case "$1" in
    --asan)
      ASAN="ON"
      shift
      ;;
    --type)
      BUILD_TYPE="$2"
      shift 2
      ;;
    --clean)
      CLEAN=true
      shift
      ;;
    --test)
      RUN_TESTS=true
      shift
      ;;
    *)
      echo "Unknown option: $1"
      exit 1
      ;;
  esac
done

# Navigate to the project root directory
cd "$(git rev-parse --show-toplevel)"

# Clean build directory if requested
if [ "$CLEAN" = true ]; then
  echo "Cleaning build directory..."
  rm -rf build
fi

# Create and navigate to the build directory
mkdir -p build
cd build

# Run CMake to configure the project
echo "Configuring with BUILD_TYPE=$BUILD_TYPE..."
cmake -DCMAKE_BUILD_TYPE="$BUILD_TYPE" -DENABLE_ASAN="ASAN" ..

# Build the project
echo "Building..."
make

# Run tests if requested
if [ "$RUN_TESTS" = true ]; then
  echo "Running tests..."
  ctest --output-on-failure
fi

echo "Build completed successfully!"
