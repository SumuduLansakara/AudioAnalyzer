cmake_minimum_required(VERSION 2.6)
project (aulog)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -Wall -pedantic -Wextra -Weffc++")
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

link_directories(${PROJECT_SOURCE_DIR}/deps/lib)
include_directories(${PROJECT_SOURCE_DIR}/deps/include)

add_subdirectory(src)
