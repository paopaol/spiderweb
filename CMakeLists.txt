cmake_minimum_required(VERSION 3.10)

project(spiderweb)

set(CMAKE_EXPORT_COMPILE_COMMANDS 1)
set(CMAKE_CXX_STANDARD 14)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

add_subdirectory(thrd)

add_compile_options("-Wall" "-Werror" "-Wno-unused-function")

add_subdirectory(spiderweb)
# add_subdirectory(Qt)
