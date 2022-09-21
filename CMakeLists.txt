cmake_minimum_required(VERSION 3.10)

project(spiderweb)

set(CMAKE_EXPORT_COMPILE_COMMANDS 1)
set(CMAKE_CXX_STANDARD 11)

add_subdirectory(thrd)

add_subdirectory(spiderweb)
#add_subdirectory(Qt)
