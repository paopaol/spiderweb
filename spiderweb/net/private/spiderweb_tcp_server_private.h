#ifndef SPIDERWEB_TCP_SERVER_PRIVATE_H
#define SPIDERWEB_TCP_SERVER_PRIVATE_H

#include "asio.hpp"
#include "spdlog/spdlog.h"
#include "spiderweb/core/spiderweb_eventloop.h"
#include "spiderweb/io/private/spiderweb_stream_private.h"
#include "spiderweb/net/spiderweb_tcp_server.h"
#include "spiderweb/net/spiderweb_tcp_socket.h"
#include "spiderweb_tcp_socket_private.h"

namespace spiderweb {
namespace net {
class TcpServer::Private : public std::enable_shared_from_this<TcpServer::Private> {
 public:
  explicit Private(uint16_t port, TcpServer *qq)
      : q(qq),
        port_(port),
        /**
         * @brief currently, only support ipv4
         */
        acceptor(AsioService(qq->ownerEventLoop())) {
    asio::error_code        ec;
    asio::ip::tcp::endpoint endpoint(asio::ip::tcp::v4(), port);

    acceptor.open(endpoint.protocol(), ec);
    acceptor.set_option(asio::ip::tcp::acceptor::reuse_address(true));
    acceptor.bind(endpoint, ec);
    if (ec) {
      spdlog::warn("TcpServer({}) {}", fmt::ptr(q), ec.message());
      return;
    }
    acceptor.listen();
  }

  ~Private() = default;

  template <typename Acceptor>
  void StartAccept(Acceptor &tcp_acceptor) {
    if (!q || !acceptor.is_open()) {
      return;
    }

    auto *client = new TcpSocket(q);
    tcp_acceptor.async_accept(
        client->d->impl.socket,
        [this, self = shared_from_this(), &tcp_acceptor, client](const asio::error_code &ec) {
          HandleAccept(tcp_acceptor, client, ec);
        });
  }

  template <typename Acceptor>
  void HandleAccept(Acceptor &tcp_acceptor, TcpSocket *client, const asio::error_code &ec) {
    if (ec) {
      spdlog::warn("TcpServer({}) {}", fmt::ptr(q), ec.message());

      client->DeleteLater();
      return;
    }
    StartAccept(tcp_acceptor);

    client->d->StartRead(client->d->impl.socket);
    spider_emit Object::Emit(q, &TcpServer::InComingConnection, std::forward<TcpSocket *>(client));
  }

  TcpServer              *q = nullptr;
  uint16_t                port_;
  asio::ip::tcp::acceptor acceptor;
};

}  // namespace net
}  // namespace spiderweb

#endif
