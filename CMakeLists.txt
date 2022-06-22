cmake_minimum_required(VERSION "3.16")
project(SPACE_GAME2D VERSION "0.0.0")

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/game")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/game")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/game")

add_compile_options("/std:c++17")

add_subdirectory("dep")
add_subdirectory("framework")
add_subdirectory("engine")
add_subdirectory("source")