cmake_minimum_required(VERSION 3.14)
project(thales_backtester)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include the Thales C++ headers
set(THALES_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../..)
include_directories(${THALES_ROOT}/include)

# Find Python and pybind11
find_package(PythonLibs REQUIRED)
find_package(pybind11 REQUIRED)

# Create the bindings module
pybind11_add_module(thales_cpp_bindings
    src/thales_backtester/bindings/bindings.cpp
    src/thales_backtester/bindings/models_bindings.cpp
    src/thales_backtester/bindings/strategies_bindings.cpp
    
    # Include actual Thales C++ sources
    ${THALES_ROOT}/src/models/black_scholes_model.cpp
    ${THALES_ROOT}/src/strategies/strategy_base.cpp
    ${THALES_ROOT}/src/strategies/black_scholes_arbitrage.cpp
    # Add more source files as needed
)

# Link against necessary Thales libraries
target_link_libraries(thales_cpp_bindings PRIVATE
    ${PYTHON_LIBRARIES}
)

# Include directories for the target
target_include_directories(thales_cpp_bindings PRIVATE
    ${PYTHON_INCLUDE_DIRS}
    ${pybind11_INCLUDE_DIRS}
    ${THALES_ROOT}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Install the bindings
install(TARGETS thales_cpp_bindings
    LIBRARY DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/src/thales_backtester
)
