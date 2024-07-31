cmake_minimum_required(VERSION 3.10)

project(spiderweb)

set(CMAKE_EXPORT_COMPILE_COMMANDS 1)
set(CMAKE_CXX_STANDARD 14)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# add_subdirectory(thrd)

find_package(PkgConfig REQUIRED)
find_package(pugixml REQUIRED)
find_package(yyjson REQUIRED)
find_package(date REQUIRED)
find_package(GTest REQUIRED)
find_package(ghc_filesystem REQUIRED)
find_package(benchmark REQUIRED)
find_package(spdlog REQUIRED)
find_package(absl REQUIRED)
pkg_check_modules(asio REQUIRED)
find_package(canary REQUIRED)

add_compile_options("-Wall" "-Werror" "-Wno-unused-function")

add_subdirectory(spiderweb)
# add_subdirectory(Qt)
