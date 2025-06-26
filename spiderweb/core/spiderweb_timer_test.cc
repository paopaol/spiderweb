#include "spiderweb/core/spiderweb_timer.h"

#include "gtest/gtest.h"
#include "spiderweb/core/spiderweb_eventloop.h"

spiderweb::EventLoop *g_loop = nullptr;

class Reciver : public spiderweb::Object {
 public:
  void ontimeout() {
    g_loop->Quit();
  }
};

TEST(spiderweb_timer, State) {
  spiderweb::EventLoop loop;

  auto timer = std::make_shared<spiderweb::Timer>();

  timer->SetInterval(10000);

  EXPECT_FALSE(timer->IsRunning());
  EXPECT_EQ(timer->Interval(), 10000);

  timer->Start();
  EXPECT_TRUE(timer->IsRunning());
}

TEST(spiderweb_timer, Timeout) {
  spiderweb::EventLoop loop;
  g_loop = &loop;

  auto    timer = std::make_shared<spiderweb::Timer>();
  Reciver reciver;

  bool called = false;

  spiderweb::Object::Connect(timer.get(), &spiderweb::Timer::timeout, &reciver, [&]() {
    called = true;
    g_loop->Quit();
  });

  timer->Start();

  loop.Exec();
  EXPECT_EQ(called, true);
}

TEST(spiderweb_timer, Stop) {
  spiderweb::EventLoop loop;
  g_loop = &loop;

  auto timer = std::make_shared<spiderweb::Timer>();

  timer->SetInterval(3000);
  timer->Start();

  timer->Stop();

  EXPECT_FALSE(timer->IsRunning());
}
