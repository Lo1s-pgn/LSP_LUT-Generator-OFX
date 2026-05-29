set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_OBJCXX_STANDARD 20)
set(CMAKE_OBJCXX_STANDARD_REQUIRED ON)

set(LUTGEN_BUILD_ROOT "${CMAKE_SOURCE_DIR}/build/macos" CACHE PATH "macOS build intermediates")
set(LUTGEN_DIST_PLATFORM_DIR "${CMAKE_SOURCE_DIR}/release/${LUTGEN_RELEASE_FOLDER_MACOS}")
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

set(LUTGEN_PLATFORM_DIALOG_SRC "${CMAKE_SOURCE_DIR}/plugin/macos/LSPLutGeneratorDialogs.mm")
set(LUTGEN_PLATFORM_INCLUDE_DIRS "${CMAKE_SOURCE_DIR}/plugin/macos")

set(LUTGEN_COMPILE_OPTIONS
  -O2
  -Wno-dynamic-exception-spec
  -fvisibility=hidden
)
set(LUTGEN_COMPILE_DEFINITIONS "")
set(LUTGEN_LINK_LIBS
  "-framework" "Foundation"
  "-framework" "AppKit"
)
set(LUTGEN_LINK_OPTIONS
  "-bundle"
  "-fvisibility=hidden"
  "-Wl,-rpath,@loader_path"
)

include("${CMAKE_CURRENT_LIST_DIR}/LutGenCommon.cmake")

add_dependencies(lutgen_ofx lutgen_gen_plist)

target_compile_options(lutgen_ofx PRIVATE
  "$<$<COMPILE_LANGUAGE:OBJCXX>:-fblocks>"
)

add_custom_command(TARGET lutgen_ofx POST_BUILD
  COMMAND strip -x "$<TARGET_FILE:lutgen_ofx>"
  COMMENT "strip -x lutgen_ofx"
)

lutgen_assemble_bundle_postbuild("MacOS")

message(STATUS "[LutGen] macOS bundle: ${LUTGEN_OFX_BUNDLE}")
if(LUTGEN_OFX_FAT_ARCHS)
  message(STATUS "[LutGen] macOS architectures: ${CMAKE_OSX_ARCHITECTURES}")
else()
  message(STATUS "[LutGen] macOS architectures: host default")
endif()
