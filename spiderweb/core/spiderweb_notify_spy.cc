#include "spiderweb/core/spiderweb_notify_spy.h"

#include "core/internal/asio_cast.h"

namespace spiderweb {
void NotifySpy::Wait(int ms, uint64_t count, uint64_t tick_ms) {
  auto left = ms;

  do {
    std::error_code ec;
    AsioService(ownerEventLoop()).run_for(std::chrono::milliseconds(tick_ms));
    left -= 10;
  } while (left > 0 && count_ < count);
}

}  // namespace spiderweb
