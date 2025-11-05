#include "spiderweb/io/spiderweb_named_pipe.h"

#include "gtest/gtest.h"
#include "spiderweb/core/spiderweb_eventloop.h"
#include "spiderweb/core/spiderweb_notify_spy.h"

TEST(NamedPipe, Open) {
  spiderweb::EventLoop loop;

  spiderweb::NamedPipe pipe("/tmp/testp");
  spiderweb::NotifySpy spy(&pipe, &spiderweb::NamedPipe::OpenSuccess);

  pipe.Open();

  spy.Wait();
  EXPECT_EQ(spy.Count(), 0);
  // std::vector<uint8_t> v = {1, 2, 3};
  // pipe.Write(v);
}
