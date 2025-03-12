cmake_minimum_required(VERSION 3.12)
project(thales VERSION 0.1.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

# Compiler flags
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")
endif()

# Enable testing
enable_testing()

# Find dependencies
find_package(Threads REQUIRED)

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party
)

# Define source files
set(THALES_CORE_SOURCES
    src/core/engine.cpp
)

set(THALES_MODELS_SOURCES
    src/models/black_scholes.cpp
)

set(THALES_STRATEGIES_SOURCES
    src/strategies/strategy_base.cpp
    src/strategies/strategy_registry.cpp
    src/strategies/black_scholes_arbitrage.cpp
)

set(THALES_UTILS_SOURCES
    src/utils/logger.cpp
    src/utils/config.cpp
)

# Combine all sources
set(THALES_SOURCES
    ${THALES_CORE_SOURCES}
    ${THALES_MODELS_SOURCES}
    ${THALES_STRATEGIES_SOURCES}
    ${THALES_UTILS_SOURCES}
)

# Create the library
add_library(thales_lib STATIC ${THALES_SOURCES})
target_include_directories(thales_lib PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)
target_link_libraries(thales_lib PRIVATE
    Threads::Threads
)

# Create the main executable
add_executable(thales src/main.cpp)
target_link_libraries(thales PRIVATE thales_lib)

# Tests
add_subdirectory(tests)

# Examples
add_executable(black_scholes_example examples/black_scholes_example.cpp)
target_link_libraries(black_scholes_example PRIVATE thales_lib)

# Install targets
install(TARGETS thales thales_lib
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)
install(DIRECTORY include/ DESTINATION include)

# Add a custom target for building documentation
add_custom_target(docs
    COMMAND echo "Documentation generation not yet configured"
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Generating documentation"
)

# Print configuration summary
message(STATUS "")
message(STATUS "Thales Options Trading Bot Configuration:")
message(STATUS "  CMake version:       ${CMAKE_VERSION}")
message(STATUS "  C++ Compiler:        ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "  Build type:          ${CMAKE_BUILD_TYPE}")
message(STATUS "  Install prefix:      ${CMAKE_INSTALL_PREFIX}")
message(STATUS "")
