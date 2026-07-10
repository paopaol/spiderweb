#include "spiderweb/net/spiderweb_uds_server.h"

#include <ghc/filesystem.hpp>

#include "net/private/spiderweb_uds_server_private.h"
#include "spiderweb/core/internal/thread_check.h"
#include "spiderweb/core/spiderweb_error_code.h"
#include "spiderweb/spiderweb_check.h"

namespace spiderweb {
namespace net {

UdsServer::UdsServer(Object* parent) : Object(parent), d(std::make_shared<Private>(this)) {
}

UdsServer::~UdsServer() {
  SPIDERWEB_CALL_THREAD_CHECK(UdsServer::~UdsServer);
  d->q = nullptr;
}

ErrorCode UdsServer::ListenAndServ(const std::string& uds_address) {
  SPIDERWEB_CALL_THREAD_CHECK(UdsServer::~ListenAndServ);

  spiderweb::ErrorCode ec;
  ghc::filesystem::remove(uds_address, ec);

  ec = d->Listen(uds_address);
  SPIDERWEB_VERIFY(!ec, return ec);

  d->StartAccept(d->acceptor);
  return ec;
}

void UdsServer::Stop() {
  SPIDERWEB_CALL_THREAD_CHECK(UdsServer::~Stop);
  ErrorCode ec;
  (void)d->acceptor.close(ec);
  spider_emit Stopped(std::move(ec));
}

}  // namespace net
}  // namespace spiderweb
