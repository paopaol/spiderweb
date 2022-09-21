find_package(Qt5 REQUIRED Core)

set(CMAKE_AUTOMOC 1)

add_subdirectory(EventLoopEPoll)

add_executable(Qt5Benchmark)
target_sources(Qt5Benchmark PUBLIC connect.cc)
target_link_libraries(Qt5Benchmark PRIVATE Qt5::Core benchmark benchmark_main
                                           Cutelyst2Qt5EventLoopEpoll)
