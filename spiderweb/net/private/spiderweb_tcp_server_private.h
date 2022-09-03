#ifndef SPIDERWEB_TCP_SERVER_PRIVATE_H
#define SPIDERWEB_TCP_SERVER_PRIVATE_H

#include "asio.hpp"
#include "fmt/format.h"
#include "net/private/spiderweb_stream_private.h"
#include "spdlog/spdlog.h"
#include "spiderweb/core/spiderweb_eventloop.h"
#include "spiderweb/net/spiderweb_tcp_server.h"
#include "spiderweb/net/spiderweb_tcp_socket.h"
#include "spiderweb_tcp_socket_private.h"

namespace spiderweb {
namespace net {
class TcpServer::Private : public std::enable_shared_from_this<TcpServer::Private> {
 public:
  explicit Private(uint16_t port, TcpServer *qq)
      : q(qq),
        /**
         * @brief currently, only support ipv4
         */
        acceptor(qq->ownerEventLoop()->IoService(),
                 asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port)) {
  }

  ~Private() = default;

  template <typename Acceptor>
  void StartAccept(Acceptor &tcp_acceptor) {
    if (!q) {
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
    StartAccept(tcp_acceptor);
    if (ec) {
      spdlog::warn("TcpServer({}) {}", fmt::ptr(q), ec.message());

      client->DeleteLater();
      return;
    }

    client->d->StartRead(client->d->impl.socket);
    spider_emit Object::Emit(q, &TcpServer::InComingConnection, std::forward<TcpSocket *>(client));
  }

  TcpServer              *q = nullptr;
  asio::ip::tcp::acceptor acceptor;
};

}  // namespace net
}  // namespace spiderweb

#endif
