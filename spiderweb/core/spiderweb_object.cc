#include "spiderweb_object.h"

#include <thread>

#include "spiderweb_eventloop.h"

namespace spiderweb {
class Object::Private {
 public:
  Private(EventLoop *_loop, Object *_parent)
      : id(std::this_thread::get_id()), loop(_loop), parent(_parent) {}

  std::thread::id id;
  EventLoop      *loop = nullptr;
  Object         *parent = nullptr;
};

Object::Object(Object *parent) : d(new Private(GetLoop(parent), parent)) {}

Object::Object(EventLoop *loop, Object *parent)
    : d(new Private(loop, parent)) {}

Object::~Object() = default;

spiderweb::EventLoop *Object::ownerEventLoop() { return d->loop; }

void Object::QueueTask(std::function<void()> &&f) const {
  d->loop->IoService().post(std::forward<decltype(f)>(f));
}

std::thread::id Object::ThreadId() const { return d->id; }
}  // namespace spiderweb
