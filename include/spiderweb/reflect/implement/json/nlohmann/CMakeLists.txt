add_executable(nl_impl_test)
target_sources(nl_impl_test PRIVATE nl_impl_test.cc)
target_link_libraries(nl_impl_test PRIVATE gtest_main gtest)
target_include_directories(nl_impl_test PRIVATE ${PROJECT_SOURCE_DIR}/include
                                                ${PROJECT_SOURCE_DIR}/thrd/json)
# gtest_discover_tests(nl_impl_test) if(IS_DEBUG)
# setup_target_for_coverage_lcov(NAME nl_impl_test_coverage EXECUTABLE
# nl_impl_test DEPENDENCIES nl_impl_test) endif()

add_executable(nl_impl_bench)
target_sources(nl_impl_bench PRIVATE nl_impl_bench.cc)
target_include_directories(
  nl_impl_bench PRIVATE ${PROJECT_SOURCE_DIR}/include
                        ${PROJECT_SOURCE_DIR}/thrd/json)
target_link_libraries(nl_impl_bench PRIVATE benchmark benchmark_main)
