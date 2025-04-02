#!/bin/bash
# Script to download and install Interactive Brokers C++ API

set -e

# Create the third_party/ibapi directory
mkdir -p third_party/ibapi

# Set the IB API version - you can update this to the latest version
IB_API_VERSION="1035.01"

# Determine the OS type
if [[ "$OSTYPE" == "linux-gnu"* || "$OSTYPE" == "darwin"* ]]; then
    # For Linux or macOS, use the macunix package
    DOWNLOAD_URL="https://interactivebrokers.github.io/downloads/twsapi_macunix.${IB_API_VERSION}.zip"
else
    echo "Unsupported OS: $OSTYPE"
    exit 1
fi

# Download URL
TMP_DIR=$(mktemp -d)

echo "Downloading IB API version ${IB_API_VERSION}..."
curl -L -o "${TMP_DIR}/ibapi.zip" "${DOWNLOAD_URL}"

echo "Extracting files..."
unzip -q "${TMP_DIR}/ibapi.zip" -d "${TMP_DIR}"

# The C++ API client files are in the IBJts/source/cppclient/client directory
IB_SOURCE_DIR="${TMP_DIR}/IBJts/source/cppclient/client"

echo "Copying C++ API files to third_party/ibapi..."
cp -r "${IB_SOURCE_DIR}/"* "third_party/ibapi/"

# Clean up temporary directory
rm -rf "${TMP_DIR}"

echo "IB API installed successfully in third_party/ibapi/"
echo "Now you can build your project with IB API support"
