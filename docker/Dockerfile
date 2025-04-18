# Define ARG before FROM (proper syntax)
ARG UBUNTU_VERSION=22.04
# Specify platform
FROM --platform=linux/amd64 ubuntu:${UBUNTU_VERSION} AS builder

# Install build dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    pkg-config \
    python3 \
    python3-pip \
    && rm -rf /var/lib/apt/lists/*

# Install Conan package manager
RUN pip3 install conan==1.59.0 && \
    conan profile new --detect default && \
    conan profile update settings.compiler.libcxx=libstdc++11 default

# Copy source code
WORKDIR /build
COPY . .

# Initialize any submodules in third_party
RUN git submodule update --init --recursive

# Ensure no CMake cache issues by cleaning up any potential cache files
RUN find . -name "CMakeCache.txt" -delete && \
    find . -name "CMakeFiles" -type d -exec rm -rf {} +

# Set environment variables for optimization
ENV CXXFLAGS="-O3 -march=native -ffast-math -flto"
ENV CFLAGS="-O3 -march=native -ffast-math -flto"

# Debug: show what's in the directory
RUN ls -la

# Step by step Conan build with verbose output
RUN mkdir -p build && cd build && \
    echo "Running conan install..." && \
    conan install .. --build=missing -v && \
    echo "Running conan build..." && \
    conan build .. -v

# Create minimal runtime image
FROM --platform=linux/amd64 ubuntu:${UBUNTU_VERSION}

# Install runtime dependencies (minimal set needed to run the application)
RUN apt-get update && apt-get install -y \
    libboost-system1.74.0 \
    libboost-filesystem1.74.0 \
    libboost-thread1.74.0 \
    libboost-container1.74.0 \
    libpq5 \
    postgresql-client \
    && rm -rf /var/lib/apt/lists/*

# Copy the compiled application and configs
COPY --from=builder /build/build/bin/thales /usr/local/bin/
COPY config /etc/thales/config/

# Create necessary directories
RUN mkdir -p /usr/local/thales/logs /usr/local/thales/data

# Create entrypoint script
COPY docker/entrypoint.sh /entrypoint.sh
RUN chmod +x /entrypoint.sh

ENTRYPOINT ["/entrypoint.sh"]
