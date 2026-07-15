#pragma once

#include "core/internal/asio_cast.h"
#include "spiderweb/io/spiderweb_process_fd.h"

#if defined(_WIN32)
#include <asio/windows/stream_handle.hpp>

using fd_stream = asio::windows::stream_handle;
#else
#include <asio/posix/stream_descriptor.hpp>

using fd_stream = asio::posix::stream_descriptor;
#endif

namespace spiderweb {
class ProcessFd::Private {
 public:
  explicit Private(ProcessFd* qq) : q(qq), stream(AsioService(qq->ownerEventLoop())) {
  }

  inline const char* Description() const {
    return "Fd";
  }

  template <typename AsyncStream, typename Handler>
  void Read(AsyncStream& stream, const asio::mutable_buffers_1& buffer, Handler&& handler) {
    stream.async_read_some(buffer, std::forward<Handler>(handler));
  }

  template <typename AsyncStream, typename Handler>
  void Write(AsyncStream& stream, const asio::mutable_buffers_1& buffers, Handler&& handler) {
    asio::async_write(stream, buffers, asio::transfer_all(), std::forward<Handler>(handler));
  }

  void Error(const asio::error_code& ec) {
    spider_emit Object::Emit(q, &ProcessFd::Error, ec);
  }

  template <typename AsyncStream>
  void Close(AsyncStream&) {
  }

  void Written(std::size_t size) {
    spider_emit q->BytesWritten(size);
  }

  void Readden(const io::BufferReader& reader) {
    spider_emit q->BytesRead(reader);
  }

  ProcessFd* q = nullptr;
  fd_stream  stream;
};

}  // namespace spiderweb
