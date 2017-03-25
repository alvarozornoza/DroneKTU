cmake_minimum_required(VERSION 2.8)
project(DroneKTU)

set(CMAKE_VERBOSE_MAKEFILE OFF)

# Add module path for <name>.cmake conf files
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/contrib/)

include(${CMAKE_MODULE_PATH}/DJIConfig.cmake)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/libs)

add_subdirectory(osdk-core)
add_subdirectory(osdk-wrapper)
add_subdirectory(platform/linux)
add_subdirectory(drone)
add_subdirectory(libserial)

