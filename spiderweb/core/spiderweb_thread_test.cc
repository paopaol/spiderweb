#include "spiderweb/core/spiderweb_thread.h"

#include <chrono>
#include <thread>

#include "absl/memory/memory.h"
#include "gtest/gtest.h"
#include "spiderweb/core/spiderweb_timer.h"
#include "spiderweb/core/spiderweb_waiter.h"

class ThreadTest : public testing::Test {
 public:
  using TimerPtr = std::unique_ptr<spiderweb::Timer>;

  spiderweb::Waiter<bool> waiter;
  spiderweb::Thread       r;
  TimerPtr                timer;
  bool                    called = false;
};

TEST_F(ThreadTest, Run) {
  const auto task = [&]() {
    timer = absl::make_unique<spiderweb::Timer>();

    timer->SetInterval(100);
    timer->SetSingalShot(true);

    spiderweb::Object::Connect(timer.get(), &spiderweb::Timer::timeout, timer.get(), [&]() {
      called = true;
      waiter.Notify(true);
    });

    timer->Start();
  };

  EXPECT_FALSE(r.IsRunning());
  r.Start();
  EXPECT_TRUE(r.IsRunning());
  r.QueueTask(task);

  waiter.Wait();
  EXPECT_TRUE(called);

  r.Quit();
  EXPECT_FALSE(r.IsRunning());
}

TEST_F(ThreadTest, MoveConstruct) {
  spiderweb::Thread a;

  a.Start();

  spiderweb::Thread b(std::move(a));

  EXPECT_TRUE(b.IsRunning());
  EXPECT_FALSE(a.IsRunning());
}

TEST_F(ThreadTest, MoveCopy) {
  spiderweb::Thread a;
  spiderweb::Thread b;

  a.Start();
  b.Start();

  b = std::move(a);

  EXPECT_TRUE(b.IsRunning());
}
