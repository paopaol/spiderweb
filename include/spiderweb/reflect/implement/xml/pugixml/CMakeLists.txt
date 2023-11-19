include(FetchContent)

add_executable(pugixml_impl_test)
target_sources(pugixml_impl_test PRIVATE pugixml_impl_test.cc)
target_link_libraries(pugixml_impl_test PRIVATE gtest gtest_main spiderweb)
# gtest_discover_tests(pugixml_impl_test)
