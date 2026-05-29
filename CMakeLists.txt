# LSP - Simple LUT Generator OFX — unified CMake (macOS + Windows, CPU only)
cmake_minimum_required(VERSION 3.22)

option(LUTGEN_OFX_FAT_ARCHS "Link universal Mach-O (arm64 + x86_64)" ON)

# Universal macOS must be set before project() or CMake keeps a single host arch.
if(CMAKE_HOST_APPLE)
  set(LUTGEN_MACOSX_DEPLOYMENT_TARGET "11.0" CACHE STRING "Minimum macOS deployment target")
  set(CMAKE_OSX_DEPLOYMENT_TARGET "${LUTGEN_MACOSX_DEPLOYMENT_TARGET}" CACHE STRING "" FORCE)
  if(LUTGEN_OFX_FAT_ARCHS)
    set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64" CACHE STRING "macOS build architectures" FORCE)
  endif()
elseif(NOT CMAKE_HOST_WIN32)
  message(FATAL_ERROR "LSP Simple LUT Generator CMake supports macOS and Windows only.")
endif()

if(CMAKE_HOST_APPLE)
  project(LSP_Simple_LUT_Generator
    VERSION 0.0.0
    DESCRIPTION "LSP - Simple LUT Generator OFX"
    LANGUAGES C CXX OBJCXX
  )
elseif(CMAKE_HOST_WIN32)
  project(LSP_Simple_LUT_Generator
    VERSION 0.0.0
    DESCRIPTION "LSP - Simple LUT Generator OFX"
    LANGUAGES C CXX
  )
endif()

include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/LutGenVersion.cmake")

if(APPLE)
  include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/LutGenApple.cmake")
elseif(WIN32)
  include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/LutGenWindows.cmake")
endif()

if(EXISTS "${CMAKE_SOURCE_DIR}/scripts/archive_version.sh")
  add_custom_target(lutgen_archive
    COMMAND bash "${CMAKE_SOURCE_DIR}/scripts/archive_version.sh"
    WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    USES_TERMINAL
    COMMENT "Archive current version, bump VERSION, rebuild"
  )
endif()
