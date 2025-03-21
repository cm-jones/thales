#!/bin/bash
# Script to download and install Interactive Brokers C++ API

set -e

# Create the third_party/ibapi directory
mkdir -p third_party/ibapi

# Set the IB API version - you can update this to the latest version
IB_API_VERSION="10.19.03"

# Determine the OS type
if [[ "$OSTYPE" == "linux-gnu"* ]]; then
    OS_TYPE="linux"
elif [[ "$OSTYPE" == "darwin"* ]]; then
    OS_TYPE="macos"
else
    echo "Unsupported OS: $OSTYPE"
    exit 1
fi

# Download URL
DOWNLOAD_URL="https://github.com/Interactive-Brokers/tws-api/archive/refs/tags/v${IB_API_VERSION}.tar.gz"
TMP_DIR=$(mktemp -d)

echo "Downloading IB API version ${IB_API_VERSION}..."
curl -L -o "${TMP_DIR}/ibapi.tar.gz" "${DOWNLOAD_URL}"

echo "Extracting files..."
tar -xzf "${TMP_DIR}/ibapi.tar.gz" -C "${TMP_DIR}"

IB_SOURCE_DIR="${TMP_DIR}/tws-api-${IB_API_VERSION}/source/cppclient/client"

echo "Copying C++ API files to third_party/ibapi..."
cp -r "${IB_SOURCE_DIR}/"* "third_party/ibapi/"

# Clean up temporary directory
rm -rf "${TMP_DIR}"

echo "IB API installed successfully in third_party/ibapi/"
echo "Now you can build your project with IB API support"