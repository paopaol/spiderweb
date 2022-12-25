#include "spiderweb/core/spiderweb_waiter.h"

#include <condition_variable>
#include <mutex>

namespace spiderweb {
class WaitGroup::Private {
 public:
  uint32_t                size = 0;
  std::mutex              mutex;
  std::condition_variable cond;
};

WaitGroup::WaitGroup(uint32_t size) : d(new Private()) {
  d->size = size;
}

WaitGroup::~WaitGroup() {
  delete d;
}

void WaitGroup::Done() {
  std::unique_lock<std::mutex> lock(d->mutex);

  d->size--;
  d->cond.notify_one();
}

void WaitGroup::Wait() {
  std::unique_lock<std::mutex> lock(d->mutex);

  while (d->size > 0) {
    d->cond.wait(lock);
  }
}

}  // namespace spiderweb
