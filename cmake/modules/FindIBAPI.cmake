# FindIBAPI.cmake
# Finds the Interactive Brokers API (TWS API)
#
# This module defines:
#  IBAPI_FOUND        - True if IBAPI is found
#  IBAPI_INCLUDE_DIRS - Include directories for IBAPI
#  IBAPI_LIBRARIES    - Libraries to link against IBAPI
#  IBAPI_VERSION      - Version of IBAPI if available

# Look for the headers and protobuf files in various common locations
find_path(IBAPI_INCLUDE_DIR
  NAMES 
    EClient.h
    Contract.h
  PATHS
    ${IBAPI_ROOT_DIR}
    $ENV{IBAPI_ROOT_DIR}
    ${CMAKE_SOURCE_DIR}/third_party/ibapi
    /usr/include/ibapi
    /usr/local/include/ibapi
  PATH_SUFFIXES
    include
    client
    IBJts/source/cppclient/client
    tws-api/IBJts/source/cppclient/client
    tws-api/source/cppclient/client
    protobufUnix
)

# Also find protobuf headers directory
find_path(IBAPI_PROTOBUF_INCLUDE_DIR
  NAMES
    ExecutionFilter.pb.h
  PATHS
    ${IBAPI_ROOT_DIR}
    $ENV{IBAPI_ROOT_DIR}
    ${CMAKE_SOURCE_DIR}/third_party/ibapi
  PATH_SUFFIXES
    protobufUnix
)

# Look for the library
find_library(IBAPI_LIBRARY
  NAMES 
    twsapi
    tws
    ibapi
  PATHS
    ${IBAPI_ROOT_DIR}
    $ENV{IBAPI_ROOT_DIR}
    ${CMAKE_SOURCE_DIR}/third_party/ibapi
    /usr/lib
    /usr/local/lib
  PATH_SUFFIXES
    lib
    build
)

# Try to determine version from EClient.h
if(IBAPI_INCLUDE_DIR)
  if(EXISTS "${IBAPI_INCLUDE_DIR}/EClient.h")
    file(STRINGS "${IBAPI_INCLUDE_DIR}/EClient.h" IBAPI_VERSION_LINE
         REGEX "^[ \t]*static const int[a-zA-Z_ \t]+VERSION[ \t]*=[ \t]*[0-9]+;.*$")
    if(IBAPI_VERSION_LINE)
      string(REGEX REPLACE "^[ \t]*static const int[a-zA-Z_ \t]+VERSION[ \t]*=[ \t]*([0-9]+);.*$" "\\1"
             IBAPI_VERSION "${IBAPI_VERSION_LINE}")
    endif()
  endif()
endif()

# Handle find_package arguments
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(IBAPI
  REQUIRED_VARS IBAPI_INCLUDE_DIR
  VERSION_VAR IBAPI_VERSION
)

# Set output variables
if(IBAPI_FOUND)
  if(IBAPI_LIBRARY)
    set(IBAPI_LIBRARIES ${IBAPI_LIBRARY})
  else()
    # Handle the case where IBAPI is header-only or source-only
    set(IBAPI_LIBRARIES "")
  endif()
  set(IBAPI_INCLUDE_DIRS ${IBAPI_INCLUDE_DIR} ${IBAPI_PROTOBUF_INCLUDE_DIR})
endif()

# Set IBAPI_ROOT_DIR for future use
if(IBAPI_INCLUDE_DIR)
  get_filename_component(IBAPI_ROOT_DIR "${IBAPI_INCLUDE_DIR}" PATH)
  set(IBAPI_ROOT_DIR "${IBAPI_ROOT_DIR}" CACHE PATH "Root directory for IBAPI")
endif()

# Don't show these in the GUI
mark_as_advanced(
  IBAPI_INCLUDE_DIR
  IBAPI_LIBRARY
  IBAPI_ROOT_DIR
)
