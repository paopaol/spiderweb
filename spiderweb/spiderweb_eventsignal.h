#ifndef SPIDERWEB_EVENTSIGNAL_H
#define SPIDERWEB_EVENTSIGNAL_H

#include <memory>

#include "eventpp/callbacklist.h"

namespace spiderweb {

template <typename Prototype_, typename... Args>
class EventSignal : protected eventpp::CallbackList<Prototype_> {
 public:
  using super = eventpp::CallbackList<Prototype_, eventpp::DefaultPolicies>;

  void operator()(Args... args) const { super::operator()(args...); }

  void append(const typename eventpp::internal_::CallbackListBase<
              Prototype_, eventpp::DefaultPolicies>::Callback &call) {
    super::append(call);
  }
};
}  // namespace spiderweb

#endif
