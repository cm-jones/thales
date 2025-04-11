cmake_minimum_required(VERSION 3.14)
project(thales VERSION 0.1.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Find required packages
find_package(PkgConfig REQUIRED)
find_package(Boost 1.74 REQUIRED COMPONENTS container)
find_package(yaml-cpp 0.7.0 REQUIRED)
pkg_check_modules(LIBPQXX REQUIRED libpqxx)

# Include directories
include_directories(include)
include_directories(third_party)

# Add subdirectories
add_subdirectory(third_party/ibapi)
add_subdirectory(third_party/benchmark)

# Define source files
file(GLOB_RECURSE CORE_SOURCES "src/core/*.cpp")
file(GLOB_RECURSE DATA_SOURCES "src/data/*.cpp")
file(GLOB_RECURSE MODELS_SOURCES "src/models/*.cpp")
file(GLOB_RECURSE STRATEGIES_SOURCES "src/strategies/*.cpp")
file(GLOB_RECURSE UTILS_SOURCES "src/utils/*.cpp")

# Define utility library
add_library(utils STATIC ${UTILS_SOURCES})
target_include_directories(utils PUBLIC ${LIBPQXX_INCLUDE_DIRS})
target_link_libraries(utils PUBLIC ${LIBPQXX_LIBRARIES} yaml-cpp::yaml-cpp)

# Define core library
add_library(core STATIC ${CORE_SOURCES})
target_link_libraries(core PUBLIC utils Boost::container)

# Define data library
add_library(data STATIC ${DATA_SOURCES})
target_link_libraries(data PUBLIC utils twsapi)

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

# Add tests
add_subdirectory(tests)

# Add benchmarks
add_subdirectory(benchmarks)

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

# Export targets
install(EXPORT thales-targets
        FILE thales-targets.cmake
        NAMESPACE thales::
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/thales)

# Create and install config-file package
include(CMakePackageConfigHelpers)
configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/config/thales-config.cmake.in
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
