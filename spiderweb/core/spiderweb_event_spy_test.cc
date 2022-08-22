#include "core/spiderweb_event_spy.h"

#include "core/spiderweb_eventloop.h"
#include "core/spiderweb_timer.h"
#include "gtest/gtest.h"

TEST(spiderweb_event_spy, capture) {
  spiderweb::EventLoop loop;
  spiderweb::Timer     timer;

  spiderweb::EventSpy spy(&timer, &spiderweb::Timer::timeout);

  spiderweb::Object::Connect(&timer, &spiderweb::Timer::timeout, &loop,
                             [&]() { loop.Quit(); });

  timer.Start();
  spy.Wait();

  loop.Exec();

  EXPECT_EQ(spy.Count(), 1);
}
