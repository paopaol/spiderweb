#include "spiderweb/net/spiderweb_udp_socket.h"

#include <absl/types/span.h>

#include "asio.hpp"
#include "core/internal/asio_cast.h"
#include "spdlog/spdlog.h"
#include "spiderweb/core/internal/thread_check.h"
#include "spiderweb_endpoint_visitor.h"

namespace spiderweb {
namespace net {
using asio::ip::udp;

class UdpSocket::Private : public std::enable_shared_from_this<Private> {
 public:
  explicit Private(UdpSocket* qq) : q(qq), socket(AsioService(qq->ownerEventLoop())) {
  }

  void SendTo(const EndPoint& endpoint, const uint8_t* data, std::size_t size) {
    if (stopped) {
      spdlog::warn("{}({}) stopped", Description(), fmt::ptr(q));
      return;
    }

    Datagram            datagram(endpoint, data, size);
    absl::Span<uint8_t> span(const_cast<uint8_t*>(datagram.Data().data()), datagram.Data().size());
    auto                self = shared_from_this();

    socket.async_send_to(
        asio::buffer(span), EndPointVisitor::MakeAsioEndpoint<udp>(endpoint),
        [this, datagram = std::move(datagram), self](const asio::error_code& ec, std::size_t size) {
          HandleWrite(ec, size);
        });
  }

  void StartRead() {
    if (stopped) {
      return;
    }

    auto self = shared_from_this();
    socket.async_receive_from(
        asio::buffer(recv_buffer), remote,
        [this, self](const std::error_code& ec, std::size_t bytes_transferred) {
          HandleReceive(ec, bytes_transferred);
        });
  }

  void HandleReceive(const std::error_code& ec, std::size_t bytes_transferred) {
    if (stopped) {
      return;
    }

    if (ec) {
      Stop();
      /**
       * @brief we use Object::Emit here, beause maybe `q` has gone.
       */
      Error(ec);
      return;
    }

    StartRead();

    spider_emit q->DatagramArrived(
        Datagram(EndPointVisitor::FromAsioEndpoint(remote), recv_buffer.data(), bytes_transferred));
  }

  void HandleWrite(const asio::error_code& ec, std::size_t size) {
    if (stopped) {
      return;
    }

    if (ec) {
      Stop();
      /**
       * @brief we use Object::Emit here, beause maybe `q` has gone.
       */
      Error(ec);
      return;
    }

    spider_emit q->BytesWritten(size);
  }

  inline void Stop() {
    stopped = true;
    Close();
  }

  inline void Close() {
    if (close_called) {
      return;
    }

    socket.close();
    close_called = true;
  }

  void Error(const asio::error_code& ec) {
    spider_emit Object::Emit(q, &UdpSocket::Error, ec);
  }

  inline const char* Description() const {
    return "UdpSocket";
  }

  UdpSocket*                 q = nullptr;
  asio::ip::udp::socket      socket;
  std::array<uint8_t, 65535> recv_buffer;
  asio::ip::udp::endpoint    remote;
  bool                       stopped = true;
  bool                       close_called = false;
};

UdpSocket::UdpSocket(Object* parent) : Object(parent), d(std::make_shared<Private>(this)) {
  d->q = this;
}

UdpSocket::~UdpSocket() {
  Close();
}

void UdpSocket::Open(spiderweb::ErrorCode& ec) {
  SPIDERWEB_CALL_THREAD_CHECK(UdpSocket::Open);
  (void)d->socket.open(udp::v4(), ec);
  if (!ec) {
    d->stopped = false;
  }
}

void UdpSocket::Close() {
  SPIDERWEB_CALL_THREAD_CHECK(UdpSocket::Close);
  spiderweb::ErrorCode ec;
  (void)d->socket.close(ec);
  d->stopped = true;
}

void UdpSocket::Bind(const EndPoint& endpoint, spiderweb::ErrorCode& ec) {
  SPIDERWEB_CALL_THREAD_CHECK(UdpSocket::Bind);
  (void)d->socket.bind(EndPointVisitor::MakeAsioEndpoint<udp>(endpoint), ec);
  if (!ec) {
    d->StartRead();
    ;
  }
}

void UdpSocket::SendTo(const EndPoint& endpoint, const uint8_t* data, std::size_t size) {
  SPIDERWEB_CALL_THREAD_CHECK(UdpSocket::Write);
  d->SendTo(endpoint, data, size);
}

}  // namespace net
}  // namespace spiderweb
