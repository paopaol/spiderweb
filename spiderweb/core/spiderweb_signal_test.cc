#include "spiderweb/core/spiderweb_signal.h"

#include "gtest/gtest.h"
#include "spiderweb/core/spiderweb_eventloop.h"

TEST(SignalTest, Emit) {
  spiderweb::EventLoop loop;
  spiderweb::Signals   signals(SIGINT);

  bool called;
  spiderweb::Object::Connect(&signals, &spiderweb::Signals::Triggered, &signals, [&](int sig) {
    called = true;
    EXPECT_EQ(SIGINT, sig);
    loop.Exit(0);
  });

  kill(getpid(), SIGINT);

  loop.Exec();

  EXPECT_TRUE(called);
}
