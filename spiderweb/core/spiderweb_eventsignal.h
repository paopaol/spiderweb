#ifndef SPIDERWEB_EVENTSIGNAL_H
#define SPIDERWEB_EVENTSIGNAL_H

#include <cassert>
#include <functional>
#include <memory>

namespace spiderweb {

// template <typename Prototype_, typename... Args>
// class Notify : protected eventpp::CallbackList<Prototype_> {
//  public:
//   using super = eventpp::CallbackList<Prototype_, eventpp::DefaultPolicies>;
//
//   template <typename... Fargs>
//   void operator()(Fargs... args) const {
//     super::operator()(args...);
//   }
//
//   // private:
//   void append(const typename eventpp::internal_::CallbackListBase<
//               Prototype_, eventpp::DefaultPolicies>::Callback &call) {
//     super::append(call);
//   }
//
//   friend class Object;
// };

template <typename... Args>
class Notify;

template <typename... Args>
class Notify {
 public:
  template <typename... Fargs>
  void operator()(Fargs &&...args) const {
    if (f_) {
      f_(std::forward<Fargs>(args)...);
    }
  }

  void append(std::function<void(Args...)> &&f) {
    assert(!f_ && "Notify only support append once");
    f_ = std::forward<decltype(f)>(f);
  }

  friend class Object;

 private:
  std::function<void(Args...)> f_{nullptr};
};
}  // namespace spiderweb

#endif
