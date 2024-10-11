#include "spiderweb/net/spiderweb_tcp_server.h"

#include "net/private/spiderweb_tcp_server_private.h"
#include "spiderweb/core/internal/thread_check.h"

namespace spiderweb {
namespace net {

TcpServer::TcpServer(uint16_t port, Object *parent)
    : Object(parent), d(std::make_shared<Private>(port, this)) {
}

TcpServer::~TcpServer() {
  SPIDERWEB_CALL_THREAD_CHECK(TcpServer::~TcpServer);
  d->q = nullptr;
}

void TcpServer::ListenAndServ() {
  SPIDERWEB_CALL_THREAD_CHECK(TcpServer::~ListenAndServ);
  if (!d->acceptor.is_open()) {
    d->acceptor = asio::ip::tcp::acceptor(AsioService(ownerEventLoop()),
                                          asio::ip::tcp::endpoint(asio::ip::tcp::v4(), d->port_));
  }
  d->StartAccept(d->acceptor);
}

void TcpServer::Stop() {
  SPIDERWEB_CALL_THREAD_CHECK(TcpServer::~Stop);
  ErrorCode ec;
  (void)d->acceptor.close(ec);
  spider_emit Stopped(std::move(ec));
}

}  // namespace net
}  // namespace spiderweb
