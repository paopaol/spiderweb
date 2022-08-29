#include "spiderweb_tcp_socket.h"

#include <thread>

#include "core/internal/thread_check.h"
#include "net/private/spiderweb_tcp_socket_private.h"
#include "spiderweb/io/spiderweb_buffer.h"
#include "spiderweb_tcp_server.h"

namespace spiderweb {
namespace net {

TcpSocket::TcpSocket(Object *parent) : Object(parent), d(std::make_shared<Private>(this)) {
}

TcpSocket::TcpSocket(TcpServer *parent) : TcpSocket(static_cast<Object *>(parent)) {
  d->stopped = false;
}

TcpSocket::~TcpSocket() {
  CALL_THREAD_CHECK(TcpSocket::~TcpSocket);
  d->q = nullptr;
}

void TcpSocket::ConnectToHost(const std::string &ip, uint16_t port) {
  CALL_THREAD_CHECK(TcpSocket::ConnectToHost);

  asio::ip::tcp::endpoint endpoint(asio::ip::make_address(ip), port);

  d->StartConnect(endpoint);
}

void TcpSocket::DisConnectFromHost() {
  CALL_THREAD_CHECK(TcpSocket::DisConnectFromHost);
  d->CloseSocket();
}

bool TcpSocket::IsClosed() const {
  /**
   * @brief note that default construct IsClosed is false;
   */
  return d->IsStopped();
}

std::string TcpSocket::LocalStringAddress() const {
  asio::error_code ec;
  const auto      &endpoint = d->socket.local_endpoint(ec);

  return fmt::format("{}:{}", endpoint.address().to_string(), endpoint.port());
}

std::string TcpSocket::RemoteStringAddress() const {
  asio::error_code ec;
  const auto      &endpoint = d->socket.remote_endpoint(ec);

  return fmt::format("{}:{}", endpoint.address().to_string(), endpoint.port());
}

void TcpSocket::Write(const uint8_t *data, std::size_t size) {
  CALL_THREAD_CHECK(TcpSocket::Write);
  d->StartWrite(data, size);
}

}  // namespace net
}  // namespace spiderweb
