#ifndef SPIDERWEB_EVENT_SPY_H
#define SPIDERWEB_EVENT_SPY_H

#include <vector>

#include "absl/types/any.h"
#include "core/spiderweb_eventsignal.h"
#include "core/spiderweb_object.h"

namespace spiderweb {

class EventSpy : public Object {
 public:
  explicit EventSpy(Object *loop);

  template <typename T, typename T2, typename... Args>
  explicit EventSpy(T *instance, Notify<Args...> T2::*event) {
    Object::Connect(instance, event, instance, [this](Args... args) {
      ++count_;
      std::tuple<decay_t<Args>...> t(std::forward<Args>(args)...);
      results_.emplace_back(std::move(t));
    });
  }

  inline uint64_t Count() const {
    return count_;
  }

  void Wait(int ms = 3000, uint64_t count = 1);

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
