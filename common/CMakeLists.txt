cmake_minimum_required(VERSION 3.20)
project(UCompress)

set (CMAKE_CXX_STANDARD 14)
set(GCC_COVERAGE_COMPILE_FLAGS "-O2")

file(GLOB COMMON_H *.h)
file(GLOB COMMON_SRC *.cpp)

add_library(COMMON_LIB OBJECT ${COMMON_H} ${COMMON_SRC})