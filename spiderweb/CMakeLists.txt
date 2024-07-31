add_library(spiderweb SHARED)

target_sources(
  spiderweb
  PRIVATE
    ${PROJECT_SOURCE_DIR}/include/spiderweb/core/spiderweb_error_code.h
    ${PROJECT_SOURCE_DIR}/include/spiderweb/core/spiderweb_notify_spy.h
    ${PROJECT_SOURCE_DIR}/include/spiderweb/core/spiderweb_notify.h
    ${PROJECT_SOURCE_DIR}/include/spiderweb/core/spiderweb_eventloop.h
    ${PROJECT_SOURCE_DIR}/include/spiderweb/core/spiderweb_object.h
    ${PROJECT_SOURCE_DIR}/include/spiderweb/core/spiderweb_timer.h
    ${PROJECT_SOURCE_DIR}/include/spiderweb/core/spiderweb_waiter.h
    ${PROJECT_SOURCE_DIR}/include/spiderweb/core/spiderweb_thread.h
    ${PROJECT_SOURCE_DIR}/include/spiderweb/core/spiderweb_thread_data.h
    ${PROJECT_SOURCE_DIR}/include/spiderweb/core/spiderweb_unbounded_queue.h
    ${PROJECT_SOURCE_DIR}/include/spiderweb/core/spiderweb_range.h
    ${PROJECT_SOURCE_DIR}/include/spiderweb/core/spiderweb_synchronized.h
    ${PROJECT_SOURCE_DIR}/include/spiderweb/core/spiderweb_signal.h
    ${PROJECT_SOURCE_DIR}/include/spiderweb/io/spiderweb_binary_writer.hpp
    ${PROJECT_SOURCE_DIR}/include/spiderweb/io/spiderweb_buffer.h
    ${PROJECT_SOURCE_DIR}/include/spiderweb/io/spiderweb_bitmap_readwriter.h
    ${PROJECT_SOURCE_DIR}/include/spiderweb/io/spiderweb_named_pipe.h
    ${PROJECT_SOURCE_DIR}/include/spiderweb/net/spiderweb_tcp_socket.h
    ${PROJECT_SOURCE_DIR}/include/spiderweb/net/spiderweb_tcp_server.h
    ${PROJECT_SOURCE_DIR}/include/spiderweb/serial/spiderweb_serialport.h
    ${PROJECT_SOURCE_DIR}/include/spiderweb/serial/spiderweb_socketcan.h
    ${PROJECT_SOURCE_DIR}/include/spiderweb/reflect/enum_reflect.h
    ${PROJECT_SOURCE_DIR}/include/spiderweb/reflect/json_reflect.h
    ${PROJECT_SOURCE_DIR}/include/spiderweb/reflect/json_node.h
    ${PROJECT_SOURCE_DIR}/include/spiderweb/reflect/xml_reflect.h
    ${PROJECT_SOURCE_DIR}/include/spiderweb/reflect/xml_node.h
    ${PROJECT_SOURCE_DIR}/include/spiderweb/reflect/macro_map.h
    ${PROJECT_SOURCE_DIR}/include/spiderweb/type/spiderweb_any_view.h
    ${PROJECT_SOURCE_DIR}/include/spiderweb/type/spiderweb_binary_view.h
    ${PROJECT_SOURCE_DIR}/include/spiderweb/type/spiderweb_variant.h
    ${PROJECT_SOURCE_DIR}/include/spiderweb/arch/spiderweb_arch.hpp
    ${PROJECT_SOURCE_DIR}/include/spiderweb/core/internal/index_sequence.hpp
    ${PROJECT_SOURCE_DIR}/include/spiderweb/core/internal/thread_check.h
    ${PROJECT_SOURCE_DIR}/include/spiderweb/core/internal/move_tuple_wrapper.h
    core/spiderweb_error_code.cc
    core/spiderweb_notify_spy.cc
    core/spiderweb_eventloop.cc
    core/spiderweb_object.cc
    core/spiderweb_timer.cc
    core/spiderweb_waiter.cc
    core/spiderweb_thread.cc
    core/spiderweb_signal.cc
    io/spiderweb_buffer.cc
    io/spiderweb_named_pipe.cc
    net/spiderweb_tcp_socket.cc
    net/spiderweb_tcp_server.cc
    serial/spiderweb_serialport.cc
    serial/spiderweb_socketcan.cc
    type/spiderweb_variant.cc
    reflect/pugixml_impl.cc
    reflect/yyjson_impl.cc
    io/private/spiderweb_stream_private.h
    io/private/spiderweb_named_pipe_private.h
    net/private/spiderweb_tcp_socket_private.h
    net/private/spiderweb_tcp_server_private.h
    serial/private/spiderweb_serialport_private.h
    serial/private/spiderweb_socketcan_private.h
    ppk_assert.h
    ppk_assert.cpp)

target_include_directories(
  spiderweb
  PUBLIC ${PROJECT_SOURCE_DIR}/thrd/asio-1.22.1/include
         ${CMAKE_CURRENT_SOURCE_DIR} ${PROJECT_SOURCE_DIR}
         ${PROJECT_SOURCE_DIR}/include)
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
         absl::variant
         absl::strings
         ghcFilesystem::ghc_filesystem
         spdlog::spdlog
         yyjson::yyjson
         pugixml
         date::date
         date::date-tz
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
          core/spiderweb_notify_spy_test.cc
          core/spiderweb_waiter_test.cc
          core/spiderweb_thread_test.cc
          core/spiderweb_thread_data_test.cc
          core/spiderweb_unbounded_queue_test.cc
          core/spiderweb_range_test.cc
          core/spiderweb_synchronized_test.cc
          core/spiderweb_signal_test.cc
          io/spiderweb_binary_writer_test.cc
          io/spiderweb_bitmap_readwriter_test.cc
          io/spiderweb_named_pipe_test.cc
          net/spiderweb_tcp_socket_test.cc
          net/spiderweb_tcp_server_test.cc
          serial/spiderweb_serialport_test.cc
          serial/spiderweb_socketcan_test.cc
          type/spiderweb_any_view_test.cc
          type/spiderweb_binary_view_test.cc
          type/spiderweb_variant_test.cc
          reflect/pugixml_impl_test.cc
          reflect/yyjson_impl_test.cc)
target_link_libraries(spiderweb_test PRIVATE spiderweb GTest::gtest_main
                                             GTest::gtest GTest::gmock)

# benchmark
add_executable(spiderweb_benchmark)
target_sources(
  spiderweb_benchmark
  PRIVATE core/spiderweb_notify_benchmark.cc core/spiderweb_timer_benchmark.cc
          type/spiderweb_variant_benchmark.cc)
target_link_libraries(
  spiderweb_benchmark PRIVATE spiderweb benchmark::benchmark
                              benchmark::benchmark_main)
