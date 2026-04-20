#pragma once

#include "spiderweb/core/spiderweb_object.h"
#include "spiderweb/net/spiderweb_tcp_socket.h"

namespace spiderweb {

namespace net {

class TcpSocketConnector : public spiderweb::Object {
 public:
  explicit TcpSocketConnector(spiderweb::Object* parent = nullptr);

  ~TcpSocketConnector() override;

  void SetDeadline(int32_t timeout);

  void Bind(const std::string& local_ip, uint16_t port);

  void ConnectToHost(const std::string& ip, uint16_t port);

  void CancelConnect();

  spiderweb::Notify<TcpSocket*> ConnectionEstablished;

  spiderweb::Notify<const std::error_code&> ConnectError;

  Notify<const std::error_code&> SetOptionError;

 private:
  class Private;
  std::unique_ptr<Private> d;
};
}  // namespace net

}  // namespace spiderweb
