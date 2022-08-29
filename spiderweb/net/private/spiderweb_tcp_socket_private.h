#ifndef SPIDERWEB_TCP_SOCKET_PRIVATE_H
#define SPIDERWEB_TCP_SOCKET_PRIVATE_H

#include "asio.hpp"
#include "core/spiderweb_eventloop.h"
#include "net/spiderweb_tcp_socket.h"
#include "spdlog/spdlog.h"

namespace spiderweb {
namespace net {
class TcpSocket::Private : public std::enable_shared_from_this<TcpSocket::Private> {
 public:
  explicit Private(TcpSocket *qq) : q(qq), socket(qq->ownerEventLoop()->IoService()) {
  }

  ~Private() = default;

  inline bool IsStopped() const {
    return stopped;
  }

  void StartConnect(const asio::ip::tcp::endpoint &endpoint) {
    if (socket.is_open()) {
      spdlog::warn("TcpSocket({}) alreay connected or connecting", fmt::ptr(q));
      return;
    }

    stopped = false;
    socket.async_connect(
        endpoint, std::bind(&Private::HandleConnect, shared_from_this(), std::placeholders::_1));
  }

  void HandleConnect(const asio::error_code &ec) {
    if (stopped) {
      return;
    }

    if (ec) {
      Stop();
      /**
       * @brief we use Object::Emit here, beause maybe `q` has gone.
       */
      spider_emit Object::Emit(q, &TcpSocket::Error, ec);
      return;
    }
    StartRead();
    spider_emit Object::Emit(q, &TcpSocket::ConnectionEstablished);
  }

  void StartRead() {
    if (stopped) {
      return;
    }

    /**
     * @brief
     *
     * Since we use our own custom buffer as the receive buffer. There are many
     * reasons to use your
     *
     * own buffer. For example, it can be easily read and written, and the user
     * does not have to
     *
     * care about the management of the buffer size. But asio has its own
     * asynchronous read and
     *
     * write method, although we can use a buffer of constant size to
     * temporarily receive, and then
     *
     * copy the data to our own buffer every time we receive data. Looks fine,
     * just one more copy.
     *
     * So we can avoid it as much as possible.
     *
     * The Write process of io::Buffer is:
     *
     * 1. PrepareWrite
     *
     * 2. Copy
     *
     * 3. Commit
     *
     * Therefore, in StartRead, we separate these three steps and execute them.
     */
    if (recv_buffer.leftSpace() < kSpaceGrowSize) {
      recv_buffer.PrepareWrite(kSpaceGrowSize);
    }

    const auto &buffer = asio::buffer(recv_buffer.beginWrite(), recv_buffer.leftSpace());

    auto self = shared_from_this();
    socket.async_read_some(buffer, [this, self](const asio::error_code ec, std::size_t n) {
      if (stopped) {
        return;
      }

      if (ec) {
        Stop();
        /**
         * @brief we use Object::Emit here, beause maybe `q` has gone.
         */
        spider_emit Object::Emit(q, &TcpSocket::Error, ec);
        return;
      }

      recv_buffer.CommitWrite(n);
      StartRead();
      spider_emit q->BytesRead(io::BufferReader(recv_buffer));
    });
  }

  void StartWrite(const uint8_t *data, std::size_t size) {
    if (stopped) {
      spdlog::warn("TcpSocket({}) stopped", fmt::ptr(q));
      return;
    }

    send_buffer.Write(reinterpret_cast<const char *>(data), size);

    asio::async_write(socket, asio::buffer(send_buffer.lastRead(), send_buffer.Len()),
                      asio::transfer_all(),
                      std::bind(&Private::HanleWrite, shared_from_this(), std::placeholders::_1));
  }

  void HanleWrite(const asio::error_code &ec) {
    if (stopped) {
      return;
    }

    if (ec) {
      Stop();
      /**
       * @brief we use Object::Emit here, beause maybe `q` has gone.
       */
      spider_emit Object::Emit(q, &TcpSocket::Error, ec);
      return;
    }

    char       *ptr = nullptr;
    std::size_t written = send_buffer.Len();
    send_buffer.ZeroCopyRead(ptr, send_buffer.Len());

    spider_emit q->BytesWritten(written);
  }

  inline void Stop() {
    stopped = true;
    CloseSocket();
  }

  inline void CloseSocket() {
    if (close_called) {
      return;
    }

    socket.close();
    close_called = true;
  }

  TcpSocket               *q = nullptr;
  bool                     stopped = true;
  asio::ip::tcp::socket    socket;
  io::Buffer               recv_buffer;
  io::Buffer               send_buffer;
  static const std::size_t kSpaceGrowSize = 8192;
  bool                     close_called = false;
};

}  // namespace net
}  // namespace spiderweb

#endif
