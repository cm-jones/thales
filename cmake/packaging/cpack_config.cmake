# cpack_config.cmake
# Configuration for CPack to create distribution packages
#
# Usage:
#   include(cmake/packaging/cpack_config.cmake)

# Set package metadata
set(CPACK_PACKAGE_NAME "${PROJECT_NAME}")
set(CPACK_PACKAGE_VENDOR "Thales")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Thales - Quantitative Trading System")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "${PROJECT_NAME}")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
set(CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/README.md")

# Source package settings
set(CPACK_SOURCE_GENERATOR "TGZ;ZIP")
set(CPACK_SOURCE_IGNORE_FILES
  "\\\\.git/"
  "\\\\.gitignore"
  "build/"
  "CMakeFiles/"
  "CMakeCache.txt"
  "CPackConfig.cmake"
  "CPackSourceConfig.cmake"
  "_CPack_Packages/"
  "\\\\.DS_Store"
)

# Platform-specific settings
if(WIN32)
  # Windows settings
  set(CPACK_GENERATOR "NSIS;ZIP")
  set(CPACK_NSIS_INSTALL_ROOT "$PROGRAMFILES64")
  set(CPACK_NSIS_PACKAGE_NAME "${PROJECT_NAME}")
  set(CPACK_NSIS_MODIFY_PATH ON)
elseif(APPLE)
  # macOS settings
  set(CPACK_GENERATOR "DragNDrop;TGZ")
  set(CPACK_DMG_VOLUME_NAME "${PROJECT_NAME}")
  set(CPACK_DMG_FORMAT "UDBZ")
  set(CPACK_OSX_PACKAGE_VERSION "10.15")
else()
  # Linux settings
  set(CPACK_GENERATOR "TGZ;DEB;RPM")
  
  # DEB-specific settings
  set(CPACK_DEBIAN_PACKAGE_DEPENDS "libpq-dev, libboost-all-dev")
  set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Your Name <your.email@example.com>")
  set(CPACK_DEBIAN_PACKAGE_SECTION "devel")
  
  # RPM-specific settings
  set(CPACK_RPM_PACKAGE_RELEASE "1")
  set(CPACK_RPM_PACKAGE_LICENSE "MIT")
  set(CPACK_RPM_PACKAGE_GROUP "Development/Libraries")
  set(CPACK_RPM_PACKAGE_REQUIRES "postgresql-devel, boost-devel")
endif()

# Include CPack module to make the CPack commands available
include(CPack)
