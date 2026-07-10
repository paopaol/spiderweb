#pragma once

#include <string>

#include "asio.hpp"
#include "spdlog/spdlog.h"
#include "spiderweb/core/spiderweb_eventloop.h"
#include "spiderweb/io/private/spiderweb_stream_private.h"
#include "spiderweb/net/spiderweb_uds_server.h"
#include "spiderweb/net/spiderweb_uds_socket.h"
#include "spiderweb/spiderweb_check.h"
#include "spiderweb_uds_socket_private.h"

namespace spiderweb {
namespace net {
class UdsServer::Private : public std::enable_shared_from_this<UdsServer::Private> {
 public:
  explicit Private(UdsServer* qq) : q(qq), acceptor(AsioService(qq->ownerEventLoop())) {
  }

  ~Private() = default;

  ErrorCode Listen(const std::string& uds_address) {
    ErrorCode ec;

    asio::local::stream_protocol::endpoint endpoint(uds_address);

    auto e = acceptor.open(endpoint.protocol(), ec);
    SPIDERWEB_VERIFY(!ec, {
      spdlog::warn("UdsServer({}) open {}", fmt::ptr(q), ec.message());
      return ec;
    });

    e = acceptor.bind(endpoint, ec);
    SPIDERWEB_VERIFY(!ec, {
      spdlog::warn("UdsServer bind ({}): {}", uds_address, ec.message());
      return ec;
    });

    e = acceptor.listen(asio::socket_base::max_listen_connections, ec);
    SPIDERWEB_VERIFY(!e, {
      spdlog::warn("UdsServer({}) listen {}", fmt::ptr(q), ec.message());
      return ec;
    });
    return ec;
  }

  template <typename Acceptor>
  void StartAccept(Acceptor& uds_acceptor) {
    if (!q || !acceptor.is_open()) {
      return;
    }

    auto* client = new UdsSocket(q);
    uds_acceptor.async_accept(
        client->d->impl.socket,
        [this, self = shared_from_this(), &uds_acceptor, client](const asio::error_code& ec) {
          HandleAccept(uds_acceptor, client, ec);
        });
  }

  template <typename Acceptor>
  void HandleAccept(Acceptor& uds_acceptor, UdsSocket* client, const asio::error_code& ec) {
    if (ec) {
      spdlog::warn("UdsServer({}) {}", fmt::ptr(q), ec.message());

      client->DeleteLater();
      return;
    }
    StartAccept(uds_acceptor);

    client->d->StartRead(client->d->impl.socket);
    spider_emit Object::Emit(q, &UdsServer::InComingConnection, std::forward<UdsSocket*>(client));
  }

  UdsServer*                             q = nullptr;
  asio::local::stream_protocol::acceptor acceptor;
};

}  // namespace net
}  // namespace spiderweb
