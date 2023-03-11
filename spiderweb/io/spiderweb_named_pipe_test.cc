#include "spiderweb/io/spiderweb_named_pipe.h"

#include "absl/types/variant.h"
#include "fmt/format.h"
#include "gtest/gtest.h"
#include "spiderweb/core/spiderweb_event_spy.h"
#include "spiderweb/core/spiderweb_eventloop.h"

TEST(NamedPipe, Open) {
  spiderweb::EventLoop loop;

  spiderweb::NamedPipe pipe("/tmp/testp");
  spiderweb::EventSpy  spy(&pipe, &spiderweb::NamedPipe::OpenSuccess);

  pipe.Open();

  spy.Wait();
  EXPECT_EQ(spy.Count(), 1);
  std::vector<uint8_t> v = {1, 2, 3};
  pipe.Write(v);
}
