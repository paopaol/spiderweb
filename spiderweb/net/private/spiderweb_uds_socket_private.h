#pragma once

#include <tuple>

#include "asio.hpp"
#include "core/internal/asio_cast.h"
#include "spiderweb/core/spiderweb_eventloop.h"
#include "spiderweb/net/spiderweb_uds_socket.h"

namespace spiderweb {
namespace net {
class UdsSocket::Private {
 public:
  explicit Private(UdsSocket* qq) : q(qq), socket(AsioService(qq->ownerEventLoop())) {
  }

  template <typename AsyncStream, typename Handler>
  void Open(AsyncStream& stream, const asio::local::stream_protocol::endpoint& endpoint,
            Handler&& handler) {
    stream.async_connect(endpoint, std::forward<Handler>(handler));
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
    spider_emit Object::Emit(q, &UdsSocket::Error, ec);
  }

  void OpenSuccess() {
    spider_emit Object::Emit(q, &UdsSocket::ConnectionEstablished);
  }

  void OpenFailed(const asio::error_code& ec) {
    spider_emit Object::Emit(q, &UdsSocket::ConnectError, ec);
  }

  template <typename AsyncStream>
  void Close(AsyncStream& stream) {
    stream.close();
  }

  void Written(std::size_t size) {
    spider_emit q->BytesWritten(size);
  }

  void Readden(const io::BufferReader& reader) {
    spider_emit q->BytesRead(reader);
  }

  inline const char* Description() const {
    return "UdsSocket";
  }

  void SetSendBufferSize() {
    if (send_buffer_size == 0) {
      return;
    }

    std::error_code ec;
    std::ignore = socket.set_option(asio::socket_base::send_buffer_size(send_buffer_size), ec);
    if (ec) {
      q->SetOptionError(ec);
    }
  }

  UdsSocket*                           q = nullptr;
  asio::local::stream_protocol::socket socket;
  uint32_t                             send_buffer_size = 0;
};
}  // namespace net
}  // namespace spiderweb
