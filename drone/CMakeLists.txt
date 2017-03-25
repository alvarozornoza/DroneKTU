cmake_minimum_required(VERSION 2.8)
project(drone)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread -g -O0")

if (COVERAGE)
  set(GCC_COVERAGE_COMPILE_FLAGS "-fprofile-arcs -ftest-coverage -fPIC")
  set(GCC_COVERAGE_LINK_FLAGS    "-lgcov")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS} ${COMMON_CXX_FLAGS} ${CMAKE_CXX_FLAGS_DEBUG}")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${GCC_COVERAGE_LINK_FLAGS}")
endif ()

include_directories(../osdk-core/inc)
include_directories(../osdk-wrapper/inc)
include_directories(../platform/linux/inc)
include_directories(inc)
include_directories(../libserial)

FILE(GLOB SOURCE_FILES inc/*.h src/*.cpp)
add_executable(${PROJECT_NAME} ${SOURCE_FILES})

add_dependencies(${PROJECT_NAME} djiosdk-wrapper)
add_dependencies(${PROJECT_NAME} LibSerial)

set(BLOCKING_LINK_LIBRARIES djiosdk-core djiosdk-platform djiosdk-wrapper)
set(SERIALPORT_LINK_LIBRARIES LibSerial)

target_link_libraries(${PROJECT_NAME} ${BLOCKING_LINK_LIBRARIES})
target_link_libraries(${PROJECT_NAME} ${SERIALPORT_LINK_LIBRARIES})

