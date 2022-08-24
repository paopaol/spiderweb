add_library(spiderweb)

target_sources(
  spiderweb
  PRIVATE core/spiderweb_eventsignal.h
          core/spiderweb_eventloop.cc
          core/spiderweb_eventloop.h
          core/spiderweb_object.cc
          core/spiderweb_object.h
          core/spiderweb_timer.cc
          core/spiderweb_timer.h
          arch/spiderweb_arch.hpp
          io/spiderweb_binary_writer.hpp
  PRIVATE core/internal/index_sequence.hpp)

target_include_directories(spiderweb PUBLIC asio-1.22.1/include
                                            ${CMAKE_CURRENT_SOURCE_DIR})
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
target_sources(
  spiderweb_test
  PRIVATE core/spiderweb_object_test.cc core/spiderweb_timer_test.cc
          core/spiderweb_eventloop_test.cc core/spiderweb_event_spy_test.cc
          io/spiderweb_binary_writer_test.cc)
target_link_libraries(spiderweb_test PRIVATE spiderweb gtest_main gtest gmock)

# benchmark
add_executable(spiderweb_benchmark)
target_sources(
  spiderweb_benchmark PRIVATE core/spiderweb_eventsignal_benchmark.cc
                              core/spiderweb_timer_benchmark.cc)
target_link_libraries(spiderweb_benchmark PRIVATE spiderweb benchmark
                                                  benchmark_main)
