#ifndef SPIDERWEB_TCP_SERVER_H
#define SPIDERWEB_TCP_SERVER_H

#include "core/spiderweb_eventsignal.h"
#include "core/spiderweb_object.h"

namespace spiderweb {
namespace net {
class TcpSocket;
class TcpServer : public Object {
 public:
  class Private;
  explicit TcpServer(uint16_t port, Object *parent = nullptr);

  ~TcpServer() override;

  void ListenAndServ();

  EventSignal<void(TcpSocket *conn)> InComingConnection;

 private:
  std::shared_ptr<Private> d;
};
}  // namespace net
}  // namespace spiderweb

#endif
