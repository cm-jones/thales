cmake_minimum_required(VERSION 3.12)
project(thales VERSION 0.1.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

# Add option for Address Sanitizer
option(ENABLE_ASAN "Enable Address Sanitizer" OFF)

# Add option for code coverage
option(ENABLE_COVERAGE "Enable code coverage reporting" OFF)

# Use all available cores
set(CMAKE_BUILD_PARALLEL_LEVEL "")

# Configure Address Sanitizer
if(ENABLE_ASAN)
    message(STATUS "Building with Address Sanitizer")
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        set(ASAN_FLAGS "-fsanitize=address -fno-omit-frame-pointer")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${ASAN_FLAGS}")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${ASAN_FLAGS}")
        set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} ${ASAN_FLAGS}")
    else()
        message(WARNING "Address Sanitizer is only supported with GCC and Clang")
    endif()
endif()

# Configure code coverage
if(ENABLE_COVERAGE)
    message(STATUS "Building with code coverage")
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 --coverage -fprofile-arcs -ftest-coverage")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --coverage")
    else()
        message(WARNING "Code coverage is only supported with GCC and Clang")
    endif()
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

# Find PostgreSQL and libpqxx (optional)
find_package(PostgreSQL)
find_package(libpqxx)

# Define compile definitions based on available packages
if(PostgreSQL_FOUND AND libpqxx_FOUND)
    message(STATUS "PostgreSQL and libpqxx found, enabling database logging")
    add_compile_definitions(ENABLE_DB_LOGGER=1)
else()
    message(STATUS "PostgreSQL or libpqxx not found, database logging will be disabled")
    add_compile_definitions(ENABLE_DB_LOGGER=0)
endif()

# Find Interactive Brokers C++ API (optional)
find_path(IBAPI_INCLUDE_DIRS EClient.h
    PATHS 
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/ibapi
    /usr/include
    /usr/local/include
    NO_DEFAULT_PATH
)

if(IBAPI_INCLUDE_DIRS)
    message(STATUS "Interactive Brokers API found, enabling IB connectivity")
    add_compile_definitions(ENABLE_IB_CLIENT=1)
else()
    message(STATUS "Interactive Brokers API not found, IB connectivity will be disabled")
    add_compile_definitions(ENABLE_IB_CLIENT=0)
endif()

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party
)

if(IBAPI_INCLUDE_DIRS)
    include_directories(${IBAPI_INCLUDE_DIRS})
endif()

# Define source files
set(THALES_CORE_SOURCES
    src/core/engine.cpp
    src/core/portfolio.cpp
    src/core/position.cpp
    src/core/order.cpp
)

# Data sources
set(THALES_DATA_SOURCES
    src/data/data_manager.cpp
    src/data/ib_client.cpp
)

set(THALES_MODELS_SOURCES
    src/models/black_scholes_model.cpp
)

set(THALES_STRATEGIES_SOURCES
    src/strategies/strategy_base.cpp
    src/strategies/strategy_registry.cpp
    src/strategies/black_scholes.cpp
)

set(THALES_UTILS_SOURCES
    src/utils/logger.cpp
    src/utils/config.cpp
    src/utils/symbol_lookup.cpp
)

# Add database logger if PostgreSQL and libpqxx are available
if(PostgreSQL_FOUND AND libpqxx_FOUND)
    list(APPEND THALES_UTILS_SOURCES src/utils/db_logger.cpp)
endif()

# Combine all sources
set(THALES_SOURCES
    ${THALES_CORE_SOURCES}
    ${THALES_DATA_SOURCES}
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

# Link PostgreSQL and libpqxx if available
if(PostgreSQL_FOUND AND libpqxx_FOUND)
    target_include_directories(thales_lib PRIVATE ${PostgreSQL_INCLUDE_DIRS})
    target_include_directories(thales_lib PRIVATE ${LIBPQXX_INCLUDE_DIRS})
    target_link_libraries(thales_lib PRIVATE ${PostgreSQL_LIBRARIES})
    target_link_libraries(thales_lib PRIVATE ${LIBPQXX_LIBRARIES})
endif()

# Create the main executable
add_executable(thales src/main.cpp)
target_link_libraries(thales PRIVATE thales_lib)

# Tests
add_subdirectory(tests)

# Benchmarks
add_subdirectory(benchmarks)

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
message(STATUS "Thales Configuration:")
message(STATUS "  CMake version:       ${CMAKE_VERSION}")
message(STATUS "  C++ Compiler:        ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "  Build type:          ${CMAKE_BUILD_TYPE}")
message(STATUS "  Install prefix:      ${CMAKE_INSTALL_PREFIX}")
message(STATUS "  PostgreSQL support:  ${PostgreSQL_FOUND}")
message(STATUS "  libpqxx support:     ${libpqxx_FOUND}")
message(STATUS "  Address Sanitizer:   ${ENABLE_ASAN}")
message(STATUS "")
