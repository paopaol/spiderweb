#include "spiderweb/core/spiderweb_promise.h"

#include <gtest/gtest.h>

#include <array>
#include <cstdint>
#include <cstdio>
#include <string>
#include <vector>

#include "spiderweb/core/spiderweb_error_code.h"
#include "spiderweb/core/spiderweb_eventloop.h"
#include "spiderweb/core/spiderweb_object.h"
#include "spiderweb/core/spiderweb_process.h"

namespace spiderweb {

template <typename T>
Promise<T> make_process_promise(std::vector<std::string> cmdline, Object* parent = nullptr) {
  Promise<T> promise;

  auto* proc = new Process(parent);
  proc->SetProgram(std::move(cmdline));

  Object::Connect(proc, &Process::Stopped, proc, [promise, proc](int exit) mutable {
    proc->DeleteLater();
    promise.Resolve(exit);
  });

  Object::Connect(
      proc, &Process::BytesRead, proc,
      [](Process::Channnel, const io::BufferReader& reader) mutable { reader.Skip(reader.Len()); });

  auto ec = proc->Start();
  if (ec) {
    promise.Reject(ec);
  }

  return promise;
}

class PromiseTest : public testing::Test {
 public:
  void TearDown() override {};

  EventLoop    loop;
  Promise<int> promise;
};

TEST_F(PromiseTest, ErrorTest) {
  {
    Error e;

    EXPECT_FALSE(e);
  }
  {
    auto err = Error::Make(static_cast<uint8_t>(1));
    EXPECT_TRUE(err);
  }
}

TEST_F(PromiseTest, ErrorIs) {
  {
    auto err = Error::Make(static_cast<uint8_t>(1));

    EXPECT_TRUE(err.Is<uint8_t>());
    EXPECT_FALSE(err.Is<uint16_t>());
  }
  {
    auto err = Error::Make(std::string("123"));

    EXPECT_TRUE(err.Is<std::string>());
  }
}

TEST_F(PromiseTest, ErrorGet) {
  {
    auto err = Error::Make(static_cast<uint8_t>(1));
    EXPECT_EQ(err.Get<uint8_t>(), 1);
  }
  {
    auto err = Error::Make(std::string("123"));
    EXPECT_EQ(err.Get<std::string>(), "123");
  }
}

TEST_F(PromiseTest, ErrorRef) {
  using Array = std::array<uint8_t, 1>;

  Array array = {1};

  {
    auto err = Error::Make(array);
    EXPECT_TRUE(err.Is<Array>());
  }
  {
    auto& ref = array;
    auto  err = Error::Make(ref);
    EXPECT_TRUE(err.Is<Array>());
  }
}

TEST_F(PromiseTest, VoidInput_VoidOutput) {
  spiderweb::Promise<void> f;
  bool                     called = false;

  f.Then([&]() { called = true; });
  f.Resolve();

  EXPECT_TRUE(called);
}

TEST_F(PromiseTest, VoidInput_NoneVoidOutput) {
  spiderweb::Promise<void> f;
  int                      result = 0;

  f.Then([]() { return 1; }).Then([&](int v) { result = v; });
  f.Resolve();
  EXPECT_EQ(result, 1);
}

TEST_F(PromiseTest, Input_VoidOutput) {
  spiderweb::Promise<int> f;
  int                     received_input = -1;
  bool                    next_void_called = false;

  f.Then([&](int val) { received_input = val; }).Then([&]() { next_void_called = true; });
  f.Resolve(42);
  EXPECT_EQ(received_input, 42);
  EXPECT_TRUE(next_void_called);
}

TEST_F(PromiseTest, Input_NonVoidOutput) {
  spiderweb::Promise<int> f;
  std::string             captured = "";

  f.Then([](int val) {
     (void)val;
     return std::string("hello");
   }).Then([&](std::string s) { captured = std::move(s); });

  f.Resolve(100);
  EXPECT_EQ(captured, "hello");
}

TEST_F(PromiseTest, ResolveBeforeThen) {
  spiderweb::Promise<int> f;
  int                     result = 0;

  f.Resolve(99);

  f.Then([&](int val) { result = val; });

  EXPECT_EQ(result, 99);
}

TEST_F(PromiseTest, DoubleResolveDefense) {
  spiderweb::Promise<int> f;
  int                     call_count = 0;
  int                     final_value = 0;

  f.Then([&](int val) {
    call_count++;
    final_value = val;
  });

  // 第一次触发
  f.Resolve(10);
  // 第二次触发（必须被无视）
  f.Resolve(20);

  EXPECT_EQ(call_count, 1);
  EXPECT_EQ(final_value, 10);  // 必须保持第一次的值
}

TEST_F(PromiseTest, MakePromise) {
  promise = make_process_promise<int>({"ls"});

  int code = -1;
  promise
      .Then([&](int exit) {
        loop.Quit();
        return exit;
      })
      .Then([](int code) { return 10 + code; })
      .Then([&](int v) { code = v; })
      .Then([&]() { printf("%d\n", code); });

  loop.ExecEx();
  EXPECT_EQ(code, 10);
}

TEST_F(PromiseTest, OnError) {
  spiderweb::Promise<void> f;
  bool                     ok_called = false;
  bool                     error_called = false;

  f.Then([&]() {
     ok_called = true;
     return true;
   }).OnError(Tag<std::string>{}, [&](const std::string& err) {
    EXPECT_EQ(err, "123");

    error_called = true;
    return 3;
  });

  f.Reject(std::string("123"));

  EXPECT_FALSE(ok_called);
  EXPECT_TRUE(error_called);
}

static Promise<int> create(int) {
  return make_process_promise<int>({"ls", "111"});
}

TEST_F(PromiseTest, MakePromiseThen) {
  promise = make_process_promise<int>({"ls", "/not"});

  promise.Then(create)
      .Then([&](int status) {
        printf("%d\n", status);
        loop.Quit();
      })
      .OnError(Tag<ErrorCode>{}, [&](const ErrorCode& ec) {
        puts(ec.FormatedMessage().c_str());
        loop.Quit();
        return 1;
      });

  loop.ExecEx();
}

}  // namespace spiderweb
