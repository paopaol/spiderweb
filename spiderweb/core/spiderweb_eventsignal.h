#ifndef SPIDERWEB_EVENTSIGNAL_H
#define SPIDERWEB_EVENTSIGNAL_H

#include <cassert>
#include <functional>
#include <memory>

namespace spiderweb {

// template <typename Prototype_, typename... Args>
// class EventSignal : protected eventpp::CallbackList<Prototype_> {
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

template <typename Ret, typename... Args>
class EventSignal;

template <typename Ret, typename... Args>
class EventSignal<Ret(Args...)> {
 public:
  template <typename... Fargs>
  void operator()(Fargs &&...args) const {
    if (f_) {
      f_(std::forward<Fargs>(args)...);
    }
  }

  void append(std::function<Ret(Args...)> &&f) {
    assert(!f_ && "EventSignal only support append once");
    f_ = std::forward<decltype(f)>(f);
  }

  friend class Object;

 private:
  std::function<Ret(Args...)> f_{nullptr};
};
}  // namespace spiderweb

#endif
