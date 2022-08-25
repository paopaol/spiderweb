#ifndef SPIDER_WEB_TCP_SOCKET_H
#define SPIDER_WEB_TCP_SOCKET_H

#include "core/spiderweb_eventsignal.h"
#include "core/spiderweb_object.h"
#include "io/spiderweb_buffer.h"

namespace spiderweb {
namespace net {
class TcpSocket : public Object {
 public:
  explicit TcpSocket(Object *parent = nullptr);

  ~TcpSocket() override;

  void ConnectToHost(const std::string &ip, uint16_t port);

  void DisConnectFromHost();

  void Write(const uint8_t *data, std::size_t size);

  EventSignal<void()> ConnectionEstablished;

  EventSignal<void(const std::error_code &ec)> Error;

  EventSignal<void(const io::Buffer &buffer)> BytesRead;
  EventSignal<void(std::size_t n)>            BytesWritten;

 private:
  class Private;
  std::unique_ptr<Private> d;
};
}  // namespace net

}  // namespace spiderweb
#endif
