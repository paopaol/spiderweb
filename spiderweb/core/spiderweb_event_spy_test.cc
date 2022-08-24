#include "core/spiderweb_event_spy.h"

#include "core/spiderweb_eventloop.h"
#include "core/spiderweb_timer.h"
#include "gtest/gtest.h"

TEST(spiderweb_event_spy, capture) {
  spiderweb::EventLoop loop;
  spiderweb::Timer     timer;

  spiderweb::EventSpy spy(&timer, &spiderweb::Timer::timeout);

  timer.Start();
  spy.Wait();

  loop.QueueTask([&]() { loop.Quit(); });

  loop.Exec();

  EXPECT_EQ(spy.Count(), 1);
}
