#include "spiderweb_tcp_server.h"

#include "asio.hpp"
#include "core/internal/thread_check.h"
#include "core/spiderweb_eventloop.h"
#include "net/private/spiderweb_tcp_socket_private.h"

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

  void StartAccept() {
    if (!q) {
      return;
    }

    auto *client = new TcpSocket(q);
    acceptor.async_accept(client->d->socket, std::bind(&Private::HandleAccept, shared_from_this(),
                                                       client, std::placeholders::_1));
  }

  void HandleAccept(TcpSocket *client, const asio::error_code &ec) {
    if (ec) {
      spdlog::warn("TcpServer({}) {}", fmt::ptr(q), ec.message());

      client->DeleteLater();
      return;
    }

    StartAccept();

    client->d->StartRead();
    spider_emit Object::Emit(q, &TcpServer::InComingConnection, std::forward<TcpSocket *>(client));
  }

  TcpServer              *q = nullptr;
  asio::ip::tcp::acceptor acceptor;
};

TcpServer::TcpServer(uint16_t port, Object *parent)
    : Object(parent), d(std::make_shared<Private>(port, this)) {
}

TcpServer::~TcpServer() {
  SPIDERWEB_CALL_THREAD_CHECK(TcpServer::~TcpServer);
  d->q = nullptr;
}

void TcpServer::ListenAndServ() {
  d->StartAccept();
}

}  // namespace net
}  // namespace spiderweb
