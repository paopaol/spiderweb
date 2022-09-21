include(FetchContent)

# FetchContent_Declare(pugixml URL file://${PROJECT_SOURCE_DIR}/thrd/pugixml)
# FetchContent_MakeAvailable(pugixml)

add_executable(pugixml_impl_test)
target_sources(pugixml_impl_test PRIVATE pugixml_impl_test.cc)
target_include_directories(pugixml_impl_test
                           PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(pugixml_impl_test PRIVATE gtest gtest_main pugixml)
# gtest_discover_tests(pugixml_impl_test)
