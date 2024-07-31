add_executable(enum_reflect_test)
target_sources(enum_reflect_test PRIVATE enum_reflect_test.cc)
target_link_libraries(enum_reflect_test PRIVATE GTest::gtest_main GTest::gtest
                                                spiderweb)
# gtest_discover_tests(enum_reflect_test) if(IS_DEBUG)
# setup_target_for_coverage_lcov( NAME enum_reflect_test_coverage EXECUTABLE
# enum_reflect_test DEPENDENCIES enum_reflect_test) endif()
