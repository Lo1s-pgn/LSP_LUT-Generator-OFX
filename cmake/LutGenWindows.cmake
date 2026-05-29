set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(LUTGEN_BUILD_ROOT "${CMAKE_SOURCE_DIR}/build/windows" CACHE PATH "Windows build intermediates")
set(LUTGEN_DIST_PLATFORM_DIR "${CMAKE_SOURCE_DIR}/release/${LUTGEN_RELEASE_FOLDER_WINDOWS}")
set(LUTGEN_BIN_DIR "${LUTGEN_BUILD_ROOT}/bin")
set(LUTGEN_INFO_PLIST "${LUTGEN_BUILD_ROOT}/Info.plist")
set(LUTGEN_OFX_BUNDLE "${LUTGEN_DIST_PLATFORM_DIR}/${LUTGEN_OFX_BUNDLE_STEM}.ofx.bundle")

file(MAKE_DIRECTORY "${LUTGEN_BUILD_ROOT}")
configure_file(
  "${CMAKE_SOURCE_DIR}/Info.plist.in"
  "${LUTGEN_INFO_PLIST}"
  @ONLY
)
add_custom_target(lutgen_gen_plist DEPENDS "${LUTGEN_INFO_PLIST}")

set(LUTGEN_PLATFORM_DIALOG_SRC "${CMAKE_SOURCE_DIR}/plugin/windows/LSPLutGeneratorDialogs.cpp")
set(LUTGEN_PLATFORM_INCLUDE_DIRS "${CMAKE_SOURCE_DIR}/plugin/windows")

set(LUTGEN_COMPILE_OPTIONS /O2 /W3)
set(LUTGEN_COMPILE_DEFINITIONS
  WIN32_LEAN_AND_MEAN
  NOMINMAX
  _CRT_SECURE_NO_WARNINGS
)
set(LUTGEN_LINK_LIBS shell32 ole32 uuid comdlg32)
set(LUTGEN_LINK_OPTIONS "")

include("${CMAKE_CURRENT_LIST_DIR}/LutGenCommon.cmake")

add_dependencies(lutgen_ofx lutgen_gen_plist)

lutgen_assemble_bundle_postbuild("Win64")

message(STATUS "[LutGen] Windows bundle: ${LUTGEN_OFX_BUNDLE}")
