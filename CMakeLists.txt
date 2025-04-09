cmake_minimum_required(VERSION 3.14)
project(thales VERSION 0.1.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Find required packages
find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBPQXX REQUIRED libpqxx)

# Find Protocol Buffers (optional)
find_package(Protobuf QUIET)
if(Protobuf_FOUND)
  include_directories(${Protobuf_INCLUDE_DIRS})
  add_definitions(-DHAS_PROTOBUF)
else()
  message(STATUS "Protocol Buffers not found - some features may be disabled")
  # Add define to disable protobuf features in IBAPI
  add_definitions(-DIBAPI_NO_PROTOBUF)
endif()

# Include directories
include_directories(include)
include_directories(third_party)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})  # To find proto/*.pb.h

# Add subdirectories
add_subdirectory(third_party/ibapi)

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

# Define core library
add_library(core STATIC ${CORE_SOURCES})
target_link_libraries(core PUBLIC utils)

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
