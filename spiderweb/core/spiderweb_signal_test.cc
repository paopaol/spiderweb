#include "spiderweb/core/spiderweb_signal.h"

#include <csignal>

#include "gtest/gtest.h"
#include "spiderweb/core/spiderweb_eventloop.h"

TEST(SignalTest, Emit) {
  spiderweb::EventLoop loop;
  auto                 signals = spiderweb::MakeObject<spiderweb::Signals>(SIGINT);

  bool called = false;
  spiderweb::Object::Connect(signals.get(), &spiderweb::Signals::Triggered, signals.get(),
                             [&](int sig) {
                               called = true;
                               EXPECT_EQ(SIGINT, sig);
                               signals.reset();
                               loop.Exit(0);
                             });

  loop.RunAfter(100, []() { raise(SIGINT); });

  loop.ExecEx();

  EXPECT_TRUE(called);
}
