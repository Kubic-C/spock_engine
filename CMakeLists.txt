# MIT License

# Copyright (c) 2023 Sawyer Porter

# Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

# The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

cmake_minimum_required(VERSION "3.16")
project(SPOCK_ENGINE VERSION "0.0.1")

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/game")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/lib")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/game")

message(STATUS "Spock Engine --\n")

if(NOT "${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static -static-libgcc -static-libstdc++ -std=c++17 -Wfatal-errors")

	if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg -no-pie -gdwarf-4 -O0")
        # set(SPK_DEBUG_LIBS profiler_static) couldn't get it to work on windows :(
	else()
        # --static: allows the outputted executable to be ran
        # on other machines without the C++/C Libraries being installed
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
    endif()
	
    message("In ${CMAKE_BUILD_TYPE}, in ${CMAKE_CXX_COMPILER_ID}")
    message("  C OPTIONS: ${CMAKE_C_FLAGS}")
    message("CXX OPTIONS: ${CMAKE_CXX_FLAGS}")
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    message(FATAL_ERROR "MSVC Build enviroments are not supported")
endif()

# compiler specific stuff

# MinGW
if(WIN32)
    get_filename_component(PARENT_DIR ${CMAKE_CXX_COMPILER} PATH)
    
    file(COPY "${PARENT_DIR}/zlib1.dll" DESTINATION "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/")
    file(COPY "${PARENT_DIR}/libbz2-1.dll" DESTINATION "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/")
    file(COPY "${PARENT_DIR}/libbrotlidec.dll" DESTINATION "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/")
    file(COPY "${PARENT_DIR}/libbrotlicommon.dll" DESTINATION "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/")
endif()

message("\n-- End --")

add_subdirectory("dep")
add_subdirectory("engine")
add_subdirectory("test_field")