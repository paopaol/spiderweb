cmake_minimum_required(VERSION 3.10)

project(spiderweb)

set(CMAKE_EXPORT_COMPILE_COMMANDS 1)
set(CMAKE_CXX_STANDARD 11)

include(gtest.cmake)
set(BENCHMARK_ENABLE_TESTING FALSE)
add_subdirectory(benchmark)

add_subdirectory(spiderweb)
add_subdirectory(Qt)
