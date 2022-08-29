#ifndef SPIDERWEB_OBJECT_H
#define SPIDERWEB_OBJECT_H

#include <functional>
#include <memory>
#include <thread>
#include <tuple>
#include <type_traits>
#include <utility>

#include "asio-1.22.1/include/asio/io_service.hpp"
#include "internal/move_tuple_wrapper.h"
#include "spiderweb_eventsignal.h"

#define SPIDER_EMIT
#ifndef SPIDERWEB_NO_EMIT
#define spider_emit
#endif

namespace spiderweb {
template <typename Type, typename... Args>
static inline std::function<void(Args...)> create_class_member_functor(
    Type *instance, void (Type::*method)(Args... args)) {
  return [=](Args &&...args) {
    if (std::this_thread::get_id() == instance->ThreadId()) {
      (instance->*method)(std::forward<Args>(args)...);
    } else {
      auto tuple = MoveTuple(std::forward<Args>(args)...);

      instance->QueueTask([instance, method, tuple]() { tuple.Apply(method, *instance); });
    }
  };
}

template <typename Reciver, typename... Args, typename F>
static inline std::function<void(Args...)> create_none_class_member_functor(Reciver *reciver,
                                                                            F      &&f) {
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
  explicit Object(Object *parent = nullptr);

  virtual ~Object();

  Object(const Object &other) = delete;

  Object &operator=(const Object &other) = delete;

  template <typename Sender, typename SenderU, typename Reciver, typename... Args>
  static void Connect(Sender *sender, EventSignal<void(Args... args)> SenderU::*signal,
                      Reciver *reciver, void (Reciver::*method)(Args... args)) {
    static_assert(std::is_base_of<Object, Reciver>::value, "Reciver must derived from Base");

    static_assert(std::is_base_of<Object, Sender>::value, "Sender must derived from Base");

    (sender->*signal).append(create_class_member_functor(reciver, method));
  }

  template <typename Sender, typename SenderU, typename Reciver, typename... Args, typename F>
  static void Connect(Sender *sender, EventSignal<void(Args... args)> SenderU::*signal,
                      Reciver *reciver, F &&f) {
    static_assert(std::is_base_of<Object, Reciver>::value, "Reciver must derived from Base");

    static_assert(std::is_base_of<Object, Sender>::value, "Sender must derived from Base");

    (sender->*signal)
        .append(create_none_class_member_functor<Reciver, Args...>(reciver, std::forward<F>(f)));
  }

  template <typename Sender, typename SenderU, typename... Args>
  static void Emit(Sender *instance, EventSignal<void(Args...)> SenderU::*signal, Args &&...args) {
    if (instance) {
      (instance->*signal)(std::forward<Args>(args)...);
    }
  }

  EventLoop *ownerEventLoop();

  void QueueTask(std::function<void()> &&f) const;

  std::thread::id ThreadId() const;

  void DeleteLater();

 private:
  explicit Object(EventLoop *loop, Object *parent = nullptr);

  class Private;
  std::unique_ptr<Private> d;

  friend class EventLoop;
};

}  // namespace spiderweb

#endif
