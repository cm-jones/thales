#!/bin/sh

# Navigate to the project root directory
cd "$(git rev-parse --show-toplevel)"

# Directory where the compilation database is located
BUILD_DIR=build

# Check if the build directory exists
if [ ! -d "$BUILD_DIR" ]; then
    echo "Build directory not found. Please build the project first."
    exit 1
fi

# Determine the number of available CPU cores
if command -v nproc > /dev/null 2>&1; then
    NUM_CORES=$(nproc)
elif command -v sysctl > /dev/null 2>&1; then
    NUM_CORES=$(sysctl -n hw.ncpu)
else
    echo "Unable to determine the number of CPU cores. Defaulting to 1 core."
    NUM_CORES=1
fi

# Find changed files (staged in git)
CHANGED_FILES=$(git diff --cached --name-only --diff-filter=ACM | grep -E '\.cpp$|\.h$')

if [ -z "$CHANGED_FILES" ]; then
    echo "No C++ files to lint."
    exit 0
fi

# Run clang-tidy on the changed files
echo "$CHANGED_FILES" | xargs -P"$NUM_CORES" clang-tidy -p $BUILD_DIR --extra-arg=-std=c++17 --warnings-as-errors=* --quiet

# Check the result of clang-tidy
if [ $? -ne 0 ]; then
    echo "clang-tidy found issues."
    exit 1
else
    echo "clang-tidy checks passed."
    exit 0
fi
