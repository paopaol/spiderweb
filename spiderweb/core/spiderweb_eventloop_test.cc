#include "core/spiderweb_eventloop.h"

#include "absl/memory/memory.h"
#include "core/spiderweb_timer.h"
#include "gtest/gtest.h"

TEST(spiderweb_EventLoop, construct) {
  spiderweb::EventLoop loop;
}

TEST(spiderweb_EventLoop, exit) {
  spiderweb::EventLoop loop;
  loop.Exit(127);
  EXPECT_EQ(127, loop.Exec());
}

TEST(spiderweb_EventLoop, exec) {
  spiderweb::EventLoop loop;

  // loop.exec();
}

class TestObject : public spiderweb::Object {
 public:
  explicit TestObject(spiderweb::Object *parent = nullptr) : spiderweb::Object(parent) {
  }

  void SetState(bool state) {
    if (state != state_) {
      state_ = state;
      statedChanged(state_);
    }
  }

  spiderweb::Notify<void(bool state)> statedChanged;

 private:
  bool state_ = false;
};

class ReciverObject : public spiderweb::Object {
 public:
  explicit ReciverObject(spiderweb::Object *parent = nullptr) : spiderweb::Object(parent) {
  }

  void SetValue(bool state) {
    state_ = state;
    called_thread_ = std::this_thread::get_id();
  }

  std::thread::id CalledThread() const {
    return called_thread_;
  }

  bool State() const {
    return state_;
  }

 private:
  bool            state_ = false;
  std::thread::id called_thread_;
};

TEST(spiderweb_EventLoop, multithread_emit_event_class_member) {
  spiderweb::EventLoop app;

  TestObject    test;
  ReciverObject reciver;

  spiderweb::Object::Connect(&test, &TestObject::statedChanged, &reciver, &ReciverObject::SetValue);

  spiderweb::Timer timer;
  timer.SetInterval(100);

  spiderweb::Object::Connect(&timer, &spiderweb::Timer::timeout, &app, [&]() { app.Quit(); });

  std::thread subthread([&]() {
    test.SetState(true);

    app.QueueTask([&]() { timer.Start(); });
  });

  app.Exec();

  subthread.join();

  bool called = reciver.State();
  bool calledInAppThread = reciver.CalledThread() == reciver.ThreadId();
  EXPECT_TRUE(called);
  EXPECT_TRUE(calledInAppThread);
}

TEST(spiderweb_EventLoop, multithread_emit_event_lambda) {
  spiderweb::EventLoop app;

  TestObject test;

  bool called = false;
  bool calledInAppThread = false;
  spiderweb::Object::Connect(&test, &TestObject::statedChanged, &test, [&](bool /*state*/) {
    called = true;
    calledInAppThread = test.ThreadId() == std::this_thread::get_id();
  });

  spiderweb::Timer timer;
  timer.SetInterval(100);

  spiderweb::Object::Connect(&timer, &spiderweb::Timer::timeout, &app, [&]() { app.Quit(); });

  std::thread subthread([&]() {
    test.SetState(true);

    app.QueueTask([&]() { timer.Start(); });
  });

  app.Exec();

  subthread.join();

  EXPECT_TRUE(called);
  EXPECT_TRUE(calledInAppThread);
}

class MyObject : public spiderweb::Object {
 public:
  explicit MyObject(spiderweb::Object *parent = nullptr) : spiderweb::Object(parent) {
    timer_ = absl::make_unique<spiderweb::Timer>(this);
  }

  std::unique_ptr<spiderweb::Timer> timer_;
};

TEST(spiderweb_EventLoop, destory) {
  spiderweb::EventLoop loop;

  MyObject object;
}
