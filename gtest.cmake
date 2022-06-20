include(FetchContent)

set(gtest_force_shared_crt ON CACHE INTERNAL "")

FetchContent_Declare(
  googletest
  URL ${CMAKE_CURRENT_SOURCE_DIR}/googletest-release-1.10.0.tar.gz
  URL_HASH MD5=ecd1fa65e7de707cd5c00bdac56022cd)

FetchContent_MakeAvailable(googletest)
