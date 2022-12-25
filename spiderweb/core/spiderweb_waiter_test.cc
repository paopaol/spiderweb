#include "spiderweb/core/spiderweb_waiter.h"

#include "gtest/gtest.h"
#include "spiderweb/core/spiderweb_thread.h"

TEST(Waiter, WaitCrossThread) {
  spiderweb::Waiter<bool> waiter;

  std::thread t([&]() {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    waiter.Notify(true);
  });
  EXPECT_TRUE(waiter.Wait());
  t.join();
}

TEST(WaitGroup, Wait) {
  using ThreadPtr = std::shared_ptr<spiderweb::Thread>;

  spiderweb::WaitGroup group(5);

  std::vector<ThreadPtr> threads;

  for (auto i = 0; i < 5; ++i) {
    auto thread = std::make_shared<spiderweb::Thread>();
    threads.push_back(thread);
    thread->Start();
    thread->QueueTask(std::bind(&spiderweb::WaitGroup::Done, &group));
  }

  group.Wait();
}
