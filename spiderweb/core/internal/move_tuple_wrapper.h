#ifndef INTERNAL_MOVE_TUPLE_WRAPPER_H
#define INTERNAL_MOVE_TUPLE_WRAPPER_H

#include "index_sequence.hpp"

namespace spiderweb {
template <typename T>
using decay_t = typename std::decay<T>::type;

template <typename... Args>
struct MoveTupleWrapper {
  MoveTupleWrapper(std::tuple<Args...> &&tuple) : tuple_(std::move(tuple)) {
  }

  MoveTupleWrapper(const MoveTupleWrapper &other) : tuple_(std::move(other.tuple_)) {
  }

  MoveTupleWrapper &operator=(const MoveTupleWrapper &other) {
    tuple_ = std::move(other.tuple_);
  }

  template <typename T, typename... Params>
  void Apply(void (T::*member)(Params...), T &object) const {
    ApplyHelper(member, object, index_sequence_for<Params...>());
  }

  template <typename F>
  void Apply(F &f) const {
    ApplyHelper2(f, index_sequence_for<Args...>());
  }

  template <typename T, typename... Params, size_t... Is>
  void ApplyHelper(void (T::*member)(Params...), T &object, index_sequence<Is...>) const {
    (object.*member)(std::move(std::get<Is>(tuple_))...);
  }

  template <typename F, size_t... Is>
  void ApplyHelper2(F &f, index_sequence<Is...>) const {
    f(std::move(std::get<Is>(tuple_))...);
  }

 private:
  mutable std::tuple<Args...> tuple_;
};

template <typename... Ts>
auto MoveTuple(Ts &&...objects) -> MoveTupleWrapper<decay_t<Ts>...> {
  return std::make_tuple(std::forward<Ts>(objects)...);
}

}  // namespace spiderweb

#endif
