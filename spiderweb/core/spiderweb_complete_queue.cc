#include "spiderweb/core/spiderweb_complete_queue.h"

#include <deque>
#include <mutex>
#include <utility>

#include "spiderweb/spiderweb_check.h"

namespace spiderweb {

struct AsyncOperationElement {
  AsyncOperation::Ptr                                             op;
  std::function<void(AsyncOperation::Ptr, AsyncOperation::State)> done;
};

class AsyncQueueContext : public std::enable_shared_from_this<AsyncQueueContext> {
 public:
  using SPtr = std::shared_ptr<AsyncQueueContext>;

  ~AsyncQueueContext() = default;

  void BeginOperation(AsyncOperation *op) {
    if (!shutdown) {
      op->Start();
    } else {
      EndOperation(op, AsyncOperation::State::kShutDown);
    }
  }

  void EndOperation(AsyncOperation *op, AsyncOperation::State result) {
    AsyncOperationElement el;

    {
      std::lock_guard<std::mutex> l(queue_mutex);
      assert(!queue.empty() && queue.front().op.get() == op);

      el = std::move(queue.front());
      queue.pop_front();
    }

    auto self = shared_from_this();
    el.done(std::move(el.op), result);

    DispatchNextPending();
  }

  void DispatchNextPending() {
    AsyncOperation *next_op = nullptr;

    {
      std::lock_guard<std::mutex> l(queue_mutex);

      SPIDERWEB_VERIFY(!queue.empty() && queue.front().op->state == AsyncOperation::State::kPending,
                       return);

      queue.front().op->state = AsyncOperation::State::kStarted;
      next_op = queue.front().op.get();
    }

    BeginOperation(next_op);
  }

  std::mutex                        queue_mutex;
  std::deque<AsyncOperationElement> queue;

  std::mutex shutdown_mutex;
  bool       shutdown = true;
};

void AsyncOperation::Done() {
  assert(ctx);
  ctx->EndOperation(this, State::kDone);
}

class AsyncCompleteQueue::Private {
 public:
  AsyncQueueContext::SPtr ctx;
};

AsyncCompleteQueue::AsyncCompleteQueue() : d(new Private()) {
  d->ctx = std::make_shared<AsyncQueueContext>();
}

AsyncCompleteQueue::~AsyncCompleteQueue() {
  ShutDown();
}

void AsyncCompleteQueue::Commit(
    AsyncOperation::Ptr op, std::function<void(AsyncOperation::Ptr, AsyncOperation::State)> done) {
  AsyncOperationElement el{std::move(op), std::move(done)};

  if (IsShutDown()) {
    el.done(std::move(el.op), AsyncOperation::State::kShutDown);
    return;
  }

  {
    std::lock_guard<std::mutex> l(d->ctx->queue_mutex);
    el.op->ctx = d->ctx;
    d->ctx->queue.push_back(std::move(el));
  }

  d->ctx->DispatchNextPending();
}

void AsyncCompleteQueue::Open() {
  std::lock_guard<std::mutex> l(d->ctx->shutdown_mutex);
  d->ctx->shutdown = false;
}

void AsyncCompleteQueue::ShutDown() {
  std::lock_guard<std::mutex> l(d->ctx->shutdown_mutex);

  d->ctx->shutdown = true;
}

bool AsyncCompleteQueue::IsShutDown() const {
  std::lock_guard<std::mutex> l(d->ctx->shutdown_mutex);
  return d->ctx->shutdown;
}

}  // namespace spiderweb
