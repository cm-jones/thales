#!/bin/bash

# Exit on error
set -e

# Function to check if a command exists
command_exists() {
    command -v "$1" >/dev/null 2>&1
}

# Function to install libpqxx on different platforms
install_libpqxx() {
    echo "Installing libpqxx..."
    
    if command_exists apt; then
        # Debian/Ubuntu
        sudo apt update
        sudo apt install -y libpqxx-dev
    elif command_exists yum; then
        # RHEL/CentOS/Fedora
        sudo yum install -y libpqxx-devel
    elif command_exists brew; then
        # macOS with Homebrew
        brew install libpqxx
    else
        # Build from source
        echo "Could not determine package manager. Building libpqxx from source..."
        install_from_source
    fi
    
    echo "libpqxx installed successfully."
}

# Function to build libpqxx from source
install_from_source() {
    echo "Building libpqxx from source..."
    
    # Check if git is installed
    if ! command_exists git; then
        echo "Error: git is required to build libpqxx from source."
        exit 1
    fi
    
    # Check if cmake is installed
    if ! command_exists cmake; then
        echo "Error: cmake is required to build libpqxx from source."
        exit 1
    fi
    
    # Create a temporary directory
    TEMP_DIR=$(mktemp -d)
    cd "$TEMP_DIR"
    
    # Clone the libpqxx repository
    git clone https://github.com/jtv/libpqxx.git
    cd libpqxx
    
    # Configure and build
    mkdir build
    cd build
    cmake ..
    make
    
    # Install
    sudo make install
    
    # Clean up
    cd
    rm -rf "$TEMP_DIR"
    
    echo "libpqxx built and installed from source."
}

# Main script

echo "Thales libpqxx Installation Script"
echo "=================================="

# Check if libpqxx is already installed
if pkg-config --exists libpqxx; then
    echo "libpqxx is already installed."
    LIBPQXX_VERSION=$(pkg-config --modversion libpqxx)
    echo "libpqxx version: $LIBPQXX_VERSION"
else
    install_libpqxx
fi

# Check if PostgreSQL is installed
if command_exists psql; then
    echo "PostgreSQL is already installed."
    POSTGRES_VERSION=$(psql --version | awk '{print $3}')
    echo "PostgreSQL version: $POSTGRES_VERSION"
else
    echo "Warning: PostgreSQL is not installed. Please run setup_postgres.sh to install PostgreSQL."
fi

echo "libpqxx installation completed successfully!"
echo "You can now build the Thales trading bot with PostgreSQL support."

exit 0
