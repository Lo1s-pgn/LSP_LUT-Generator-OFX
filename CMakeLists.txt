# LSP - Simple LUT Generator OFX — unified CMake (macOS + Windows, CPU only)
cmake_minimum_required(VERSION 3.22)

if(APPLE)
  project(LSP_Simple_LUT_Generator
    VERSION 0.0.0
    DESCRIPTION "LSP - Simple LUT Generator OFX"
    LANGUAGES C CXX OBJCXX
  )
elseif(WIN32)
  project(LSP_Simple_LUT_Generator
    VERSION 0.0.0
    DESCRIPTION "LSP - Simple LUT Generator OFX"
    LANGUAGES C CXX
  )
else()
  message(FATAL_ERROR "LSP Simple LUT Generator CMake supports macOS (APPLE) and Windows (WIN32) only.")
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
