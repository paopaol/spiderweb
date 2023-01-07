#pragma once

#include <mutex>

namespace spiderweb {

template <typename T>
class Synchronized;

template <typename T>
class SynchronizedLocker {
 public:
  using DataType = T;

  explicit SynchronizedLocker(Synchronized<DataType> *ref) : ref_(ref) {
    ref_->mutex_.lock();
  }

  SynchronizedLocker(SynchronizedLocker &&rth) noexcept {
    ref_ = std::move(rth.ref_);
    rth.ref_ = nullptr;
  }

  ~SynchronizedLocker() {
    if (ref_) {
      ref_->mutex_.unlock();
    }
  }

  SynchronizedLocker &operator=(const SynchronizedLocker &) = delete;

  inline T *operator->() {
    return &ref_->data_;
  }

  inline const T *operator->() const {
    return &ref_->data_;
  }

 private:
  Synchronized<DataType> *ref_{nullptr};
};

template <typename T>
class Synchronized {
 public:
  using DataType = T;
  using LockType = std::mutex;

  template <typename... Args>
  explicit Synchronized(Args &&...args) : data_(std::forward<Args>(args)...) {
  }

  Synchronized &operator=(const Synchronized &) = delete;

  inline SynchronizedLocker<DataType> Lock() {
    return std::move(SynchronizedLocker<DataType>(this));
  }

  inline SynchronizedLocker<DataType> Lock() const {
    return std::move(SynchronizedLocker<const DataType>(this));
  }

  template <class Function>
  inline auto WithLock(Function &&function) {
    auto l(Lock());
    return function(&data_);
  }

  template <class Function>
  inline auto WithLock(Function &&function) const {
    auto l(Lock());
    return function(&data_);
  }

 private:
  LockType mutex_;
  DataType data_{};

  template <typename U>
  friend class SynchronizedLocker;
};
}  // namespace spiderweb
