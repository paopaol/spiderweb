#include "spiderweb/core/spiderweb_notify_spy.h"

#include "core/internal/asio_cast.h"

namespace spiderweb {
void NotifySpy::Wait(int ms, uint64_t count) {
  auto left = ms;

  do {
    std::error_code ec;
    AsioService(ownerEventLoop()).run_for(std::chrono::milliseconds(10));
    left -= 10;
  } while (left > 0 && count_ < count);
}

}  // namespace spiderweb
