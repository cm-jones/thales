# Google Benchmark is included from third_party/benchmark

# Define benchmark sources
set(BENCHMARK_SOURCES
    benchmark_black_scholes_model.cpp
    benchmark_black_scholes_model_vectorized.cpp
)

# Add benchmark executable
add_executable(thales_benchmarks ${BENCHMARK_SOURCES})

# Link against the library and Google Benchmark
target_link_libraries(thales_benchmarks PRIVATE models core data strategies utils)

# Link against the Google Benchmark library
target_link_libraries(thales_benchmarks PRIVATE benchmark)

# Add pthread if needed (usually required on Unix-like systems)
find_package(Threads REQUIRED)
target_link_libraries(thales_benchmarks PRIVATE Threads::Threads)
