#!/bin/sh

# Navigate to the project root directory
cd "$(git rev-parse --show-toplevel)"

# Check if clang-format is installed
if ! command -v clang-format > /dev/null 2>&1; then
    echo "Error: clang-format is not installed or not in PATH."
    echo "Please install clang-format and try again."
    exit 1
fi

# Find all C++ files in the project, excluding third-party code
ALL_CPP_FILES=$(find include src tests benchmarks -type f \( -name "*.cpp" -o -name "*.hpp" -o -name "*.h" \) | grep -v "third_party")

if [ -z "$ALL_CPP_FILES" ]; then
    echo "No C++ files found."
    exit 0
fi

# Run clang-format on all C++ files
if ! echo "$ALL_CPP_FILES" | xargs clang-format -i
then
    echo "Error: Failed to format C++ files."
    exit 1
fi

echo "Formatted all C++ files."
exit 0
