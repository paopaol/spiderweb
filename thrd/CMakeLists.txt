include(FetchContent)

# gtest
set(gtest_force_shared_crt
    ON
    CACHE INTERNAL "")
FetchContent_Declare(
  googletest
  URL file:///${CMAKE_CURRENT_SOURCE_DIR}/googletest-release-1.10.0.tar.gz)
FetchContent_MakeAvailable(googletest)
target_compile_options(gtest PRIVATE -Wno-error=maybe-uninitialized)

# benchmark
FetchContent_Declare(benchmark
                     URL file:///${CMAKE_CURRENT_SOURCE_DIR}/benchmark.tar.gz)
set(BENCHMARK_ENABLE_TESTING FALSE)
FetchContent_MakeAvailable(benchmark)

FetchContent_Declare(filesystem
                     URL file:///${CMAKE_CURRENT_SOURCE_DIR}/filesystem.tar.gz)
FetchContent_MakeAvailable(filesystem)

# fmt
FetchContent_Declare(fmt
                     URL file:///${CMAKE_CURRENT_SOURCE_DIR}/fmt-9.1.0.tar.gz)
FetchContent_MakeAvailable(fmt)

# spdlog
FetchContent_Declare(
  spdlog URL file:///${CMAKE_CURRENT_SOURCE_DIR}/spdlog-1.10.0.tar.gz)
set(SPDLOG_FMT_EXTERNAL ON)
FetchContent_MakeAvailable(spdlog)

# absl
FetchContent_Declare(
  absl
  URL https://github.com/abseil/abseil-cpp/archive/refs/tags/20220623.0.tar.gz)
FetchContent_MakeAvailable(absl)

# restinio(http)
FetchContent_Declare(http_parser
                     URL file:///${CMAKE_CURRENT_SOURCE_DIR}/http_parser.tar.gz)
FetchContent_MakeAvailable(http_parser)

FetchContent_Declare(
  restinio URL file:///${CMAKE_CURRENT_SOURCE_DIR}/restinio-0.6.16.tar.gz)
FetchContent_GetProperties(restinio)
if(NOT restinio_POPULATED)
  FetchContent_Populate(restinio)
  set(RESTINIO_FMT_HEADER_ONLY Off)
  add_subdirectory(${restinio_SOURCE_DIR}/dev ${restinio_BINARY_DIR}
                   EXCLUDE_FROM_ALL)
endif()
