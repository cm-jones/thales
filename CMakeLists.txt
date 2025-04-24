cmake_minimum_required(VERSION 3.14)
cmake_minimum_required(VERSION 3.14 FATAL_ERROR)
project(thales VERSION 0.1.0 LANGUAGES CXX)

# Add cmake modules directory to module path
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules")

# Set compiler flags using our custom module
include(SetCompilerFlags)
set_compiler_flags()

# Find required packages
find_package(PkgConfig REQUIRED)
find_package(Boost 1.74 REQUIRED COMPONENTS container)
find_package(LibPQXX REQUIRED)

# Optional: Find IBAPI (if available)
find_package(IBAPI QUIET)
if(IBAPI_FOUND)
  message(STATUS "Found IBAPI: ${IBAPI_VERSION}")
else()
  message(STATUS "IBAPI not found, IB-related features will be disabled")
endif()

# Include directories
include_directories(include)
include_directories(third_party)

# Add subdirectories (commented out as third_party dependencies are missing)
add_subdirectory(third_party/ibapi)
# add_subdirectory(third_party/benchmark)

# Check for Google Benchmark
find_package(benchmark QUIET)
if(NOT benchmark_FOUND)
    message(STATUS "Google Benchmark not found, benchmarks will be disabled")
endif()

# Define source files
file(GLOB_RECURSE CORE_SOURCES "src/core/*.cpp")
file(GLOB_RECURSE DATA_SOURCES "src/data/*.cpp")
file(GLOB_RECURSE MODELS_SOURCES "src/models/*.cpp")
file(GLOB_RECURSE STRATEGIES_SOURCES "src/strategies/*.cpp")
file(GLOB_RECURSE UTILS_SOURCES "src/utils/*.cpp")

# Define utility library
add_library(utils STATIC ${UTILS_SOURCES})
target_include_directories(utils PUBLIC ${LIBPQXX_INCLUDE_DIRS})
target_link_libraries(utils PUBLIC ${LIBPQXX_LIBRARIES})
# yaml-cpp is handled by Conan

# Define core library
add_library(core STATIC ${CORE_SOURCES})
target_link_libraries(core PUBLIC utils Boost::container)

# Define data library
add_library(data STATIC ${DATA_SOURCES})
target_link_libraries(data PUBLIC utils)
# twsapi is commented out because third_party/ibapi is not available

# Define models library
add_library(models STATIC ${MODELS_SOURCES})
target_link_libraries(models PUBLIC utils)

# Define strategies library
add_library(strategies STATIC ${STRATEGIES_SOURCES})
target_link_libraries(strategies PUBLIC core data models utils)

# Define main executable
add_executable(thales src/main.cpp)
target_link_libraries(thales 
    PRIVATE 
    core 
    data 
    models 
    strategies 
    utils
)

# Add tests (if GTest is available)
option(BUILD_TESTS "Build test programs" ON)
if(BUILD_TESTS)
    # Check for GTest
    find_package(GTest QUIET)
    if(NOT GTest_FOUND)
        # Fetch GTest as part of the build
        message(STATUS "System GTest not found, tests may be disabled")
        set(BUILD_TESTS OFF)
    else()
        add_subdirectory(tests)
    endif()
endif()

# Add benchmarks (only if Google Benchmark is found)
if(benchmark_FOUND)
    add_subdirectory(benchmarks)
else()
    message(STATUS "Skipping benchmarks as Google Benchmark wasn't found")
endif()

# Install targets
include(GNUInstallDirs)

# Install headers
install(DIRECTORY include/thales
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
        FILES_MATCHING PATTERN "*.hpp" PATTERN "*.h")

# Install libraries
install(TARGETS core data models strategies utils
        EXPORT thales-targets
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

# Install executable
install(TARGETS thales
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

# Install config files
install(FILES config/config.yaml
        DESTINATION ${CMAKE_INSTALL_DATADIR}/thales/config)

# Include packaging configuration
include(cmake/packaging/cpack_config.cmake)

# Export targets
install(EXPORT thales-targets
        FILE thales-targets.cmake
        NAMESPACE thales::
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/thales)

# Create and install config-file package
include(CMakePackageConfigHelpers)
configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/thales-config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/thales-config.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/thales
    PATH_VARS CMAKE_INSTALL_INCLUDEDIR CMAKE_INSTALL_LIBDIR
)
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/thales-config-version.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/thales-config.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/thales-config-version.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/thales
)
