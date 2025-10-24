#ifndef SPIDERWEB_EVENTLOOP_H
#define SPIDERWEB_EVENTLOOP_H

#include "spiderweb/core/spiderweb_notify.h"
#include "spiderweb_object.h"

#define SPIDER_EMIT
#ifndef SPIDERWEB_NO_EMIT
#define spider_emit
#endif

namespace spiderweb {
class EventLoop : public Object {
 public:
  explicit EventLoop(Object* parent = nullptr);

  ~EventLoop() override;

  int Exec();

  void Quit();

  void Exit(int code);

  static EventLoop* LoopOfCurrentThread();

  NativeIoService IoService();

 private:
  class Private;
  std::unique_ptr<Private> d;
};

inline EventLoop* GetLoop(Object* base) {
  if (base) {
    return base->ownerEventLoop();
  }

  auto* loop = EventLoop::LoopOfCurrentThread();
  assert(loop);
  return loop;
}

}  // namespace spiderweb

#endif  // !DEBUG
