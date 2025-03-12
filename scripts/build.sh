#!/bin/bash

# Exit on error
set -e

# Script directory
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
PROJECT_DIR="$( cd "$SCRIPT_DIR/.." && pwd )"

# Parse command line arguments
BUILD_TYPE="Debug"
BUILD_DIR="$PROJECT_DIR/build"
CLEAN=false
RUN=false
TEST=false
INSTALL=false
INSTALL_PREFIX="/usr/local"

print_usage() {
    echo "Usage: $0 [options]"
    echo "Options:"
    echo "  -h, --help            Show this help message"
    echo "  -t, --type TYPE       Build type (Debug, Release, RelWithDebInfo, MinSizeRel)"
    echo "  -d, --dir DIR         Build directory"
    echo "  -c, --clean           Clean build directory before building"
    echo "  -r, --run             Run the executable after building"
    echo "  --test                Run tests after building"
    echo "  --install             Install after building"
    echo "  --prefix PREFIX       Installation prefix (default: /usr/local)"
}

while [[ $# -gt 0 ]]; do
    case $1 in
        -h|--help)
            print_usage
            exit 0
            ;;
        -t|--type)
            BUILD_TYPE="$2"
            shift
            shift
            ;;
        -d|--dir)
            BUILD_DIR="$2"
            shift
            shift
            ;;
        -c|--clean)
            CLEAN=true
            shift
            ;;
        -r|--run)
            RUN=true
            shift
            ;;
        --test)
            TEST=true
            shift
            ;;
        --install)
            INSTALL=true
            shift
            ;;
        --prefix)
            INSTALL_PREFIX="$2"
            shift
            shift
            ;;
        *)
            echo "Unknown option: $1"
            print_usage
            exit 1
            ;;
    esac
done

# Validate build type
case $BUILD_TYPE in
    Debug|Release|RelWithDebInfo|MinSizeRel)
        ;;
    *)
        echo "Invalid build type: $BUILD_TYPE"
        echo "Valid build types: Debug, Release, RelWithDebInfo, MinSizeRel"
        exit 1
        ;;
esac

# Clean build directory if requested
if [ "$CLEAN" = true ]; then
    echo "Cleaning build directory: $BUILD_DIR"
    rm -rf "$BUILD_DIR"
fi

# Create build directory if it doesn't exist
mkdir -p "$BUILD_DIR"

# Navigate to build directory
cd "$BUILD_DIR"

# Configure
echo "Configuring project with build type: $BUILD_TYPE"
cmake -DCMAKE_BUILD_TYPE="$BUILD_TYPE" -DCMAKE_INSTALL_PREFIX="$INSTALL_PREFIX" "$PROJECT_DIR"

# Build
echo "Building project"
cmake --build . --config "$BUILD_TYPE" -- -j$(nproc)

# Run tests if requested
if [ "$TEST" = true ]; then
    echo "Running tests"
    ctest --output-on-failure -C "$BUILD_TYPE"
fi

# Install if requested
if [ "$INSTALL" = true ]; then
    echo "Installing to $INSTALL_PREFIX"
    cmake --install . --config "$BUILD_TYPE"
fi

# Run if requested
if [ "$RUN" = true ]; then
    echo "Running executable"
    "$BUILD_DIR/thales"
fi

echo "Build completed successfully"
