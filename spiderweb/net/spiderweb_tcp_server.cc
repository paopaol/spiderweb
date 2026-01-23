#include "spiderweb/net/spiderweb_tcp_server.h"

#include "net/private/spiderweb_tcp_server_private.h"
#include "spiderweb/core/internal/thread_check.h"
#include "spiderweb/spiderweb_check.h"

namespace spiderweb {
namespace net {

TcpServer::TcpServer(uint16_t port, Object* parent)
    : Object(parent), d(std::make_shared<Private>(port, this)) {
}

TcpServer::~TcpServer() {
  SPIDERWEB_CALL_THREAD_CHECK(TcpServer::~TcpServer);
  d->q = nullptr;
}

ErrorCode TcpServer::ListenAndServ() {
  SPIDERWEB_CALL_THREAD_CHECK(TcpServer::~ListenAndServ);

  auto ec = d->Listen();
  SPIDERWEB_VERIFY(!ec, return ec);

  d->StartAccept(d->acceptor);
  return ec;
}

void TcpServer::Stop() {
  SPIDERWEB_CALL_THREAD_CHECK(TcpServer::~Stop);
  ErrorCode ec;
  (void)d->acceptor.close(ec);
  spider_emit Stopped(std::move(ec));
}

}  // namespace net
}  // namespace spiderweb
