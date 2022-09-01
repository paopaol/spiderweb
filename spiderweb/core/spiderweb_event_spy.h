#ifndef SPIDERWEB_EVENT_SPY_H
#define SPIDERWEB_EVENT_SPY_H

#include "absl/types/any.h"
#include "core/spiderweb_eventloop.h"
#include "core/spiderweb_eventsignal.h"
#include "core/spiderweb_object.h"

namespace spiderweb {

class EventSpy : public Object {
 public:
  template <typename T, typename T2, typename Ret, typename... Args>
  explicit EventSpy(T *instance, EventSignal<Ret(Args...)> T2::*event) {
    Object::Connect(instance, event, instance, [this](Args... args) {
      ++count_;
      std::tuple<decay_t<Args>...> t(std::forward<Args>(args)...);
      results_.emplace_back(std::move(t));
    });
  }

  inline uint64_t Count() const {
    return count_;
  }

  void Wait(int ms = 3000, uint64_t count = 1) {
    auto left = ms;

    do {
      std::error_code ec;
      ownerEventLoop()->IoService().run_for(std::chrono::milliseconds(10));
      left -= 10;
    } while (left > 0 && count_ < count);
  }

  template <typename... Args>
  const std::tuple<Args...> LastResult() const {
    assert(!results_.empty());
    const auto &last = results_.back();
    return absl::any_cast<std::tuple<Args...>>(last);
  }

 private:
  uint64_t               count_ = 0;
  std::vector<absl::any> results_;
};
}  // namespace spiderweb

#endif
