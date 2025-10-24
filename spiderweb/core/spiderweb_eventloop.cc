#include "spiderweb/core/spiderweb_eventloop.h"

#include "asio/io_service.hpp"

namespace spiderweb {

thread_local EventLoop* current_loop = nullptr;

class EventLoop::Private {
 public:
  explicit Private(EventLoop* qq) : work(io), q(qq) {
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
  EventLoop*             q = nullptr;
};

EventLoop::EventLoop(Object* parent) : Object(this, parent), d(new Private(this)) {
  assert(current_loop);
}

EventLoop::~EventLoop() = default;

int EventLoop::Exec() {
  d->io.run();
  return d->exit_code;
}

void EventLoop::Quit() {
  d->io.stop();
}

void EventLoop::Exit(int code) {
  d->exit_code = code;
  d->io.stop();
}

spiderweb::EventLoop* EventLoop::LoopOfCurrentThread() {
  return current_loop;
}

NativeIoService EventLoop::IoService() {
  return &d->io;
}

}  // namespace spiderweb
