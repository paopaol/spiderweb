#ifndef SPIDERWEB_NOTIFY_SPY_H
#define SPIDERWEB_NOTIFY_SPY_H

#include <vector>

#include "absl/types/any.h"
#include "spiderweb/core/spiderweb_notify.h"
#include "spiderweb/core/spiderweb_object.h"

namespace spiderweb {

class NotifySpy : public Object {
 public:
  explicit NotifySpy(Object *loop);

  template <typename T, typename T2, typename... Args>
  explicit NotifySpy(T *instance, Notify<Args...> T2::*event) {
    Object::Connect(instance, event, instance, [this](Args... args) {
      ++count_;
      std::tuple<decay_t<Args>...> t(std::forward<Args>(args)...);
      results_.emplace_back(std::move(t));
    });
  }

  inline uint64_t Count() const {
    return count_;
  }

  void Wait(int ms = 3000, uint64_t count = 1, uint64_t tick_ms = 10);

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
