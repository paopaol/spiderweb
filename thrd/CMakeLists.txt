include(FetchContent)

FetchContent_Declare(yyjson URL file://${CMAKE_CURRENT_SOURCE_DIR}/yyjson)
FetchContent_MakeAvailable(yyjson)

FetchContent_Declare(
  pugixml URL file://${PROJECT_SOURCE_DIR}/thrd/pugixml-1.12.1.tar.gz)
FetchContent_MakeAvailable(pugixml)

# restinio(date)
FetchContent_Declare(date URL file:///${CMAKE_CURRENT_SOURCE_DIR}/date.tar.gz)
set(BUILD_TZ_LIB
    ON
    CACHE BOOL "")
set(USE_SYSTEM_TZ_DB
    ON
    CACHE BOOL "")
FetchContent_MakeAvailable(date)

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
                     URL file:///${CMAKE_CURRENT_SOURCE_DIR}/fmt-6.1.2.tar.gz)
FetchContent_MakeAvailable(fmt)

# spdlog
FetchContent_Declare(
  spdlog URL file:///${CMAKE_CURRENT_SOURCE_DIR}/spdlog-1.5.0.tar.gz)
set(SPDLOG_FMT_EXTERNAL ON)
FetchContent_MakeAvailable(spdlog)

# absl
FetchContent_Declare(
  absl URL file:///${CMAKE_CURRENT_SOURCE_DIR}/abseil-cpp-20200225.2.tar.gz)
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
