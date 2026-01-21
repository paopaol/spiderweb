#pragma once

#include "asio.hpp"
#include "core/internal/asio_cast.h"
#include "spiderweb/core/spiderweb_eventloop.h"
#include "spiderweb/io/spiderweb_named_pipe.h"

namespace spiderweb {
class NamedPipe::Private {
 public:
  explicit Private(NamedPipe* qq) : q(qq), pipe(AsioService(qq->ownerEventLoop())) {
  }

  template <typename AsyncStream, typename Handler>
  void Open(AsyncStream&, Handler&& handler) {
    std::error_code ec;
    const int       fd = ::open(name.c_str(), O_RDWR | O_NONBLOCK);

    if (fd < 0) {
      ec = std::make_error_code(static_cast<std::errc>(errno));
    } else {
      pipe.assign(fd);
    }

    q->QueueTask([ec, h = std::forward<Handler>(handler)]() { h(ec); });
  }

  inline const char* Description() const {
    return "NamedPipe";
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
    spider_emit Object::Emit(q, &NamedPipe::Error, ec);
  }

  template <typename AsyncStream>
  void Close(AsyncStream& stream) {
    std::error_code ec;
    stream.close(ec);
  }

  void Written(std::size_t size) {
    spider_emit q->BytesWritten(size);
  }

  void Readden(const io::BufferReader& reader) {
    spider_emit q->BytesRead(reader);
  }

  void OpenSuccess() {
    spider_emit Object::Emit(q, &NamedPipe::OpenSuccess);
  }

  void OpenFailed(const asio::error_code& ec) {
    spider_emit Object::Emit(q, &NamedPipe::OpenError, ec);
  }

  NamedPipe*                     q = nullptr;
  asio::posix::stream_descriptor pipe;
  std::string                    name;
};
}  // namespace spiderweb
