include(FetchContent)

add_library(pugixml_reflect_impl)
target_sources(pugixml_reflect_impl PRIVATE pugixml_impl.cc)
target_link_libraries(pugixml_reflect_impl PRIVATE pugixml)
target_include_directories(pugixml_reflect_impl
                           PRIVATE ${PROJECT_SOURCE_DIR}/include)

add_executable(pugixml_impl_test)
target_sources(pugixml_impl_test PRIVATE pugixml_impl_test.cc)
target_link_libraries(pugixml_impl_test PRIVATE pugixml_reflect_impl spiderweb
                                                gtest gtest_main spiderweb)
# gtest_discover_tests(pugixml_impl_test)
