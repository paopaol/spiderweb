#include "spiderweb/net/spiderweb_uds_socket.h"

#include "io/private/spiderweb_stream_private.h"
#include "net/private/spiderweb_uds_socket_private.h"
#include "spiderweb/core/internal/thread_check.h"
#include "spiderweb/net/spiderweb_uds_server.h"

namespace spiderweb {
namespace net {

UdsSocket::UdsSocket(Object* parent)
    : Object(parent), d(std::make_shared<io::IoPrivate<Private>>(this)) {
}

UdsSocket::UdsSocket(UdsServer* parent) : UdsSocket(static_cast<Object*>(parent)) {
  d->stopped = false;
}

UdsSocket::~UdsSocket() {
  SPIDERWEB_CALL_THREAD_CHECK(UdsSocket::~UdsSocket);
  d->impl.q = nullptr;
}

void UdsSocket::SetSendBufferSize(uint16_t size) {
  d->impl.send_buffer_size = size;
}

void UdsSocket::ConnectTo(const std::string& uds_address) {
  SPIDERWEB_CALL_THREAD_CHECK(UdsSocket::ConnectTo);

  asio::local::stream_protocol::endpoint endpoint(uds_address);

  d->StartOpen(d->impl.socket, endpoint);
}

void UdsSocket::DisConnect() {
  SPIDERWEB_CALL_THREAD_CHECK(UdsSocket::DisConnect);
  d->Close(d->impl.socket);
}

bool UdsSocket::IsClosed() const {
  /**
   * @brief note that default construct IsClosed is true;
   */
  return d->IsStopped();
}

std::string UdsSocket::StringAddress() const {
  asio::error_code ec;
  const auto&      endpoint = d->impl.socket.local_endpoint(ec);

  return endpoint.path();
}

void UdsSocket::Write(const uint8_t* data, std::size_t size) {
  SPIDERWEB_CALL_THREAD_CHECK(UdsSocket::Write);
  d->StartWrite(d->impl.socket, data, size);
}

void UdsSocket::Write(const std::vector<uint8_t>& data) {
  SPIDERWEB_CALL_THREAD_CHECK(UdsSocket::Write);
  d->StartWrite(d->impl.socket, data);
}

}  // namespace net
}  // namespace spiderweb
