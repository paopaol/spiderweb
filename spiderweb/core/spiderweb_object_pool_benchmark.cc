#include "benchmark/benchmark.h"
#include "spiderweb/core/spiderweb_object_pool.h"

template <typename T>
class GlobalMutexObjectPool {
 public:
  static T Get() {
    std::lock_guard<std::mutex> l(mutex_);

    if (pri_.empty()) {
      return T();
    }
    auto v = std::move(pri_.front());
    pri_.pop_front();

    return std::move(v);
  }

  static void Put(T v) {
    std::lock_guard<std::mutex> l(mutex_);

    pri_.push_back(std::move(v));
  }

 private:
  static std::mutex    mutex_;
  static std::deque<T> pri_;
};

template <typename T>
std::mutex GlobalMutexObjectPool<T>::mutex_;
template <typename T>
std::deque<T> GlobalMutexObjectPool<T>::pri_;

class BigObject {
 public:
  BigObject() {
    data.resize(2000);
  }

  uint32_t              index = 0;
  std::vector<uint32_t> data;
};

static void BM_NoPool(benchmark::State& state) {
  for (auto _ : state) {
    auto* v = new BigObject();
    delete v;
  }
}
BENCHMARK(BM_NoPool)->ThreadRange(1, 10);

static void BM_GlobalMutexPool(benchmark::State& state) {
  using Pool = GlobalMutexObjectPool<BigObject*>;

  for (auto _ : state) {
    auto* v = Pool::Get();
    if (!v) {
      v = new BigObject();
    }
    Pool::Put(v);
  }
}
BENCHMARK(BM_GlobalMutexPool)->ThreadRange(1, 10);

static void BM_SpiderwebPool(benchmark::State& state) {
  using Pool = spiderweb::ObjectPool<BigObject*>;

  for (auto _ : state) {
    auto* v = Pool::Get();
    if (!v) {
      v = new BigObject();
    }
    Pool::Put(v);
  }
}
BENCHMARK(BM_SpiderwebPool)->ThreadRange(1, 10);
