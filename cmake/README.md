# CMake Directory

This directory contains CMake modules, utilities, and configuration files for the Thales project. It helps organize build-related files and provides reusable components.

## Structure

- `modules/` - Find modules and utility functions
- `toolchain/` - Configuration for cross-compilation (if needed)
- `packaging/` - Packaging configuration files for distribution
- `thales-config.cmake.in` - Template for generating the Thales CMake package config file

## Modules

### FindLibPQXX.cmake

A custom find module for the libpqxx PostgreSQL C++ client library. This module provides more robust discovery than the basic pkg-config approach.

Usage:
```cmake
find_package(LibPQXX REQUIRED)
target_link_libraries(your_target ${LIBPQXX_LIBRARIES})
target_include_directories(your_target ${LIBPQXX_INCLUDE_DIRS})
```

### FindIBAPI.cmake

A custom find module for the Interactive Brokers API (TWS API). This helps locate the IBAPI headers and library in various possible locations.

Usage:
```cmake
find_package(IBAPI REQUIRED)
target_link_libraries(your_target ${IBAPI_LIBRARIES})
target_include_directories(your_target ${IBAPI_INCLUDE_DIRS})
```

### SetCompilerFlags.cmake

Centralizes compiler flag configuration for different compilers and build types. This makes it easier to maintain consistent flags across the project.

Usage:
```cmake
include(SetCompilerFlags)
set_compiler_flags()
```

## Packaging

### cpack_config.cmake

Configures CPack to create distribution packages. Supports various package formats depending on the target platform:
- Windows: NSIS installer, ZIP archive
- macOS: DMG, TGZ archive
- Linux: DEB, RPM, TGZ archive

Usage:
```cmake
include(cmake/packaging/cpack_config.cmake)
```

## Adding New Modules

To add a new CMake module:

1. Create a new .cmake file in the appropriate subdirectory
2. Document the module's purpose and usage at the top of the file
3. Implement the module's functionality
4. Update this README.md if necessary

## Best Practices

- Keep module files focused on a single responsibility
- Document modules with comments and examples
- Add proper error handling and version checking where appropriate
- Follow CMake naming conventions (e.g., use `find_package_handle_standard_args` for find modules)
