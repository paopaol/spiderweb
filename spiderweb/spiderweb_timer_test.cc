#include "spiderweb_timer.h"

#include "gtest/gtest.h"
#include "spiderweb_eventloop.h"

spiderweb::EventLoop *g_loop = nullptr;

class Reciver : public spiderweb::Base {
 public:
  void ontimeout() { g_loop->Quit(); }
};

TEST(spiderweb_timer, connect) {
  spiderweb::EventLoop loop;
  g_loop = &loop;

  spiderweb::Timer timer;
  Reciver          reciver;

  bool called = false;

  spiderweb::Base::Connect(timer.timeout, &reciver, [&]() { called = true; });
  spiderweb::Base::Connect(timer.timeout, &reciver, &Reciver::ontimeout);

  timer.Start(1000);

  loop.Exec();
  EXPECT_EQ(called, true);
}
