#include "spiderweb_tcp_socket.h"

#include <thread>

#include "core/internal/thread_check.h"
#include "io/private/spiderweb_stream_private.h"
#include "net/private/spiderweb_tcp_socket_private.h"
#include "spiderweb/io/spiderweb_buffer.h"
#include "spiderweb_tcp_server.h"

namespace spiderweb {
namespace net {

TcpSocket::TcpSocket(Object *parent)
    : Object(parent), d(std::make_shared<io::IoPrivate<Private>>(this)) {
}

TcpSocket::TcpSocket(TcpServer *parent) : TcpSocket(static_cast<Object *>(parent)) {
  d->stopped = false;
}

TcpSocket::~TcpSocket() {
  SPIDERWEB_CALL_THREAD_CHECK(TcpSocket::~TcpSocket);
  d->impl.q = nullptr;
}

void TcpSocket::ConnectToHost(const std::string &ip, uint16_t port) {
  SPIDERWEB_CALL_THREAD_CHECK(TcpSocket::ConnectToHost);

  asio::ip::tcp::endpoint endpoint(asio::ip::make_address(ip), port);

  d->StartOpen(d->impl.socket, endpoint);
}

void TcpSocket::DisConnectFromHost() {
  SPIDERWEB_CALL_THREAD_CHECK(TcpSocket::DisConnectFromHost);
  d->Close(d->impl.socket);
}

bool TcpSocket::IsClosed() const {
  /**
   * @brief note that default construct IsClosed is true;
   */
  return d->IsStopped();
}

std::string TcpSocket::LocalStringAddress() const {
  asio::error_code ec;
  const auto      &endpoint = d->impl.socket.local_endpoint(ec);

  return fmt::format("{}:{}", endpoint.address().to_string(), endpoint.port());
}

std::string TcpSocket::RemoteStringAddress() const {
  asio::error_code ec;
  const auto      &endpoint = d->impl.socket.remote_endpoint(ec);

  return fmt::format("{}:{}", endpoint.address().to_string(), endpoint.port());
}

void TcpSocket::Write(const uint8_t *data, std::size_t size) {
  SPIDERWEB_CALL_THREAD_CHECK(TcpSocket::Write);
  d->StartWrite(d->impl.socket, data, size);
}

void TcpSocket::Write(const std::vector<uint8_t> &data) {
  SPIDERWEB_CALL_THREAD_CHECK(TcpSocket::Write);
  d->StartWrite(d->impl.socket, data);
}

}  // namespace net
}  // namespace spiderweb
