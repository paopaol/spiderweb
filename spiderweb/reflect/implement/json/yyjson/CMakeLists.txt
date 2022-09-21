include(FetchContent)

# FetchContent_Declare(yyjson URL file://${PROJECT_SOURCE_DIR}/thrd/yyjson)
# FetchContent_MakeAvailable(yyjson)

add_executable(yy_impl_test)
target_sources(yy_impl_test PRIVATE yy_impl_test.cc)
target_link_libraries(yy_impl_test PRIVATE gtest_main gtest yyjson)
target_include_directories(yy_impl_test PRIVATE ${PROJECT_SOURCE_DIR}/include)
# gtest_discover_tests(yy_impl_test) if(IS_DEBUG)
# setup_target_for_coverage_lcov(NAME yy_impl_test_coverage EXECUTABLE
# yy_impl_test DEPENDENCIES yy_impl_test) endif()

add_executable(yy_impl_bench)
target_sources(yy_impl_bench PRIVATE yy_impl_bench.cc)
target_include_directories(yy_impl_bench PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(yy_impl_bench PRIVATE benchmark benchmark_main yyjson)
