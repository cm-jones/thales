#!/bin/bash

# Enhanced script to generate Doxygen documentation for the Thales project
# Usage: ./scripts/documentation/generate_docs.sh [options]
#
# Options:
#   --no-browser    Generate documentation without opening browser
#   --serve         Serve documentation on a local HTTP server
#   --clean         Clean documentation directory before generation
#   --help          Show this help message

set -e

function print_help() {
    echo "Usage: ./scripts/docs/generate_docs.sh [options]"
    echo ""
    echo "Options:"
    echo "  --no-browser    Generate documentation without opening browser"
    echo "  --serve         Serve documentation on a local HTTP server (port 8080)"
    echo "  --clean         Clean documentation directory before generation"
    echo "  --help          Show this help message"
    echo ""
    echo "Examples:"
    echo "  ./scripts/documentation/generate_docs.sh                 # Generate and open in browser"
    echo "  ./scripts/documentation/generate_docs.sh --clean         # Clean and regenerate"
    echo "  ./scripts/documentation/generate_docs.sh --serve         # Generate and serve on HTTP"
    echo "  ./scripts/documentation/generate_docs.sh --no-browser    # Generate only"
    echo ""
}

# Default options
OPEN_BROWSER=true
SERVE=false
CLEAN=false

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        --no-browser)
            OPEN_BROWSER=false
            shift
            ;;
        --serve)
            SERVE=true
            shift
            ;;
        --clean)
            CLEAN=true
            shift
            ;;
        --help)
            print_help
            exit 0
            ;;
        *)
            echo "Unknown option: $1"
            print_help
            exit 1
            ;;
    esac
done

# Go to project root directory
cd "$(dirname "$0")/../.."
ROOT_DIR=$(pwd)

# Clean documentation if requested
if [ "$CLEAN" = true ]; then
    echo "Cleaning documentation directory..."
    rm -rf docs/doxygen
fi

# Make sure the docs directories exist
mkdir -p docs/doxygen docs/images docs/extra

# Check Doxyfile exists
if [ ! -f "Doxyfile" ]; then
    echo "Error: Doxyfile not found. Please create it first."
    echo "You can run: doxygen -g Doxyfile"
    exit 1
fi

# Generate the documentation
echo "Generating Thales documentation..."
doxygen Doxyfile

if [ $? -ne 0 ]; then
    echo "Error: Doxygen generation failed!"
    exit 1
fi

echo "Documentation successfully generated at:"
echo "$(pwd)/docs/doxygen/html/index.html"

# Check if there's a web browser available for opening docs
BROWSER=""
if [ "$OPEN_BROWSER" = true ]; then
    if command -v open &> /dev/null; then
        BROWSER="open"
    elif command -v xdg-open &> /dev/null; then
        BROWSER="xdg-open"
    elif command -v firefox &> /dev/null; then
        BROWSER="firefox"
    elif command -v google-chrome &> /dev/null; then
        BROWSER="google-chrome"
    fi
fi

# Handle browser opening
if [ -n "$BROWSER" ] && [ "$OPEN_BROWSER" = true ] && [ "$SERVE" = false ]; then
    echo "Opening documentation in browser..."
    $BROWSER docs/doxygen/html/index.html
fi

# Serve documentation if requested
if [ "$SERVE" = true ]; then
    echo "Starting HTTP server on port 8080..."
    echo "Documentation available at: http://localhost:8080"
    echo "Press Ctrl+C to stop."
    
    # Try different HTTP server options
    if command -v python3 &> /dev/null; then
        cd docs/doxygen/html && python3 -m http.server 8080
    elif command -v python &> /dev/null; then
        cd docs/doxygen/html && python -m SimpleHTTPServer 8080
    elif command -v php &> /dev/null; then
        cd docs/doxygen/html && php -S localhost:8080
    else
        echo "Error: No suitable HTTP server found (tried python3, python, php)"
        echo "Please install one of these or access the documentation directly at:"
        echo "$(pwd)/docs/doxygen/html/index.html"
    fi
fi

echo "Done!"
