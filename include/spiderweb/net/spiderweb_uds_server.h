#pragma once

#include "spiderweb/core/spiderweb_error_code.h"
#include "spiderweb/core/spiderweb_notify.h"
#include "spiderweb/core/spiderweb_object.h"

namespace spiderweb {
namespace net {
class UdsSocket;
class UdsServer : public Object {
 public:
  class Private;
  explicit UdsServer(Object* parent = nullptr);

  ~UdsServer() override;

  ErrorCode ListenAndServ(const std::string& uds_address);

  void Stop();

  Notify<UdsSocket*> InComingConnection;

  Notify<ErrorCode> Stopped;

 private:
  std::shared_ptr<Private> d;
};
}  // namespace net
}  // namespace spiderweb
