# FindLibPQXX.cmake
# Finds the libpqxx library
#
# This module defines:
#  LIBPQXX_FOUND        - True if libpqxx is found
#  LIBPQXX_INCLUDE_DIRS - Include directories for libpqxx
#  LIBPQXX_LIBRARIES    - Libraries to link against libpqxx
#  LIBPQXX_VERSION      - Version of libpqxx found

# Try pkg-config first (as it's the most common way to find libpqxx)
find_package(PkgConfig QUIET)
if(PKG_CONFIG_FOUND)
  pkg_check_modules(PC_LIBPQXX QUIET libpqxx)
endif()

# Find the include directory
find_path(LIBPQXX_INCLUDE_DIR
  NAMES pqxx/pqxx
  PATHS
    ${PC_LIBPQXX_INCLUDEDIR}
    ${PC_LIBPQXX_INCLUDE_DIRS}
    /usr/include
    /usr/local/include
  PATH_SUFFIXES libpqxx
)

# Find the library
find_library(LIBPQXX_LIBRARY
  NAMES pqxx
  PATHS
    ${PC_LIBPQXX_LIBDIR}
    ${PC_LIBPQXX_LIBRARY_DIRS}
    /usr/lib
    /usr/local/lib
)

# Handle version
if(PC_LIBPQXX_VERSION)
  set(LIBPQXX_VERSION ${PC_LIBPQXX_VERSION})
else()
  # Try to extract version from the pqxx/version.hxx header
  if(LIBPQXX_INCLUDE_DIR)
    if(EXISTS "${LIBPQXX_INCLUDE_DIR}/pqxx/version.hxx")
      file(STRINGS "${LIBPQXX_INCLUDE_DIR}/pqxx/version.hxx" libpqxx_version_str 
           REGEX "^#define[ \t]+PQXX_VERSION[ \t]+\"[^\"]+\"$")
      string(REGEX REPLACE "^#define[ \t]+PQXX_VERSION[ \t]+\"([^\"]+)\"$" "\\1" 
             LIBPQXX_VERSION "${libpqxx_version_str}")
    endif()
  endif()
endif()

# Handle find_package arguments
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(LibPQXX
  REQUIRED_VARS LIBPQXX_LIBRARY LIBPQXX_INCLUDE_DIR
  VERSION_VAR LIBPQXX_VERSION
)

# Set output variables
if(LIBPQXX_FOUND)
  set(LIBPQXX_LIBRARIES ${LIBPQXX_LIBRARY})
  set(LIBPQXX_INCLUDE_DIRS ${LIBPQXX_INCLUDE_DIR})
endif()

# Don't show these in the GUI
mark_as_advanced(
  LIBPQXX_INCLUDE_DIR
  LIBPQXX_LIBRARY
)
