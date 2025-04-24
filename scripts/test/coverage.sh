#!/bin/sh
set -e

# Navigate to the project root directory
cd "$(git rev-parse --show-toplevel)"

# Check if lcov is installed
if ! command -v lcov > /dev/null; then
  echo "Error: lcov is not installed. Please install it first."
  echo "  Ubuntu: sudo apt install lcov"
  echo "  macOS: brew install lcov"
  exit 1
fi

# Create and navigate to the build directory
mkdir -p build
cd build

# Configure with coverage enabled
echo "Configuring with coverage enabled..."
cmake -DENABLE_COVERAGE=ON -DCMAKE_BUILD_TYPE=Debug ..

# Build the project
echo "Building..."
make

# Run tests
echo "Running tests..."
ctest --output-on-failure

# Generate coverage report
echo "Generating coverage report..."
lcov --directory . --capture --output-file coverage.info
echo "Coverage file created: $(du -h coverage.info | cut -f1)"

# Remove system headers and test files from coverage report
lcov --remove coverage.info '/usr/*' '/opt/*' '*/third_party/*' '*/tests/*' --output-file filtered_coverage.info
echo "Filtered coverage file created: $(du -h filtered_coverage.info | cut -f1)"

# Generate HTML report
echo "Generating HTML report..."
mkdir -p coverage_report
genhtml filtered_coverage.info --output-directory coverage_report

echo "Coverage report generated in build/coverage_report/index.html"
echo "Open this file in a browser to view the coverage report"

# Print coverage summary
echo "Coverage summary:"
lcov --summary filtered_coverage.info

echo "Coverage testing completed successfully!"
