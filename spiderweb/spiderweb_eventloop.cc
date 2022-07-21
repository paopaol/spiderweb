#include "spiderweb_eventloop.h"

#include <thread>

#include "asio.hpp"

namespace spiderweb {

thread_local EventLoop *current_loop = nullptr;

class Object::Private {
 public:
  Private(EventLoop *_loop, Object *_parent)
      : id(std::this_thread::get_id()), loop(_loop), parent(_parent) {}

  std::thread::id id;
  EventLoop      *loop = nullptr;
  Object           *parent = nullptr;
};

Object::Object(Object *parent) : d(new Private(GetLoop(parent), parent)) {}

Object::Object(EventLoop *loop, Object *parent) : d(new Private(loop, parent)) {}

Object::~Object() = default;

spiderweb::EventLoop *Object::ownerEventLoop() { return d->loop; }

class EventLoop::Private {
 public:
  Private(EventLoop *qq) : work(io), q(qq) {
    assert(!current_loop);
    current_loop = q;
  }

  ~Private() {
    assert(current_loop);
    current_loop = nullptr;
  }

  asio::io_service       io;
  asio::io_service::work work;
  int32_t                exit_code = 0;
  EventLoop             *q = nullptr;
};

EventLoop::EventLoop(Object *parent) : Object(this, parent), d(new Private(this)) {
  assert(current_loop);
}

EventLoop::~EventLoop() {}

int EventLoop::Exec() {
  d->io.run();
  return d->exit_code;
}

void EventLoop::Quit() { d->io.stop(); }

void EventLoop::Exit(int code) {
  d->exit_code = code;
  d->io.stop();
}

spiderweb::EventLoop *EventLoop::LoopOfCurrentThread() { return current_loop; }

asio::io_service &EventLoop::IoService() { return d->io; }

}  // namespace spiderweb
