cmake_minimum_required(VERSION 3.20)
project(UCompress)

set (CMAKE_CXX_STANDARD 14)

include_directories(${PROJECT_SOURCE_DIR}/common)

add_subdirectory(${PROJECT_SOURCE_DIR}/common)
add_subdirectory(${PROJECT_SOURCE_DIR}/coder)
add_subdirectory(${PROJECT_SOURCE_DIR}/decoder)
add_subdirectory(${PROJECT_SOURCE_DIR}/test)
add_subdirectory(${PROJECT_SOURCE_DIR}/JPEGEncoder)
add_subdirectory(${PROJECT_SOURCE_DIR}/JPEGDecoder)