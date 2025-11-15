#include "spiderweb/core/spiderweb_thread.h"

#include <thread>

#include "absl/memory/memory.h"
#include "internal/asio_cast.h"
#include "spiderweb/core/spiderweb_eventloop.h"
#include "spiderweb/core/spiderweb_waiter.h"

namespace spiderweb {

using EventLoopPtr = std::unique_ptr<spiderweb::EventLoop>;

class Thread::Private {
 public:
  void Routine() {
    loop = absl::make_unique<spiderweb::EventLoop>();

    wait_for_satrt.Notify(true);
    loop->Exec();
    loop = nullptr;
  }

  void StartRoutineOnce() {
    std::thread tmp(std::bind(&Private::Routine, this));
    t.swap(tmp);

    wait_for_satrt.Wait();
  }

  void WaitForThreadExit() {
    if (t.joinable()) {
      t.join();
    }
  }

  void Clear() {
    if (q->IsRunning()) {
      loop->Quit();
    }
    WaitForThreadExit();
  }

  Thread*        q = nullptr;
  Waiter<bool>   wait_for_satrt;
  std::once_flag once_;
  std::thread    t;
  EventLoopPtr   loop;
};

Thread::Thread() : d(absl::make_unique<Private>()) {
  d->q = this;
}

Thread::~Thread() {
  if (d) {
    d->Clear();
  }
}

Thread::Thread(Thread&& rh) noexcept : d(absl::make_unique<Private>()) {
  d->q = this;
  std::swap(d->q, rh.d->q);
  std::swap(d, rh.d);
}

Thread& Thread::operator=(Thread&& rh) noexcept {
  if (this != &rh) {
    d->Clear();
    d = std::move(rh.d);
    d->q = this;
  }
  return *this;
}

void Thread::Start() {
  std::call_once(d->once_, std::bind(&Thread::Private::StartRoutineOnce, d.get()));
}

void Thread::Quit() {
  if (IsRunning()) {
    assert(std::this_thread::get_id() != d->t.get_id() &&
           "Thread::Quit() must not called in it's spiderweb::Thread");
    d->loop->Quit();
  }
  d->WaitForThreadExit();
}

void Thread::QueueTask(std::function<void()>&& f) {
  if (IsRunning()) {
    d->loop->QueueTask(std::move(f));
  }
}

bool Thread::IsRunning() const {
  return d->loop && !AsioService(d->loop.get()).stopped();
}

Thread::Id Thread::GetThreadId() const {
  return IsRunning() ? d->loop->ThreadId() : d->t.get_id();
}

}  // namespace spiderweb
