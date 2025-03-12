#!/bin/sh

# Navigate to the project root directory
cd "$(git rev-parse --show-toplevel)"

# Find changed files (staged in git)
CHANGED_FILES=$(git diff --cached --name-only --diff-filter=ACM | grep -E '\.cpp$|\.h$')

if [ -z "$CHANGED_FILES" ]; then
    echo "No C++ files to format."
    exit 0
fi

# Run clang-format on the changed files
echo "$CHANGED_FILES" | xargs clang-format -i
