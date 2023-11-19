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
  d->StartAccept(d->acceptor);
}

}  // namespace net
}  // namespace spiderweb
