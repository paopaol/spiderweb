cmake_minimum_required(VERSION 3.10)

project(spiderweb)

set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 14)
endif()

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

option(SPIDERWEB_INSTALL "use find_package for absl" ON)
option(SPIDERWEB_ABSL_USE_PACKAGE "use find_package for absl" ON)
option(SPIDERWEB_PUGIXML_USE_PACKAGE "use find_package for pugixml" ON)
option(SPIDERWEB_GTEST_USE_PACKAGE "use find_package for gtest" ON)
option(SPIDERWEB_YYJSON_USE_PACKAGE "use find_package for yyjson" ON)
option(SPIDERWEB_DATE_USE_PACKAGE "use find_package for date" ON)
option(SPIDERWEB_GHC_USE_PACKAGE "use find_package for ghc_filesystem" ON)
option(SPIDERWEB_BENCHMARK_USE_PACKAGE "use find_package for benchmark" ON)
option(SPIDERWEB_SPDLOG_USE_PACKAGE "use find_package for spdlog" ON)
option(SPIDERWEB_ASIO_USE_PACKAGE "use find_package for asio" ON)

if(SPIDERWEB_ABSL_USE_PACKAGE)
  find_package(absl REQUIRED)
endif()

if(SPIDERWEB_PUGIXML_USE_PACKAGE)
  find_package(pugixml REQUIRED)
endif()

if(SPIDERWEB_YYJSON_USE_PACKAGE)
  find_package(yyjson REQUIRED)
endif()

find_package(PkgConfig REQUIRED)
if(SPIDERWEB_DATE_USE_PACKAGE)
  find_package(date REQUIRED)
endif()

if(SPIDERWEB_GTEST_USE_PACKAGE)
  find_package(GTest REQUIRED)
endif()
if(SPIDERWEB_GHC_USE_PACKAGE)
  find_package(ghc_filesystem REQUIRED)
endif()

if(SPIDERWEB_BENCHMARK_USE_PACKAGE)
  find_package(benchmark REQUIRED)
endif()
if(SPIDERWEB_SPDLOG_USE_PACKAGE)
  find_package(spdlog REQUIRED)
endif()

if(SPIDERWEB_ASIO_USE_PACKAGE)
  pkg_check_modules(asio REQUIRED)
endif()

if(NOT MSVC AND SPIDERWEB_ASIO_USE_PACKAGE)
  find_package(canary REQUIRED)
endif()

if(NOT WIN32)
  add_compile_options("-Wall" "-Werror" "-Wno-unused-function")
endif()

set(CMAKE_INSTALL_RPATH ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR})
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS 1)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address ")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=address ")
endif()

add_subdirectory(spiderweb)

# ------------------------------------------------------------------------------
# Install
if(SPIDERWEB_INSTALL)
  include(GNUInstallDirs)
  install(
    TARGETS spiderweb
    EXPORT spiderweb-targets
    ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
    INCLUDES
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")
  install(
    EXPORT spiderweb-targets
    FILE spiderweb-config.cmake
    NAMESPACE spiderweb::
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/spiderweb")
  install(DIRECTORY include/spiderweb DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")
endif()
