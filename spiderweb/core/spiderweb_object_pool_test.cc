#include "spiderweb/core/spiderweb_object_pool.h"

#include <thread>

#include "gtest/gtest.h"

namespace spiderweb {

class BigObject {
 public:
  using Ptr = std::shared_ptr<BigObject>;
  using UPtr = std::unique_ptr<BigObject>;

  BigObject() {
    data.resize(2000);
  }

  uint32_t              index = 0;
  std::vector<uint32_t> data;
};

class ObjectPoolTest : public testing::Test {
 public:
  using Pool = spiderweb::ObjectPool<BigObject::Ptr>;
  using UptrPool = spiderweb::ObjectPool<BigObject::UPtr, 100>;

  Pool pool;
};

TEST_F(ObjectPoolTest, GetPut) {
  for (int i = 0; i < 1000000; ++i) {
    auto v = Pool::Get();
    if (!v) {
      v = std::make_shared<BigObject>();
    }
    EXPECT_EQ(v.use_count(), 1);

    Pool::Put(v);
  }
}

TEST_F(ObjectPoolTest, UPtrGetPut) {
  int call_times = 0;
  for (int i = 0; i < 1000; ++i) {
    auto v = UptrPool::Get();
    if (!v) {
      v = std::make_unique<BigObject>();
      call_times++;
    }
    EXPECT_TRUE(v);

    UptrPool::Put(std::move(v));
  }
  EXPECT_EQ(call_times, 100);
}

TEST_F(ObjectPoolTest, ThreadsGetPut) {
  std::vector<std::thread> threads;

  for (int a = 0; a < 10; a++) {
    std::thread t([&]() {
      for (int i = 0; i < 1000000; ++i) {
        auto v = Pool::Get();
        if (!v) {
          v = std::make_shared<BigObject>();
        }
        EXPECT_EQ(v.use_count(), 1);

        Pool::Put(v);
      }
    });
    threads.push_back(std::move(t));
  }

  for (auto &t : threads) {
    t.join();
  }
}

}  // namespace spiderweb
