#include "spiderweb_eventloop.h"

#include "gtest/gtest.h"

TEST(spiderweb_EventLoop, construct) { spiderweb::EventLoop loop; }

TEST(spiderweb_EventLoop, exit) {
  spiderweb::EventLoop loop;
  loop.Exit(127);
  EXPECT_EQ(127, loop.Exec());
}

TEST(spiderweb_EventLoop, exec) {
  spiderweb::EventLoop loop;

  // loop.exec();
}
