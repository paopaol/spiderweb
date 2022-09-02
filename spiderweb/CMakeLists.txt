add_library(spiderweb)

target_sources(
  spiderweb
  PRIVATE core/spiderweb_event_spy.h
          core/spiderweb_eventsignal.h
          core/spiderweb_eventloop.cc
          core/spiderweb_eventloop.h
          core/spiderweb_object.cc
          core/spiderweb_object.h
          core/spiderweb_timer.cc
          core/spiderweb_timer.h
          arch/spiderweb_arch.hpp
          io/spiderweb_binary_writer.hpp
          io/spiderweb_buffer.cc
          io/spiderweb_buffer.h
          net/spiderweb_tcp_socket.cc
          net/spiderweb_tcp_socket.h
          net/spiderweb_tcp_server.cc
          net/spiderweb_tcp_server.h
  PRIVATE core/internal/index_sequence.hpp
          core/internal/thread_check.h
          core/internal/move_tuple_wrapper.h
          net/private/spiderweb_tcp_socket_private.h
          net/private/spiderweb_tcp_server_private.h)

target_include_directories(
  spiderweb PUBLIC asio-1.22.1/include ${CMAKE_CURRENT_SOURCE_DIR}
                   ${PROJECT_SOURCE_DIR})
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

target_link_libraries(spiderweb PUBLIC absl::any ghc_filesystem fmt spdlog
                                       restinio)

# unit test
add_executable(spiderweb_test)
target_sources(
  spiderweb_test
  PRIVATE core/spiderweb_object_test.cc
          core/spiderweb_timer_test.cc
          core/spiderweb_eventloop_test.cc
          core/spiderweb_event_spy_test.cc
          io/spiderweb_binary_writer_test.cc
          net/spiderweb_tcp_socket_test.cc
          net/spiderweb_tcp_server_test.cc)
target_link_libraries(spiderweb_test PRIVATE spiderweb gtest_main gtest gmock)

# benchmark
add_executable(spiderweb_benchmark)
target_sources(
  spiderweb_benchmark PRIVATE core/spiderweb_eventsignal_benchmark.cc
                              core/spiderweb_timer_benchmark.cc)
target_link_libraries(spiderweb_benchmark PRIVATE spiderweb benchmark
                                                  benchmark_main)
