# Shared lutgen_ofx target (included from LutGenApple.cmake / LutGenWindows.cmake).

set(OFX_SDK_PATH "${CMAKE_SOURCE_DIR}/openfx-sdk" CACHE PATH "Path to OFX SDK (include + Support)")

if(NOT EXISTS "${OFX_SDK_PATH}/include/ofxCore.h")
  message(FATAL_ERROR "OFX_SDK_PATH must contain include/ofxCore.h (got '${OFX_SDK_PATH}')")
endif()

set(LUTGEN_PLUGIN_CORE_SRCS
  "${CMAKE_SOURCE_DIR}/plugin/core/LSPLutGeneratorPlugin.cpp"
  "${CMAKE_SOURCE_DIR}/plugin/core/LSPLutGeneratorDescribe.cpp"
  "${CMAKE_SOURCE_DIR}/plugin/core/LSPLutGeneratorProcessor.cpp"
  "${CMAKE_SOURCE_DIR}/plugin/core/LSPLutGeneratorPattern.cpp"
  "${CMAKE_SOURCE_DIR}/plugin/core/LSPLutGeneratorCube.cpp"
)

set(LUTGEN_OFX_SUPPORT_SRCS
  "${OFX_SDK_PATH}/Support/Library/ofxsCore.cpp"
  "${OFX_SDK_PATH}/Support/Library/ofxsImageEffect.cpp"
  "${OFX_SDK_PATH}/Support/Library/ofxsInteract.cpp"
  "${OFX_SDK_PATH}/Support/Library/ofxsLog.cpp"
  "${OFX_SDK_PATH}/Support/Library/ofxsMultiThread.cpp"
  "${OFX_SDK_PATH}/Support/Library/ofxsParams.cpp"
  "${OFX_SDK_PATH}/Support/Library/ofxsProperty.cpp"
  "${OFX_SDK_PATH}/Support/Library/ofxsPropertyValidation.cpp"
)

set(LUTGEN_INCLUDE_DIRS
  "${CMAKE_SOURCE_DIR}/plugin"
  "${CMAKE_SOURCE_DIR}/plugin/core"
  "${OFX_SDK_PATH}/include"
  "${OFX_SDK_PATH}/Support/include"
  "${OFX_SDK_PATH}/Support/Library"
)

file(MAKE_DIRECTORY "${LUTGEN_BIN_DIR}")

add_library(lutgen_ofx MODULE
  ${LUTGEN_PLUGIN_CORE_SRCS}
  ${LUTGEN_OFX_SUPPORT_SRCS}
  ${LUTGEN_PLATFORM_DIALOG_SRC}
)

target_include_directories(lutgen_ofx PRIVATE ${LUTGEN_INCLUDE_DIRS} ${LUTGEN_PLATFORM_INCLUDE_DIRS})
target_compile_features(lutgen_ofx PRIVATE cxx_std_20)
target_compile_options(lutgen_ofx PRIVATE ${LUTGEN_COMPILE_OPTIONS})
target_compile_definitions(lutgen_ofx PRIVATE ${LUTGEN_COMPILE_DEFINITIONS})
if(LUTGEN_LINK_LIBS)
  target_link_libraries(lutgen_ofx PRIVATE ${LUTGEN_LINK_LIBS})
endif()
target_link_options(lutgen_ofx PRIVATE ${LUTGEN_LINK_OPTIONS})

set_target_properties(lutgen_ofx PROPERTIES
  PREFIX ""
  SUFFIX ".ofx"
  OUTPUT_NAME "${LUTGEN_OFX_BUNDLE_STEM}"
  LIBRARY_OUTPUT_DIRECTORY "${LUTGEN_BIN_DIR}"
)

add_dependencies(lutgen_ofx lutgen_gen_version)

if(EXISTS "${CMAKE_SOURCE_DIR}/ICON.png")
  set(LUTGEN_HAS_ICON TRUE)
else()
  set(LUTGEN_HAS_ICON FALSE)
endif()

function(lutgen_assemble_bundle_postbuild p_PlatformSubdir)
  set(_bundle_ofx "${LUTGEN_OFX_BUNDLE}/Contents/${p_PlatformSubdir}/${LUTGEN_OFX_EXECUTABLE_NAME}")
  add_custom_command(TARGET lutgen_ofx POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "${LUTGEN_DIST_PLATFORM_DIR}"
    COMMAND ${CMAKE_COMMAND} -E remove_directory "${LUTGEN_OFX_BUNDLE}/Contents"
    COMMAND ${CMAKE_COMMAND} -E make_directory "${LUTGEN_OFX_BUNDLE}/Contents/${p_PlatformSubdir}"
    COMMAND ${CMAKE_COMMAND} -E make_directory "${LUTGEN_OFX_BUNDLE}/Contents/Resources"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${LUTGEN_INFO_PLIST}" "${LUTGEN_OFX_BUNDLE}/Contents/Info.plist"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "$<TARGET_FILE:lutgen_ofx>" "${_bundle_ofx}"
    COMMENT "Assemble ${LUTGEN_OFX_BUNDLE_STEM}.ofx.bundle (${p_PlatformSubdir}) → ${LUTGEN_DIST_PLATFORM_DIR}"
  )
  if(LUTGEN_HAS_ICON)
    add_custom_command(TARGET lutgen_ofx POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy_if_different
              "${CMAKE_SOURCE_DIR}/ICON.png"
              "${LUTGEN_OFX_BUNDLE}/Contents/Resources/${LUTGEN_PLUGIN_ICON_NAME}"
    )
  endif()
endfunction()

add_custom_target(lutgen_all DEPENDS lutgen_ofx)
