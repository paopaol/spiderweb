#ifndef SPIDERWEB_TCP_SOCKET_PRIVATE_H
#define SPIDERWEB_TCP_SOCKET_PRIVATE_H

#include <tuple>

#include "asio.hpp"
#include "core/internal/asio_cast.h"
#include "spiderweb/core/spiderweb_eventloop.h"
#include "spiderweb/net/spiderweb_tcp_socket.h"

namespace spiderweb {
namespace net {
class TcpSocket::Private {
 public:
  explicit Private(TcpSocket* qq) : q(qq), socket(AsioService(qq->ownerEventLoop())) {
  }

  template <typename AsyncStream, typename Handler>
  void Open(AsyncStream& stream, const asio::ip::tcp::endpoint& endpoint, Handler&& handler) {
    TrySetOption(endpoint);
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
    spider_emit Object::Emit(q, &TcpSocket::Error, ec);
  }

  void OpenSuccess() {
    spider_emit Object::Emit(q, &TcpSocket::ConnectionEstablished);
  }

  void OpenFailed(const asio::error_code& ec) {
    spider_emit Object::Emit(q, &TcpSocket::ConnectError, ec);
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
    return "TcpSocket";
  }

  void TrySetOption(const asio::ip::tcp::endpoint& endpoint) {
    if (!socket.is_open()) {
      std::error_code ec;
      std::ignore = socket.open(endpoint.protocol(), ec);
    }

    SetNoDelay();
    SetSendBufferSize();
    Bind();
  }

  void SetNoDelay() {
    std::error_code ec;
    std::ignore = socket.set_option(asio::ip::tcp::no_delay(no_delay), ec);
    if (ec) {
      q->SetOptionError(ec);
    }
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

  void Bind() {
    if (local_ip.empty()) {
      return;
    }

    std::error_code         ec;
    asio::ip::tcp::endpoint local(asio::ip::make_address(local_ip), local_port);
    std::ignore = socket.bind(local, ec);
    if (ec) {
      q->SetOptionError(ec);
    }
  }

  TcpSocket*            q = nullptr;
  asio::ip::tcp::socket socket;
  bool                  no_delay = false;
  std::string           local_ip;
  uint16_t              local_port = 0;
  uint32_t              send_buffer_size = 0;
};
}  // namespace net
}  // namespace spiderweb

#endif
