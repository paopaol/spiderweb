#pragma once

#include "asio/io_service.hpp"
#include "spiderweb/core/spiderweb_eventloop.h"
#include "spiderweb/core/spiderweb_object.h"

namespace spiderweb {
inline asio::io_service& AsioService(EventLoop* loop) {
  return *reinterpret_cast<asio::io_service*>(loop->ownerEventLoop()->IoService());
}
}  // namespace spiderweb
