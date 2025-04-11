# SetCompilerFlags.cmake
# Sets compiler flags based on build type and compiler
#
# Usage:
#   include(SetCompilerFlags)
#   set_compiler_flags()

# Function to set appropriate compiler flags for different build types
function(set_compiler_flags)
  # Common C++ flags
  set(CMAKE_CXX_STANDARD 23 PARENT_SCOPE)
  set(CMAKE_CXX_STANDARD_REQUIRED ON PARENT_SCOPE)
  set(CMAKE_CXX_EXTENSIONS OFF PARENT_SCOPE)
  
  # Determine compiler type
  if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    set(COMPILER_GNU TRUE)
  elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set(COMPILER_CLANG TRUE)
  elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    set(COMPILER_MSVC TRUE)
  endif()
  
  # Set flags based on compiler and build type
  set(CXX_FLAGS_COMMON "")
  
  # Common flags for all compilers
  if(COMPILER_GNU OR COMPILER_CLANG)
    list(APPEND CXX_FLAGS_COMMON 
      -Wall 
      -Wextra 
      -Wpedantic 
      -Werror=return-type
    )
    
    # Add debug flags
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -g" PARENT_SCOPE)
    
    # Add release flags
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3" PARENT_SCOPE)
    
    # Add relwithdebinfo flags (optimized but with debug symbols)
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} -O2 -g" PARENT_SCOPE)
    
  elseif(COMPILER_MSVC)
    list(APPEND CXX_FLAGS_COMMON 
      /W4 
      /WX 
      /MP     # Enable multi-processor compilation
      /EHsc   # Standard C++ exception handling
    )
    
    # MSVC-specific flags
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Od /Zi" PARENT_SCOPE)
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /O2 /GL" PARENT_SCOPE)
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} /O2 /Zi" PARENT_SCOPE)
  endif()
  
  # Convert list to string
  string(REPLACE ";" " " CXX_FLAGS_STR "${CXX_FLAGS_COMMON}")
  
  # Set the common flags
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CXX_FLAGS_STR}" PARENT_SCOPE)
  
  # Add sanitizer options for Debug builds when using Clang or GCC
  option(ENABLE_ASAN "Enable AddressSanitizer" OFF)
  if(ENABLE_ASAN AND (COMPILER_GNU OR COMPILER_CLANG))
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address -fno-omit-frame-pointer" PARENT_SCOPE)
    set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} -fsanitize=address" PARENT_SCOPE)
  endif()
endfunction()
