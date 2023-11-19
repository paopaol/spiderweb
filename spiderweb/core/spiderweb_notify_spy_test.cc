#include "spiderweb/core/spiderweb_notify_spy.h"

#include "gtest/gtest.h"
#include "spiderweb/core/spiderweb_eventloop.h"
#include "spiderweb/core/spiderweb_timer.h"

TEST(spiderweb_notify_spy, capture) {
  spiderweb::EventLoop loop;
  spiderweb::Timer     timer;

  spiderweb::NotifySpy spy(&timer, &spiderweb::Timer::timeout);

  timer.Start();
  spy.Wait();

  loop.QueueTask([&]() { loop.Quit(); });

  loop.Exec();

  EXPECT_EQ(spy.Count(), 1);
}
