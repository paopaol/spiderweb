#pragma once

#include "asio/io_service.hpp"
#include "core/spiderweb_object.h"
#include "spiderweb/core/spiderweb_eventloop.h"

namespace spiderweb {
inline asio::io_service& AsioService(EventLoop* loop) {
  return *reinterpret_cast<asio::io_service*>(loop->ownerEventLoop()->IoService());
}
}  // namespace spiderweb
