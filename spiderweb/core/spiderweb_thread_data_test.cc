
#include "spiderweb/core/spiderweb_thread_data.h"

#include <sstream>

#include "fmt/format.h"
#include "gtest/gtest.h"
#include "spiderweb/core/spiderweb_eventloop.h"
#include "spiderweb/core/spiderweb_thread.h"
#include "spiderweb/core/spiderweb_timer.h"

class Service {
 public:
  Service() = default;

  explicit Service(int current, std::string name) : current_(current), name_(std::move(name)) {
    SetUp();
  }

  ~Service() {
    timer_->DeleteLater();
  }

  void SetUp() {
    if (timer_) {
      fmt::print("timer_ already started\n");
      return;
    }
    timer_ = new spiderweb::Timer();

    spiderweb::Timer::Connect(timer_, &spiderweb::Timer::timeout, timer_, [this]() {
      std::stringstream s;
      s << std::this_thread::get_id();
      current_++;
      fmt::print("{} {}\n", s.str(), current_);
    });

    timer_->SetSingalShot(false);
    timer_->SetInterval(1000);
    timer_->Start();
  }

  std::string GetName() const {
    return name_;
  }

  spiderweb::Thread::Id GetId() const {
    return std::this_thread::get_id();
  }

  bool IsRunning() const {
    return timer_->IsRunning();
  }

  void Stop() {
    std::this_thread::sleep_for(std::chrono::seconds(3));
    timer_->Stop();
  }

  int SetCurrent(int current) {
    current_ = current;
    return current_;
  }

 private:
  spiderweb::Timer *timer_ = nullptr;
  int               current_ = 0;
  std::string       name_;
};

TEST(a, b) {
  spiderweb::EventLoop loop;
  spiderweb::Thread    thread;

  thread.Start();

  auto *server = spiderweb::ThreadData<Service>::New(&thread, 1, "name");

  std::vector<std::string> ss = {"123", "666"};

  EXPECT_EQ(server->Call(&Service::GetName), "name");
  EXPECT_NE(std::this_thread::get_id(), server->Call(&Service::GetId));

  EXPECT_TRUE(server->Call(&Service::IsRunning));

  EXPECT_EQ(server->Call(&Service::SetCurrent, 123), 123);

  loop.Exec();
}

TEST(a, AsyncDelete) {
  spiderweb::EventLoop loop;
  spiderweb::Thread    thread;

  thread.Start();

  auto *server = spiderweb::ThreadData<Service>::New(&thread, 1, "name");

  spiderweb::ThreadData<Service>::AsyncDelete(server, []() { fmt::print("destory called \n"); });

  loop.Exec();
}

TEST(a, AsyncCall) {
  spiderweb::EventLoop loop;
  spiderweb::Thread    thread;

  thread.Start();

  auto *server = spiderweb::ThreadData<Service>::New(&thread, 1, "xiaoming");

  server->AsyncCallThen(&Service::GetName,
                        [](const std::string &name) { fmt::print("name {}\n", name); });

  server->AsyncCallThen(&Service::SetUp, []() {});

  loop.Exec();
}
