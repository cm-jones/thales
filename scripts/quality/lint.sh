#!/bin/sh

# Navigate to the project root directory
cd "$(git rev-parse --show-toplevel)"

# Check if clang-tidy is installed
if ! command -v clang-tidy > /dev/null 2>&1; then
    echo "Error: clang-tidy is not installed or not in PATH."
    echo "Please install clang-tidy and try again."
    exit 1
fi

# Directory where the compilation database is located
BUILD_DIR=build

# Check if the build directory exists
if [ ! -d "$BUILD_DIR" ]; then
    echo "Build directory not found. Please build the project first."
    exit 1
fi

# Check if the compilation database exists
if [ ! -f "$BUILD_DIR/compile_commands.json" ]; then
    echo "Compilation database not found in $BUILD_DIR."
    echo "Please build the project with CMAKE_EXPORT_COMPILE_COMMANDS=ON:"
    echo "  cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -B $BUILD_DIR"
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

# Find all C++ files in the project, excluding third-party code
ALL_CPP_FILES=$(find include src tests -type f \( -name "*.cpp" -o -name "*.hpp" -o -name "*.h" \) | grep -v "third_party")

if [ -z "$ALL_CPP_FILES" ]; then
    echo "No C++ files found."
    exit 0
fi

# Determine the C++ standard library include path
if [ "$(uname)" = "Darwin" ]; then
    # macOS - try to find the include path from Homebrew's LLVM
    if [ -d "$(brew --prefix llvm)/include/c++/v1" ]; then
        STD_INCLUDE="$(brew --prefix llvm)/include/c++/v1"
    elif [ -d "/Library/Developer/CommandLineTools/usr/include/c++/v1" ]; then
        STD_INCLUDE="/Library/Developer/CommandLineTools/usr/include/c++/v1"
    elif [ -d "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/include/c++/v1" ]; then
        STD_INCLUDE="/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/include/c++/v1"
    else
        echo "Warning: Could not find C++ standard library include path. Header errors may occur."
        STD_INCLUDE=""
    fi
else
    # Linux - try to find the include path
    if [ -d "/usr/include/c++/$(gcc -dumpversion)" ]; then
        STD_INCLUDE="/usr/include/c++/$(gcc -dumpversion)"
    else
        echo "Warning: Could not find C++ standard library include path. Header errors may occur."
        STD_INCLUDE=""
    fi
fi

# Determine system include paths
if [ "$(uname)" = "Darwin" ]; then
    # macOS - try to find system include paths
    if [ -d "$(brew --prefix llvm)/include/c++/v1" ]; then
        SYSTEM_INCLUDES="-isystem $(brew --prefix llvm)/include/c++/v1 -isystem $(brew --prefix llvm)/include -isystem /usr/include"
    else
        SYSTEM_INCLUDES="-isystem /Library/Developer/CommandLineTools/usr/include/c++/v1 -isystem /usr/include"
    fi
else
    # Linux - try to find system include paths
    SYSTEM_INCLUDES="-isystem /usr/include/c++/$(gcc -dumpversion) -isystem /usr/include"
fi

# Run clang-tidy on all C++ files
echo "Running clang-tidy with system includes: $SYSTEM_INCLUDES"
if ! echo "$ALL_CPP_FILES" | xargs -P"$NUM_CORES" clang-tidy -p $BUILD_DIR \
    --extra-arg=-std=c++23 \
    --extra-arg=-stdlib=libc++ \
    --extra-arg="$SYSTEM_INCLUDES" \
    --quiet 2>&1 | grep -v "unknown warning option" | grep -v "Error while processing" | grep -v "error generated" | grep -v "warnings generated"
then
    echo "clang-tidy encountered errors."
    exit 1
fi

echo "clang-tidy checks passed."
exit 0
