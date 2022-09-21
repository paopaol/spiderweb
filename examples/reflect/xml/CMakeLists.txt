add_executable(xmlreader)
target_sources(xmlreader PRIVATE xmlreader.cc)
target_include_directories(xmlreader PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/..)
target_include_directories(xmlreader PRIVATE ${PROJECT_SOURCE_DIR}/include
                                             ${PROJECT_SOURCE_DIR}/implement)

target_link_libraries(xmlreader PRIVATE pugixml)
