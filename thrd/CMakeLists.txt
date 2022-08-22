include(FetchContent)

# gtest
set(gtest_force_shared_crt
    ON
    CACHE INTERNAL "")
FetchContent_Declare(
  googletest URL file:///${CMAKE_CURRENT_SOURCE_DIR}/googletest-release-1.10.0)

FetchContent_MakeAvailable(googletest)
target_compile_options(gtest PRIVATE -Wno-error=maybe-uninitialized)

# benchmark
FetchContent_Declare(benchmark
                     URL file:///${CMAKE_CURRENT_SOURCE_DIR}/benchmark)
set(BENCHMARK_ENABLE_TESTING FALSE)
FetchContent_MakeAvailable(benchmark)
