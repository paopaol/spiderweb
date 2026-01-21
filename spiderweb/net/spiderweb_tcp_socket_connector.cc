#include "spiderweb/net/spiderweb_tcp_socket_connector.h"

#include <absl/memory/memory.h>

#include "spiderweb/core/spiderweb_timer.h"

namespace spiderweb {
namespace net {

class TcpSocketConnector::Private {
 public:
  int32_t dead_line = 3000;
};

TcpSocketConnector::TcpSocketConnector(spiderweb::Object* parent)
    : spiderweb::Object(parent), d(absl::make_unique<Private>()) {
}

TcpSocketConnector::~TcpSocketConnector() = default;

void TcpSocketConnector::SetDeadline(int32_t timeout) {
  d->dead_line = timeout;
}

void TcpSocketConnector::ConnectToHost(const std::string& ip, uint16_t port) {
  auto* timer = new spiderweb::Timer(this);
  auto* sd = new TcpSocket(this);

  Connect(sd, &TcpSocket::ConnectionEstablished, this, [this, sd, timer]() {
    timer->Stop();
    timer->DeleteLater();
    spider_emit ConnectionEstablished(sd);
  });

  Connect(sd, &TcpSocket::ConnectError, this, [this, sd, timer](const std::error_code& ec) {
    timer->Stop();
    timer->DeleteLater();

    sd->DeleteLater();
    spider_emit ConnectError(ec);
  });

  Connect(timer, &spiderweb::Timer::timeout, this, [sd]() { sd->DisConnectFromHost(); });

  timer->SetInterval(d->dead_line);
  timer->SetSingalShot(true);
  timer->Start();

  sd->ConnectToHost(ip, port);
}
}  // namespace net
}  // namespace spiderweb
