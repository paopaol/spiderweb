#include "spiderweb/core/spiderweb_waiter.h"

#include "gtest/gtest.h"

TEST(Waiter, WaitCrossThread) {
  spiderweb::Waiter<bool> waiter;

  std::thread t([&]() {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    waiter.Notify(true);
  });
  EXPECT_TRUE(waiter.Wait());
  EXPECT_TRUE(waiter.Wait());
  t.join();
}
