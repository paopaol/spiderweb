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
    std::error_code ec;
    if (!socket.is_open()) {
      std::ignore = socket.open(endpoint.protocol(), ec);
    }

    if (!ec) {
      ec = SetOptionWhenConnect();
    }

    /**
     * @brief 选项应用失败时不再发起连接
     *
     * 否则 bind 失败会静默地从其它网卡连出去，比直接失败更难排查
     */
    if (ec) {
      handler(ec);
      return;
    }

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

  /**
   * @brief 建连前应用：open 之后、bind/connect 之前调用一次
   */
  std::error_code SetOptionWhenConnect() {
    if (!socket.is_open()) {
      return {};
    }

    std::error_code ec;

    if (option.send_buffer_size && *option.send_buffer_size > 0) {
      ec = Apply(asio::socket_base::send_buffer_size(*option.send_buffer_size));
    }

    /**
     * @brief 接收缓冲必须在 connect 前设置：窗口缩放在 SYN 中协商
     */
    if (!ec && option.recv_buffer_size && *option.recv_buffer_size > 0) {
      ec = Apply(asio::socket_base::receive_buffer_size(*option.recv_buffer_size));
    }

    if (!ec && option.local && !option.local->ip.empty() && !bound) {
      asio::ip::tcp::endpoint local(asio::ip::make_address(option.local->ip), option.local->port);
      ec = Bind(local);
      bound = !ec;
    }

    if (!ec && option.no_delay) {
      ec = Apply(asio::ip::tcp::no_delay(*option.no_delay));
    }

    return ec;
  }

  /**
   * @brief 运行期应用：socket 已打开（含 accept）时随时可调，不含 bind
   */
  std::error_code SetOptionWhenOpened() {
    if (!socket.is_open()) {
      return {};
    }

    std::error_code ec;

    if (option.no_delay) {
      ec = Apply(asio::ip::tcp::no_delay(*option.no_delay));
    }

    if (!ec && option.send_buffer_size && *option.send_buffer_size > 0) {
      ec = Apply(asio::socket_base::send_buffer_size(*option.send_buffer_size));
    }

    return ec;
  }

  void MergeOption(const Options& other) {
    if (other.local) {
      option.local = other.local;
    }

    if (other.send_buffer_size) {
      option.send_buffer_size = other.send_buffer_size;
    }

    if (other.recv_buffer_size) {
      option.recv_buffer_size = other.recv_buffer_size;
    }

    if (other.no_delay) {
      option.no_delay = other.no_delay;
    }
  }

  template <typename T>
  std::error_code Apply(const T& opt) {
    std::error_code ec;
    socket.set_option(opt, ec);
    return ec;
  }

  std::error_code Bind(const asio::ip::tcp::endpoint& local) {
    std::error_code ec;
    socket.bind(local, ec);
    return ec;
  }

  TcpSocket*            q = nullptr;
  asio::ip::tcp::socket socket;
  Options               option;
  bool                  bound = false;
};
}  // namespace net
}  // namespace spiderweb

#endif
