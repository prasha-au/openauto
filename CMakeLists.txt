cmake_minimum_required(VERSION 3.5.1)

set (openauto_VERSION_MAJOR 2)
set (openauto_VERSION_MINOR 1)
set (openauto_VERSION_PATCH 0)

project(openauto
VERSION ${openauto_VERSION_MAJOR}.${openauto_VERSION_MINOR}.${openauto_VERSION_PATCH}
LANGUAGES CXX)

find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "${CCACHE_PROGRAM}")
endif()

set(base_directory ${CMAKE_CURRENT_SOURCE_DIR})
set(CMAKE_BUILD_RPATH_USE_ORIGIN TRUE)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/assets)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/assets)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/assets)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/assets)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake_modules/")
SET(CMAKE_CXX_STANDARD 14)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS_INIT} -Wall -pedantic")
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
set(CMAKE_CXX_FLAGS_RELEASE "-g -O3")

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-psabi")
endif()

set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
add_definitions(-DBOOST_ALL_DYN_LINK)
find_package(Boost REQUIRED COMPONENTS system log OPTIONAL_COMPONENTS unit_test_framework)

find_package(libusb-1.0 REQUIRED)
find_package(Qt5 COMPONENTS Multimedia MultimediaWidgets Bluetooth Qml Quick QuickWidgets REQUIRED)
find_package(Protobuf REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(aasdk REQUIRED)
find_package(Threads)

find_package(GObject)
find_package(Qt5GStreamer)
find_package(PkgConfig REQUIRED)
pkg_check_modules(GST REQUIRED
                  gstreamer-1.0>=1.4
                  gstreamer-sdp-1.0>=1.4
                  gstreamer-video-1.0>=1.4
                  gstreamer-app-1.0>=1.4)
message(STATUS "${GST_LIBRARIES}")


add_subdirectory(extra_protos)
set(EXTRA_PROTOS_INCLUDE_DIR ${CMAKE_CURRENT_BINARY_DIR})
include_directories(${EXTRA_PROTOS_INCLUDE_DIR})

add_subdirectory(openauto)
add_subdirectory(autoapp)
add_dependencies(autoapp extra_protos)
