#include "asio-1.22.1/src/tests/unit/unit_test.hpp"
#include "asio.hpp"
#include "gmock/gmock.h"

class test_stream {
 public:
  using executor_type = asio::io_context::executor_type;

  explicit test_stream(asio::io_context &io_context)
      : io_context_(io_context), length_(0), position_(0), next_read_length_(0) {
  }

  executor_type get_executor() ASIO_NOEXCEPT {
    return io_context_.get_executor();
  }

  // void reset(const void* data, size_t length) {
  //   ASIO_CHECK(length <= max_length);
  //
  //   memcpy(data_, data, length);
  //   length_ = length;
  //   position_ = 0;
  //   next_read_length_ = length;
  // }
  // void next_read_length(size_t length) {
  //   next_read_length_ = length;
  // }
  // template <typename Mutable_Buffers>
  // size_t read_some(const Mutable_Buffers& buffers) {
  //   size_t n =
  //       asio::buffer_copy(buffers, asio::buffer(data_, length_) + position_,
  //       next_read_length_);
  //   position_ += n;
  //   return n;
  // }
  //
  // template <typename Mutable_Buffers>
  // size_t read_some(const Mutable_Buffers& buffers, asio::error_code& ec) {
  //   ec = asio::error_code();
  //   return read_some(buffers);
  // }
  //
  // template <typename Mutable_Buffers, typename Handler>
  // void async_read_some(const Mutable_Buffers& buffers, Handler handler) {
  //   size_t bytes_transferred = read_some(buffers);
  //   asio::post(get_executor(),
  //   asio::detail::bind_handler(ASIO_MOVE_CAST(Handler)(handler),
  //                                                         asio::error_code(),
  //                                                         bytes_transferred));
  // }

  using ReadHandler = std::function<void(const asio::error_code &ec, std::size_t size)>;
  using WriteHandler = std::function<void(const asio::error_code &ec, std::size_t size)>;
  using ConnectHandler = std::function<void(const asio::error_code &ec)>;

  MOCK_METHOD(bool, is_open, ());
  MOCK_METHOD(void, reset, (const void *data, size_t length));
  MOCK_METHOD(void, next_read_length, (size_t length));
  MOCK_METHOD(std::size_t, read_some, (const asio::mutable_buffers_1 &buffers));
  MOCK_METHOD(std::size_t, read_some,
              (const asio::mutable_buffers_1 &buffers, asio::error_code &ec));
  MOCK_METHOD(void, async_read_some,
              (const asio::mutable_buffers_1 &buffers, const ReadHandler &handler));
  MOCK_METHOD(void, async_connect,
              (const asio::ip::tcp::endpoint &, const ConnectHandler &handler));
  MOCK_METHOD(void, async_write_some,
              (const asio::const_buffers_1 &buffers, const WriteHandler &handler));

 private:
  asio::io_context &io_context_;
  enum { max_length = 8192 };
  char   data_[max_length];
  size_t length_;
  size_t position_;
  size_t next_read_length_;
};
