set(CMAKE_AUTOMOC 1)
find_package(Qt5 REQUIRED Core Test)

add_executable(qt_impl_test)
target_sources(qt_impl_test PRIVATE qt_impl_test.cc)
target_link_libraries(qt_impl_test PRIVATE Qt5::Core gtest_main gtest)
target_include_directories(qt_impl_test PRIVATE ${PROJECT_SOURCE_DIR}/include)
# gtest_discover_tests(qt_impl_test) if(IS_DBEUG)
# setup_target_for_coverage_lcov(NAME qt_impl_test_coverage EXECUTABLE
# qt_impl_test DEPENDENCIES qt_impl_test) endif()

add_executable(qt_impl_bench)
target_sources(qt_impl_bench PRIVATE qt_impl_bench.cc)
target_include_directories(qt_impl_bench PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(qt_impl_bench PRIVATE Qt5::Core benchmark benchmark_main)
