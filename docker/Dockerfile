FROM ubuntu:22.04 as builder

# Install build dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    libboost-all-dev \
    libpq-dev \
    git \
    && rm -rf /var/lib/apt/lists/*

# Copy source code
WORKDIR /build
COPY . .

# Configure with performance flags
RUN mkdir -p build && cd build && \
    cmake -DCMAKE_BUILD_TYPE=Release \
          -DCMAKE_CXX_FLAGS="-O3 -march=native -ffast-math -flto" ..

# Compile with all cores
RUN cd build && make -j$(nproc)

# Create minimal runtime image
FROM ubuntu:22.04

# Install only runtime dependencies
RUN apt-get update && apt-get install -y \
    libboost-system1.74.0 \
    libboost-thread1.74.0 \
    libpq5 \
    postgresql-client \
    && rm -rf /var/lib/apt/lists/*

# Copy the compiled application and configs
COPY --from=builder /build/build/thales /usr/local/bin/
COPY config /etc/thales/config/

# Create entrypoint script
COPY docker/entrypoint.sh /entrypoint.sh
RUN chmod +x /entrypoint.sh

ENTRYPOINT ["/entrypoint.sh"]
