#include "spiderweb/net/spiderweb_tcp_socket_connector.h"

#include <absl/memory/memory.h>

#include "spiderweb/core/spiderweb_timer.h"

namespace spiderweb {
namespace net {

class TcpSocketConnector::Private {
 public:
  int32_t    deadline = 3000;
  TcpSocket* pending = nullptr;
};

TcpSocketConnector::TcpSocketConnector(spiderweb::Object* parent)
    : spiderweb::Object(parent), d(absl::make_unique<Private>()) {
}

TcpSocketConnector::~TcpSocketConnector() {
  assert(!d->pending);
}

void TcpSocketConnector::SetDeadline(int32_t timeout) {
  d->deadline = timeout;
}

void TcpSocketConnector::ConnectToHost(const std::string& ip, uint16_t port) {
  if (d->pending) {
    return;
  }

  auto* timer = new spiderweb::Timer(this);
  auto* sd = new TcpSocket(this);

  d->pending = sd;

  Connect(sd, &TcpSocket::ConnectionEstablished, sd, [this, sd, timer]() {
    d->pending = nullptr;
    timer->Stop();
    timer->DeleteLater();
    spider_emit ConnectionEstablished(sd);
  });

  Connect(sd, &TcpSocket::ConnectError, sd, [this, sd, timer](const std::error_code& ec) {
    d->pending = nullptr;
    timer->Stop();
    timer->DeleteLater();

    sd->DeleteLater();
    spider_emit ConnectError(ec);
  });

  Connect(timer, &spiderweb::Timer::timeout, sd, [sd]() { sd->DisConnectFromHost(); });

  timer->SetInterval(d->deadline);
  timer->SetSingalShot(true);
  timer->Start();

  sd->ConnectToHost(ip, port);
}

void TcpSocketConnector::CancelConnect() {
  if (d->pending) {
    d->pending->DisConnectFromHost();
    d->pending = nullptr;
  }
}

}  // namespace net
}  // namespace spiderweb
