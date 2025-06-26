#include "spiderweb/core/spiderweb_notify_spy.h"

#include "gtest/gtest.h"
#include "spiderweb/core/spiderweb_eventloop.h"
#include "spiderweb/core/spiderweb_timer.h"

TEST(spiderweb_notify_spy, capture) {
  spiderweb::EventLoop loop;
  auto                 timer = std::make_shared<spiderweb::Timer>();

  spiderweb::NotifySpy spy(timer.get(), &spiderweb::Timer::timeout);

  timer->Start();
  spy.Wait();

  loop.QueueTask([&]() { loop.Quit(); });

  loop.Exec();

  EXPECT_EQ(spy.Count(), 1);
}
