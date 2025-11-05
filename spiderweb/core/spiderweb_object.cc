#include "spiderweb/core/spiderweb_object.h"

#include <asio/steady_timer.hpp>
#include <thread>

#include "core/internal/asio_cast.h"
#include "spiderweb/core/spiderweb_eventloop.h"

namespace spiderweb {
namespace detail {
struct ObjectDleter {
  explicit ObjectDleter(Object* ptr) : ptr_(ptr) {
  }

  void operator()() {
    delete ptr_;
  }

 private:
  Object* ptr_{nullptr};
};
}  // namespace detail

class Object::Private {
 public:
  Private(EventLoop* _loop, Object* _parent)
      : id(std::this_thread::get_id()), loop(_loop), parent(_parent) {
  }

  std::thread::id id;
  EventLoop*      loop = nullptr;
  Object*         parent = nullptr;
};

Object::Object(Object* parent) : d(new Private(GetLoop(parent), parent)) {
}

Object::Object(EventLoop* loop, Object* parent) : d(new Private(loop, parent)) {
}

Object::~Object() = default;

spiderweb::EventLoop* Object::ownerEventLoop() {
  return d->loop;
}

EventLoop* Object::ownerEventLoop() const {
  return d->loop;
}

void Object::QueueTask(std::function<void()>&& f) const {
  AsioService(d->loop).post(std::forward<decltype(f)>(f));
}

void Object::RunAfter(uint64_t delay_ms, std::function<void()>&& f) const {
  auto timer = std::make_shared<asio::steady_timer>(AsioService(ownerEventLoop()),
                                                    std::chrono::milliseconds(delay_ms));
  timer->async_wait([timer, ff = std::move(f)](const asio::error_code& ec) {
    if (ec.value() == asio::error::operation_aborted) {
      return;
    }
    ff();
  });
}

std::thread::id Object::ThreadId() const {
  return d->id;
}

void Object::DeleteLater() {
  QueueTask(detail::ObjectDleter(this));
}

void Object::DeleteLater(Object* object) {
  object->DeleteLater();
}

}  // namespace spiderweb
