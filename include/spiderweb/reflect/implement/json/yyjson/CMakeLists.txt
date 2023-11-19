include(FetchContent)

add_executable(yy_impl_test)
target_sources(yy_impl_test PRIVATE yy_impl_test.cc)
target_link_libraries(yy_impl_test PRIVATE gtest_main gtest spiderweb)
# gtest_discover_tests(yy_impl_test) if(IS_DEBUG)
# setup_target_for_coverage_lcov(NAME yy_impl_test_coverage EXECUTABLE
# yy_impl_test DEPENDENCIES yy_impl_test) endif()

add_executable(yy_impl_bench)
target_sources(yy_impl_bench PRIVATE yy_impl_bench.cc)
target_link_libraries(yy_impl_bench PRIVATE benchmark benchmark_main yyjson
                                            spiderweb)
