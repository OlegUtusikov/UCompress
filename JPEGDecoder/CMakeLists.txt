cmake_minimum_required(VERSION 3.20)
project(UCompress)

set (CMAKE_CXX_STANDARD 14)
set(GCC_COVERAGE_COMPILE_FLAGS "-O2")

add_executable(JPegUDecoder main.cpp)

set(COMMON_DIR ${CMAKE_SOURCE_DIR}/common)
link_directories(${COMMON_DIR})
target_link_libraries(JPegUDecoder COMMON_LIB)