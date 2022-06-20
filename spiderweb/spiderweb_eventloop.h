#ifndef SPIDERWEB_EVENTLOOP_H
#define SPIDERWEB_EVENTLOOP_H

#include "asio-1.22.1/include/asio/io_service.hpp"
#include "spiderweb_eventsignal.h"

#define SPIDER_EMIT
#ifndef SPIDERWEB_NO_EMIT
#define spider_emit
#endif

namespace spiderweb {

template <typename Type, typename... Args>
static inline std::function<void(Args...)> createFunctor(
    Type *instance, void (Type::*method)(Args... args)) {
  return [=](Args &&...args) { (instance->*method)(args...); };
}

class EventLoop;
class Base {
 public:
  Base(Base *parent = nullptr);

  ~Base();

  template <typename Reciver, typename... Args>
  static void Connect(EventSignal<void(Args... args)> &signal, Reciver *reciver,
                      void (Reciver::*method)(Args... args)) {
    static_assert(std::is_base_of<Base, Reciver>::value,
                  "Reciver must derived from Base");
    signal.append(createFunctor(reciver, method));
  }

  template <typename Reciver, typename... Args,
            typename F = std::function<void(Args...)>>
  static void Connect(EventSignal<void(Args... args)> &signal, Reciver *reciver,
                      const F &f) {
    static_assert(std::is_base_of<Base, Reciver>::value,
                  "Reciver must derived from Base");
    signal.append([reciver, f](Args... args) { f(args...); });
  }

  EventLoop *ownerEventLoop();

 private:
  Base(EventLoop *loop, Base *parent = nullptr);

  class Private;
  std::unique_ptr<Private> d;

  friend class EventLoop;
};

class EventLoop : public Base {
 public:
  EventLoop(Base *parent = nullptr);
  ~EventLoop();

  int Exec();

  void Quit();

  void Exit(int code);

  static EventLoop *LoopOfCurrentThread();

  asio::io_service &IoService();

 private:
  class Private;
  std::unique_ptr<Private> d;
};

inline EventLoop *GetLoop(Base *base) {
  if (base) {
    return base->ownerEventLoop();
  }

  auto *loop = EventLoop::LoopOfCurrentThread();
  assert(loop);
  return loop;
}

}  // namespace spiderweb

#endif  // !DEBUG
