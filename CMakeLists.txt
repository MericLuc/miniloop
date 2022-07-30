project(miniloop)

cmake_minimum_required(VERSION 3.10)

set(SYSLOGGER_BRIEF "Lightweight cpp wrapper around libevent")

set(INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/${PROJECT_NAME} )
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_definitions("-Wall -g -O3")

# Directories are created with 0755 permissions
set(CMAKE_INSTALL_DEFAULT_DIRECTORY_PERMISSIONS
    OWNER_READ
    OWNER_WRITE
    OWNER_EXECUTE
    GROUP_READ
    GROUP_EXECUTE
    WORLD_READ
    WORLD_EXECUTE
)

add_library(
  ${PROJECT_NAME}
  STATIC
  Loop.cpp
  AlarmClock.cpp
  Timeout.cpp
  UnixSignal.cpp
  IO.cpp
)

find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBEVENT_CORE REQUIRED libevent_core)

target_include_directories(${PROJECT_NAME}
    PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:${INSTALL_DIR}/include>
)

set(
  INTERFACE_HEADERS
  Loop.h
)
set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)
set_target_properties(${PROJECT_NAME} PROPERTIES PUBLIC_HEADER "${INTERFACE_HEADERS}")
set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "${PROJECT_NAME}")
set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".a")
set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "")

install( TARGETS "${PROJECT_NAME}"
   ARCHIVE DESTINATION "${INSTALL_DIR}/lib"
   COMPONENT library
   PUBLIC_HEADER DESTINATION "${INSTALL_DIR}/include"
)
