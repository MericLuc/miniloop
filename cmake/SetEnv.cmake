string(TOUPPER ${PROJECT_NAME} PROJECT_NAME_UPPERCASE)
string(TOLOWER ${PROJECT_NAME} PROJECT_NAME_LOWERCASE)

set(LIBRARY_NAME    "miniLoop")
set(LIBRARY_FOLDER  "miniLoop")
set(EXAMPLE_FOLDER  "${CMAKE_CURRENT_SOURCE_DIR}/example")

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
endif()

set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
             "Release" "Debug" "MinSizeRel" "RelWithDebInfo")

option(BUILD_SHARED_LIBS "Build ${LIBRARY_NAME} as a shared library." ON )
option(BUILD_EXAMPLE     "Build ${LIBRARY_NAME} usage example."       OFF)

message(STATUS "CMAKE_BUILD_TYPE : ${CMAKE_BUILD_TYPE}" )
message(STATUS "CMAKE_GENERATOR  : ${CMAKE_GENERATOR}"  )
message(STATUS "BUILD_SHARED_LIB : ${BUILD_SHARED_LIBS}")
message(STATUS "BUILD_EXAMPLE    : ${BUILD_EXAMPLE}"    )

if ( ${BUILD_SHARED_LIBS} )
  set(LIB_SUFFIX                        ".so"                         )
  set(CMAKE_SKIP_BUILD_RPATH            FALSE                         )
  set(CMAKE_BUILD_WITH_INSTALL_RPATH    FALSE                         )
  set(CMAKE_INSTALL_RPATH               "${CMAKE_INSTALL_PREFIX}/lib" )
  set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE                          )
  set(LIB_TYPE                          SHARED                        )

  list(FIND CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES "${CMAKE_INSTALL_PREFIX}/lib" isSystemDir)
  if("${isSystemDir}" STREQUAL "-1")
      set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
  endif()
else()
  set(LIB_SUFFIX ".a"   )
  set(LIB_TYPE   STATIC )

endif()

set(GENERATED_HEADERS_DIR
  "${CMAKE_CURRENT_BINARY_DIR}/generated_headers"
)

configure_file(
  "${PROJECT_SOURCE_DIR}/${LIBRARY_FOLDER}/version.h.in"
  "${GENERATED_HEADERS_DIR}/${LIBRARY_FOLDER}/version.h"
  @ONLY
)

include(GNUInstallDirs)

set(CONFIG_INSTALL_DIR "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}")

set(GENERATED_DIR       "${CMAKE_CURRENT_BINARY_DIR}/generated")
set(VERSION_CONFIG_FILE "${GENERATED_DIR}/${PROJECT_NAME}ConfigVersion.cmake")
set(PROJECT_CONFIG_FILE "${GENERATED_DIR}/${PROJECT_NAME}Config.cmake")
set(TARGETS_EXPORT_NAME "${PROJECT_NAME}Targets")

include(CMakePackageConfigHelpers)

write_basic_package_version_file(
    "${VERSION_CONFIG_FILE}"
    VERSION "${${PROJECT_NAME}_VERSION}"
    COMPATIBILITY SameMajorVersion
)

configure_package_config_file(
    "${PROJECT_SOURCE_DIR}/cmake/Config.cmake.in"
    "${PROJECT_CONFIG_FILE}"
      INSTALL_DESTINATION "${CONFIG_INSTALL_DIR}"
)

configure_file("${PROJECT_SOURCE_DIR}/cmake/Uninstall.cmake.in"
  "${GENERATED_DIR}/Uninstall.cmake"
  IMMEDIATE @ONLY)
add_custom_target(uninstall
  COMMAND ${CMAKE_COMMAND} -P ${GENERATED_DIR}/Uninstall.cmake)

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/CMakeRegistry.cmake)
