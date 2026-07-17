#include "spiderweb/core/spiderweb_future.h"

#include <gtest/gtest.h>

#include <cstdio>
#include <vector>

#include "spiderweb/core/spiderweb_eventloop.h"
#include "spiderweb/core/spiderweb_object.h"
#include "spiderweb/core/spiderweb_process.h"

namespace spiderweb {

template <typename T>
Future<T> make_process_future(std::vector<std::string> cmdline, Object* parent = nullptr) {
  Future<T> fut;

  auto* proc = new Process(parent);
  proc->SetProgram(std::move(cmdline));

  Object::Connect(proc, &Process::Stopped, proc, [fut, proc](int exit) mutable {
    proc->DeleteLater();
    fut.Resolve(exit);
  });

  Object::Connect(
      proc, &Process::BytesRead, proc,
      [](Process::Channnel, const io::BufferReader& reader) mutable { reader.Skip(reader.Len()); });

  proc->Start();

  return fut;
}

class FutureTest : public testing::Test {
 public:
  void TearDown() override {};

  EventLoop   loop;
  Future<int> future;
};

TEST_F(FutureTest, VoidInput_VoidOutput) {
  spiderweb::Future<void> f;
  bool                    called = false;

  f.Then([&]() { called = true; });
  f.Resolve();

  EXPECT_TRUE(called);
}

TEST_F(FutureTest, VoidInput_NoneVoidOutput) {
  spiderweb::Future<void> f;
  int                     result = 0;

  f.Then([]() { return 1; }).Then([&](int v) { result = v; });
  f.Resolve();
  EXPECT_EQ(result, 1);
}

TEST_F(FutureTest, Input_VoidOutput) {
  spiderweb::Future<int> f;
  int                    received_input = -1;
  bool                   next_void_called = false;

  f.Then([&](int val) { received_input = val; }).Then([&]() { next_void_called = true; });
  f.Resolve(42);
  EXPECT_EQ(received_input, 42);
  EXPECT_TRUE(next_void_called);
}

TEST_F(FutureTest, Input_NonVoidOutput) {
  spiderweb::Future<int> f;
  std::string            captured = "";

  f.Then([](int val) {
     (void)val;
     return std::string("hello");
   }).Then([&](std::string s) { captured = std::move(s); });

  f.Resolve(100);
  EXPECT_EQ(captured, "hello");
}

TEST_F(FutureTest, ResolveBeforeThen) {
  spiderweb::Future<int> f;
  int                    result = 0;

  f.Resolve(99);

  f.Then([&](int val) { result = val; });

  EXPECT_EQ(result, 99);
}

TEST_F(FutureTest, DoubleResolveDefense) {
  spiderweb::Future<int> f;
  int                    call_count = 0;
  int                    final_value = 0;

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

TEST_F(FutureTest, MakeFuture) {
  future = make_process_future<int>({"ls"});

  int code = -1;
  future
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

TEST_F(FutureTest, OnError) {
  spiderweb::Future<void> f;
  bool                    ok_called = false;
  bool                    error_called = false;

  f.Then([&]() {
     ok_called = true;
     return true;
   }).OnError([&](const std::string& err) {
    EXPECT_EQ(err, "123");

    error_called = true;
    return 3;
  });

  f.ResolveError(std::string("123"));

  EXPECT_FALSE(ok_called);
  EXPECT_TRUE(error_called);
}

}  // namespace spiderweb
