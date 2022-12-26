cmake_minimum_required(VERSION "3.16")
project(SPOK_ENGINE VERSION "0.0.1")

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/game")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/lib")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/game")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -gdwarf-4 -O0 -std=c++17 -Wfatal-errors")
message(STATUS "CXX OPTIONS: ${CMAKE_CXX_FLAGS}")

add_subdirectory("dep")
add_subdirectory("engine")
add_subdirectory("test_field")