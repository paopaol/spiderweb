#include "spiderweb_timer.h"

namespace spiderweb {
class Timer::Private {
 public:
  Private(Timer *qq, Object *parent)
      : q(qq), timer(GetLoop(parent)->IoService()) {}

  asio::steady_timer timer;
  uint32_t           timeoutms = 3000;
  bool               is_running = {false};
  bool               singal_shot = false;
  Timer             *q = nullptr;

  void DoSetExpired(const uint64_t timeout_ms) {
    if (!is_running) {
      return;
    }

    timer.expires_from_now(std::chrono::milliseconds(timeout_ms));
    timer.async_wait([this, timeout_ms](const asio::error_code &e) {
      if (e.value() == asio::error::operation_aborted || !is_running) {
        return;
      }

      spider_emit q->timeout();

      if (singal_shot) {
        is_running = false;
        return;
      }
      DoSetExpired(timeout_ms);
    });
  }
};

Timer::Timer(Object *parent) : Object(parent), d(new Private(this, parent)) {}

Timer::~Timer() = default;

void Timer::SetInterval(uint32_t timeout_ms) { d->timeoutms = timeout_ms; }

uint32_t Timer::Interval() const { return d->timeoutms; }

void Timer::SetSingalShot(bool flag) { d->singal_shot = flag; }

void Timer::Start() { Reset(d->timeoutms); }

void Timer::Stop() {
  d->is_running = false;
  d->timer.cancel();
}

void Timer::Reset(const uint64_t timeout_ms) {
  assert(ThreadId() == std::this_thread::get_id());

  d->is_running = true;
  d->DoSetExpired(timeout_ms);
}

bool Timer::IsRunning() const { return d->is_running; }

}  // namespace spiderweb
