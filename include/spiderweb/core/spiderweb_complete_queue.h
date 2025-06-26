#pragma once

#include <cassert>
#include <cstdio>
#include <functional>
#include <memory>

namespace spiderweb {
class AsyncCompleteQueue;
class AsyncQueueContext;

class AsyncOperation {
 public:
  using Ptr = std::unique_ptr<AsyncOperation>;

  enum class State : uint8_t {
    kPending,
    kStarted,
    kDone,
    kCanceled,
    kShutDown,
  };

  virtual ~AsyncOperation() = default;

  virtual void Start() = 0;

 protected:
  void Done();

 private:
  std::shared_ptr<AsyncQueueContext> ctx;
  State                             state = State::kPending;

  friend class AsyncCompleteQueue;
  friend class AsyncQueueContext;
};

class AsyncShutDownOperation : public AsyncOperation {};

class AsyncCompleteQueue {
 public:
  explicit AsyncCompleteQueue();

  ~AsyncCompleteQueue();

  AsyncCompleteQueue(const AsyncCompleteQueue &) = delete;

  AsyncCompleteQueue &operator=(const AsyncCompleteQueue &) = delete;

  void Commit(AsyncOperation::Ptr                                              op,
              std::function<void(AsyncOperation::Ptr, AsyncOperation::State)> done);

  void Open();

  void ShutDown();

  bool IsShutDown() const;

  class Private;
  std::unique_ptr<Private> d;

  friend class AsyncOperation;
};

}  // namespace spiderweb
