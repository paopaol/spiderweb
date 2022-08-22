#ifndef SPIDERWEB_EVENT_SPY_H
#define SPIDERWEB_EVENT_SPY_H

#include "core/spiderweb_eventloop.h"
#include "core/spiderweb_eventsignal.h"
#include "core/spiderweb_object.h"

namespace spiderweb {

class EventSpy : public Object {
 public:
  template <typename T, typename T2, typename Ret, typename... Args>
  explicit EventSpy(T *instance, EventSignal<Ret, Args...> T2::*event) {
    Object::Connect(instance, event, instance,
                    [this](Args &&...) { ++count_; });
  }

  inline uint64_t Count() const { return count_; }

  void Wait(int ms = 3000) {
    auto left = ms;

    do {
      std::error_code ec;
      ownerEventLoop()->IoService().run_one(ec);
      std::this_thread::sleep_for(std::chrono::microseconds(10));
      left -= 10;
    } while (left > 0);
  }

 private:
  uint64_t count_ = 0;
};
}  // namespace spiderweb

#endif
