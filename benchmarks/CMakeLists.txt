# Find Google Benchmark
find_package(benchmark QUIET)

# Use FetchContent as a fallback if Google Benchmark is not found
if(NOT benchmark_FOUND)
    message(STATUS "System Google Benchmark not found, fetching from source")
    include(FetchContent)
    FetchContent_Declare(
        googlebenchmark
        GIT_REPOSITORY https://github.com/google/benchmark.git
        GIT_TAG v1.8.3
    )
    # Disable benchmark tests
    set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googlebenchmark)
endif()

# Define benchmark sources
set(BENCHMARK_SOURCES
    benchmark_black_scholes_model.cpp
)

# Add benchmark executable
add_executable(thales_benchmarks ${BENCHMARK_SOURCES})

# Link against the library and Google Benchmark
target_link_libraries(thales_benchmarks PRIVATE thales_lib)

# Link against the appropriate Google Benchmark libraries
if(benchmark_FOUND)
    target_link_libraries(thales_benchmarks PRIVATE benchmark::benchmark)
else()
    target_link_libraries(thales_benchmarks PRIVATE benchmark)
endif()

# Add pthread if needed (usually required on Unix-like systems)
find_package(Threads REQUIRED)
target_link_libraries(thales_benchmarks PRIVATE Threads::Threads)
