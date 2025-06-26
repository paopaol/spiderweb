#include "spiderweb/core/spiderweb_signal.h"

#include <csignal>

#include "gtest/gtest.h"
#include "spiderweb/core/spiderweb_eventloop.h"

TEST(SignalTest, Emit) {
  spiderweb::EventLoop loop;
  auto                 signals = std::make_shared<spiderweb::Signals>(SIGINT);

  bool called;
  spiderweb::Object::Connect(signals.get(), &spiderweb::Signals::Triggered, signals.get(),
                             [&](int sig) {
                               called = true;
                               EXPECT_EQ(SIGINT, sig);
                               loop.Exit(0);
                             });

  signals->Start();

  // loop.RunAfter(1000, []() { kill(getpid(), SIGINT); });

  loop.Exec();

  EXPECT_TRUE(called);
}
