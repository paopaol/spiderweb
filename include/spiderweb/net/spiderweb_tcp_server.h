#ifndef SPIDERWEB_TCP_SERVER_H
#define SPIDERWEB_TCP_SERVER_H

#include "spiderweb/core/spiderweb_error_code.h"
#include "spiderweb/core/spiderweb_notify.h"
#include "spiderweb/core/spiderweb_object.h"

namespace spiderweb {
namespace net {
class TcpSocket;
class TcpServer : public Object {
 public:
  class Private;
  explicit TcpServer(uint16_t port, Object *parent = nullptr);

  ~TcpServer() override;

  void ListenAndServ();

  void Stop();

  Notify<TcpSocket *> InComingConnection;

  Notify<spiderweb::ErrorCode> Stopped;

 private:
  std::shared_ptr<Private> d;
};
}  // namespace net
}  // namespace spiderweb

#endif
