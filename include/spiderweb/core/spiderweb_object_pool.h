#pragma once

#include <cassert>
#include <deque>
#include <mutex>

namespace spiderweb {

template <typename T>
class ObjectBlock {
 public:
  bool HasFreeObject() const {
    return !queue_.empty();
  }

  T Get() {
    assert(HasFreeObject());

    auto e = std::move(queue_.front());
    queue_.pop_front();

    return std::move(e);
  }

  void Put(T &&v) {
    queue_.push_back(std::forward<T>(v));
  }

  std::size_t Size() const {
    return queue_.size();
  }

 private:
  std::deque<T> queue_;
};

template <typename T, std::size_t BlockSize>
class ObjectBlockQueue {
 public:
  using BlockType = ObjectBlock<T>;

  BlockType GetBlock() {
    {
      std::lock_guard<std::mutex> l(mutex_);
      if (!queue_.empty()) {
        BlockType block = std::move(queue_.front());
        queue_.pop_front();

        return std::move(block);
      }
    }

    return CreateFreeBlock();
  }

  void PutBlock(BlockType &&block) {
    std::lock_guard<std::mutex> l(mutex_);
    queue_.push_back(std::forward<BlockType>(block));
  }

 private:
  BlockType CreateFreeBlock() const {
    BlockType block;

    for (std::size_t i = 0; i < BlockSize; ++i) {
      block.Put(T());
    }

    return block;
  }

  std::mutex            mutex_;
  std::deque<BlockType> queue_;
};

template <typename T, std::size_t BlockSize = 64>
class ObjectPool {
 public:
  static T Get();

  static void Put(T v);

 private:
  static ObjectBlockQueue<T, BlockSize> shared_;
  static thread_local ObjectBlock<T>    pri_;
};

template <typename T, std::size_t BlockSize>
ObjectBlockQueue<T, BlockSize> ObjectPool<T, BlockSize>::shared_;

template <typename T, std::size_t BlockSize>
thread_local ObjectBlock<T> ObjectPool<T, BlockSize>::pri_;

template <typename T, std::size_t BlockSize>
T ObjectPool<T, BlockSize>::Get() {
  if (pri_.HasFreeObject()) {
    return pri_.Get();
  }
  pri_ = std::move(shared_.GetBlock());
  return pri_.Get();
}

template <typename T, std::size_t BlockSize>
void ObjectPool<T, BlockSize>::Put(T v) {
  pri_.Put(std::move(v));

  if (pri_.Size() >= BlockSize * 2) {
    shared_.PutBlock(std::move(pri_));
  }
}

}  // namespace spiderweb
