#pragma once

#include <algorithm>
#include <mutex>
#include <queue>

namespace spiderweb {

/// a thread safe bounded queue
template <typename T>
class SyncUnboundedQueue {
 public:
  using Element = T;

  template <typename E, typename = std::deque<E>>
  using Queue = std::queue<E>;

  using ElementQueue = Queue<Element>;

  using Mutex = std::mutex;

  template <typename Mutex>
  using Lock = std::unique_lock<Mutex>;

  SyncUnboundedQueue() = default;

  SyncUnboundedQueue(const SyncUnboundedQueue &other) {
    Lock<Mutex> lock(other.mutex_);
    queue_ = other.queue_;
  }

  SyncUnboundedQueue &operator=(const SyncUnboundedQueue &rh) = delete;

  template <typename U>
  inline void PushBack(U &&element) {
    Lock<Mutex> lock(mutex_);
    queue_.push(std::forward<U>(element));
  }

  inline void PopFront(Element &output) {
    Lock<Mutex> lock(mutex_);
    output = std::move(queue_.front());
    queue_.pop();
  }

  inline std::size_t Size() const {
    Lock<Mutex> lock(mutex_);
    return queue_.size();
  }

  inline bool Empty() const {
    Lock<Mutex> lock(mutex_);
    return queue_.empty();
  }

  template <typename U>
  inline void Swap(Queue<U> &in) {
    Lock<Mutex> lock(mutex_);
    queue_.swap(in);
  }

  /// call f with first element
  template <typename F>
  void FrontDo(F &&f) const {
    Lock<Mutex> lock(mutex_);
    std::forward<F>(f)(queue_.front());
  }

  /// call f with first element
  template <typename F>
  void FrontDo(F &&f) {
    Lock<Mutex> lock(mutex_);
    std::forward<F>(f)(queue_.front());
  }

  /// call f with last element
  template <typename F>
  void BackDo(F &&f) const {
    Lock<Mutex> lock(mutex_);
    std::forward<F>(f)(queue_.back());
  }

  /// call f with last element
  template <typename F>
  void BackDo(F &&f) {
    Lock<Mutex> lock(mutex_);
    std::forward<F>(f)(queue_.back());
  }

 private:
  mutable Mutex mutex_;
  ElementQueue  queue_;
};
}  // namespace spiderweb
