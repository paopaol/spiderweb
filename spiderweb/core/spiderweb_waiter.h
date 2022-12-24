#pragma once

#include <future>

namespace spiderweb {

// Waiter is a thread notify tool,
// it only support notify once
template <typename T>
class Waiter {
 public:
  using ReturnType = T;

  Waiter() : future_(promise_.get_future()) {
  }

  void Notify(ReturnType &&value) {
    promise_.set_value(std::forward<ReturnType>(value));
  }

  ReturnType Wait() {
    return future_.get();
  }

 private:
  std::promise<bool> promise_;
  std::future<bool>  future_;
};
}  // namespace spiderweb
