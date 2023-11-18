#include "spiderweb/core/spiderweb_event_spy.h"

#include "core/internal/asio_cast.h"

namespace spiderweb {
void EventSpy::Wait(int ms, uint64_t count) {
  auto left = ms;

  do {
    std::error_code ec;
    AsioService(ownerEventLoop()).run_for(std::chrono::milliseconds(10));
    left -= 10;
  } while (left > 0 && count_ < count);
}

}  // namespace spiderweb
