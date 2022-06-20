add_library(spiderweb)

target_sources(
  spiderweb PRIVATE spiderweb_eventsignal.h spiderweb_eventloop.cc
                    spiderweb_eventloop.h spiderweb_timer.cc spiderweb_timer.h)
target_include_directories(spiderweb PUBLIC asio-1.22.1/include eventpp/include)
# we do not use any bosst components
target_compile_definitions(
  spiderweb
  PUBLIC ASIO_STANDALONE
         ASIO_HAS_STD_TYPE_TRAITS
         ASIO_HAS_STD_ARRAY
         ASIO_HAS_STD_ATOMIC
         ASIO_HAS_CSTDINT
         ASIO_HAS_STD_ADDRESSOF
         ASIO_HAS_STD_SHARED_PTR
         ASIO_HAS_STD_FUNCTION)

# unit test
add_executable(spiderweb_test)
target_sources(spiderweb_test PRIVATE spiderweb_eventloop_test.cc
                                      spiderweb_timer_test.cc)
target_link_libraries(spiderweb_test PRIVATE spiderweb gtest_main gtest)

# benchmark
add_executable(spiderweb_benchmark)
target_sources(spiderweb_benchmark PRIVATE spiderweb_eventsignal_benchmark.cc
                                           spiderweb_timer_benchmark.cc)
target_link_libraries(spiderweb_benchmark PRIVATE spiderweb benchmark
                                                  benchmark_main)
