#pragma once

#include <future>

#include "spiderweb/core/spiderweb_notify.h"
#include "core/spiderweb_object.h"

namespace spiderweb {

// Waiter is a thread notify tool,
// it only support notify once
template <typename T>
class Waiter {
 public:
  using ReturnType = T;

  Waiter() : future_(promise_.get_future()) {
  }

  Waiter(const Waiter &other) = delete;

  Waiter &operator=(const Waiter &rh) = delete;

  inline void Notify(ReturnType &&value) {
    promise_.set_value(std::forward<ReturnType>(value));
  }

  inline ReturnType Wait() {
    return future_.get();
  }

 private:
  std::promise<bool> promise_;
  std::future<bool>  future_;
};

class WaitGroup {
  class Private;

 public:
  explicit WaitGroup(uint32_t size);

  ~WaitGroup();

  WaitGroup(const WaitGroup &other) = delete;

  WaitGroup &operator=(const WaitGroup &rh) = delete;

  void Done();

  void Wait();

 private:
  Private *d = nullptr;
};

class AsyncWaitGroup : public Object {
  class Private;

 public:
  explicit AsyncWaitGroup(uint32_t size, Object *parent);

  ~AsyncWaitGroup() override;

  AsyncWaitGroup(const AsyncWaitGroup &other) = delete;

  AsyncWaitGroup &operator=(const AsyncWaitGroup &rh) = delete;

  void Done();

  Notify<> Finished;

 private:
  Private *d = nullptr;
};
}  // namespace spiderweb
