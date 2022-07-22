
#ifndef SPIDERWEB_OBJECT_H
#define SPIDERWEB_OBJECT_H

#include <functional>
#include <memory>
#include <thread>
#include <tuple>
#include <type_traits>
#include <utility>

#include "asio-1.22.1/include/asio/io_service.hpp"
#include "index_sequence.hpp"
#include "spiderweb_eventsignal.h"

#define SPIDER_EMIT
#ifndef SPIDERWEB_NO_EMIT
#define spider_emit
#endif

namespace spiderweb {
template <typename T>
using decay_t = typename std::decay<T>::type;

template <typename... Args>
struct MoveTupleWrapper {
  MoveTupleWrapper(std::tuple<Args...> &&tuple) : tuple_(std::move(tuple)) {}

  MoveTupleWrapper(const MoveTupleWrapper &other)
      : tuple_(std::move(other.tuple_)) {}

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
  void ApplyHelper(void (T::*member)(Params...), T &object,
                   index_sequence<Is...>) const {
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

template <typename Type, typename... Args>
static inline std::function<void(Args...)> create_class_member_functor(
    Type *instance, void (Type::*method)(Args... args)) {
  return [=](Args &&...args) {
    if (std::this_thread::get_id() == instance->ThreadId()) {
      (instance->*method)(std::forward<Args>(args)...);
    } else {
      auto tuple = MoveTuple(std::forward<Args>(args)...);

      instance->QueueTask(
          [instance, method, tuple]() { tuple.Apply(method, *instance); });
    }
  };
}

template <typename Reciver, typename... Args, typename F>
static inline std::function<void(Args...)> create_none_class_member_functor(
    Reciver *reciver, F &&f) {
  return [=](Args &&...args) {
    if (std::this_thread::get_id() == reciver->ThreadId()) {
      f(std::forward<Args>(args)...);
    } else {
      auto tuple = MoveTuple(std::forward<Args>(args)...);

      reciver->QueueTask([tuple, f]() { tuple.Apply(f); });
    }
  };
}

class EventLoop;
class Object {
 public:
  Object(Object *parent = nullptr);

  ~Object();

  template <typename Reciver, typename... Args>
  static void Connect(EventSignal<void(Args... args)> &signal, Reciver *reciver,
                      void (Reciver::*method)(Args... args)) {
    static_assert(std::is_base_of<Object, Reciver>::value,
                  "Reciver must derived from Base");
    signal.append(create_class_member_functor(reciver, method));
  }

  template <typename Reciver, typename... Args, typename F>
  static void Connect(EventSignal<void(Args... args)> &signal, Reciver *reciver,
                      F &&f) {
    static_assert(std::is_base_of<Object, Reciver>::value,
                  "Reciver must derived from Base");
    signal.append(create_none_class_member_functor<Reciver, decay_t<Args>...>(
        reciver, std::forward<F>(f)));
  }

  EventLoop *ownerEventLoop();

  void QueueTask(const std::function<void()> &f) const;

  std::thread::id ThreadId() const;

 private:
  Object(EventLoop *loop, Object *parent = nullptr);

  class Private;
  std::unique_ptr<Private> d;

  friend class EventLoop;
};

}  // namespace spiderweb

#endif
