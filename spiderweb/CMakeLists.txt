add_library(spiderweb)

target_sources(
  spiderweb
  PRIVATE core/spiderweb_error_code.cc
          core/spiderweb_error_code.h
          core/spiderweb_event_spy.h
          core/spiderweb_eventsignal.h
          core/spiderweb_eventloop.cc
          core/spiderweb_eventloop.h
          core/spiderweb_object.cc
          core/spiderweb_object.h
          core/spiderweb_timer.cc
          core/spiderweb_timer.h
          core/spiderweb_waiter.cc
          core/spiderweb_waiter.h
          core/spiderweb_thread.cc
          core/spiderweb_thread.h
          core/spiderweb_unbounded_queue.h
          core/spiderweb_range.h
          core/spiderweb_synchronized.h
          arch/spiderweb_arch.hpp
          io/spiderweb_binary_writer.hpp
          io/spiderweb_buffer.cc
          io/spiderweb_buffer.h
          io/spiderweb_bitmap_readwriter.h
          io/spiderweb_named_pipe.cc
          io/spiderweb_named_pipe.h
          net/spiderweb_tcp_socket.cc
          net/spiderweb_tcp_socket.h
          net/spiderweb_tcp_server.cc
          net/spiderweb_tcp_server.h
          serial/spiderweb_serialport.cc
          serial/spiderweb_serialport.h
          serial/spiderweb_socketcan.h
          serial/spiderweb_socketcan.cc
          reflect/reflect_enum.hpp
          reflect/reflect_json.hpp
          reflect/reflect_xml.hpp
          reflect/macro_map.h
          type/spiderweb_any_view.h
          type/spiderweb_binary_view.h
  PRIVATE core/internal/index_sequence.hpp
          core/internal/thread_check.h
          core/internal/move_tuple_wrapper.h
          io/private/spiderweb_stream_private.h
          io/private/spiderweb_named_pipe_private.h
          net/private/spiderweb_tcp_socket_private.h
          net/private/spiderweb_tcp_server_private.h
          serial/private/spiderweb_serialport_private.h
          serial/private/spiderweb_socketcan_private.h
          ppk_assert.h
          ppk_assert.cpp)

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
         ASIO_HAS_STD_FUNCTION
         ASIO_DISABLE_STD_ALIGNED_ALLOC)

target_link_libraries(
  spiderweb
  PUBLIC absl::any
         ghc_filesystem
         fmt
         spdlog
         restinio
         yyjson
         pugixml
         date
         date-tz
         canary::canary)
add_subdirectory(reflect)

# unit test
add_executable(spiderweb_test)
target_sources(
  spiderweb_test
  PRIVATE core/spiderweb_error_code_test.cc
          core/spiderweb_object_test.cc
          core/spiderweb_timer_test.cc
          core/spiderweb_eventloop_test.cc
          core/spiderweb_event_spy_test.cc
          core/spiderweb_waiter_test.cc
          core/spiderweb_thread_test.cc
          core/spiderweb_unbounded_queue_test.cc
          core/spiderweb_range_test.cc
          core/spiderweb_synchronized_test.cc
          io/spiderweb_binary_writer_test.cc
          io/spiderweb_bitmap_readwriter_test.cc
          io/spiderweb_named_pipe_test.cc
          net/spiderweb_tcp_socket_test.cc
          net/spiderweb_tcp_server_test.cc
          serial/spiderweb_serialport_test.cc
          serial/spiderweb_socketcan_test.cc
          type/spiderweb_any_view_test.cc
          type/spiderweb_binary_view_test.cc)
target_link_libraries(spiderweb_test PRIVATE spiderweb gtest_main gtest gmock)

# benchmark
add_executable(spiderweb_benchmark)
target_sources(
  spiderweb_benchmark PRIVATE core/spiderweb_eventsignal_benchmark.cc
                              core/spiderweb_timer_benchmark.cc)
target_link_libraries(spiderweb_benchmark PRIVATE spiderweb benchmark
                                                  benchmark_main)
