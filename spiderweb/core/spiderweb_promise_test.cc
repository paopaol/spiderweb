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

// Error 移动语义
TEST_F(PromiseTest, ErrorMove) {
  auto err1 = Error::Make(std::string("hello"));
  EXPECT_TRUE(err1.Is<std::string>());

  auto err2 = std::move(err1);
  EXPECT_TRUE(err2.Is<std::string>());
  EXPECT_EQ(err2.Get<std::string>(), "hello");
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
    return true;
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
      });

  loop.ExecEx();
}

// OnError 正常值透传
TEST_F(PromiseTest, OnError_NormalValuePassThrough) {
  Promise<int> f;
  int          result = -1;

  f.Then([](int v) { return v + 1; })
      .OnError(Tag<std::string>{}, [](const std::string&) { return 0; })
      .Then([&](int v) { result = v; });

  f.Resolve(42);
  // Then 返回 43，OnError 透传，最后一个 Then 收到 43
  EXPECT_EQ(result, 43);
}

// OnError 类型不匹配时错误透传
TEST_F(PromiseTest, OnError_TypeMismatch_Propagate) {
  Promise<int> f;
  bool         timeout_handler_called = false;
  bool         final_error_received = false;

  f.Then([](int v) { return v; })
      .OnError(Tag<int>{},
               [&](const int&) {  // 期望 int 错误
                 timeout_handler_called = true;
                 return 0;
               })
      .OnError(Tag<std::string>{}, [&](const std::string& err) {  // 期望 string 错误
        final_error_received = true;
        EXPECT_EQ(err, "oops");
        return 0;
      });

  f.Reject(std::string("oops"));  // 实际是 string 错误
  // 第一个 OnError 不匹配，透传；第二个 OnError 匹配
  EXPECT_FALSE(timeout_handler_called);
  EXPECT_TRUE(final_error_received);
}

// AlreadyContinued 异常
TEST_F(PromiseTest, Then_DoubleRegistration_Throws) {
  Promise<int> f;
  f.Then([](int v) { return v; });

  EXPECT_THROW(f.Then([](int v) { return v; }), AlreadyContinued);
}

TEST_F(PromiseTest, OnError_DoubleRegistration_Throws) {
  Promise<int> f;
  f.Then([](int v) { return v; }).OnError(Tag<int>{}, [](const int&) { return 0; });

  // 在同一个 Promise 上再注册会抛异常
  // 但这里 OnError 是在 Then 返回的新 Promise 上，不是原 Promise
  // 需要构造直接在同一 Promise 上调两次的场景
  Promise<int> p;
  p.Then([](int v) { return v; });
  EXPECT_THROW(p.Then([](int v) { return v; }), AlreadyContinued);
}

// OnError 链式错误冒泡 错误沿链路一直冒泡直到被捕获：
TEST_F(PromiseTest, OnError_ErrorBubbling) {
  Promise<int> f;
  bool         error_caught = false;
  bool         then1_called = false;
  bool         then2_called = false;

  f.Then([&](int v) {
     then1_called = true;
     return v + 1;
   })
      .Then([&](int v) {
        then2_called = true;
        return v * 2;
      })  // 错误跳过这两级
      .OnError(Tag<std::string>{}, [&](const std::string& err) {
        error_caught = true;
        EXPECT_EQ(err, "fail");
        return -1;
      });

  f.Reject(std::string("fail"));
  EXPECT_TRUE(error_caught);
  EXPECT_FALSE(then2_called);
  EXPECT_FALSE(then1_called);
}

// OnError 返回 void（Promise<void> 的 OnError）
TEST_F(PromiseTest, OnError_VoidPromise) {
  Promise<void> f;
  bool          error_called = false;

  f.Then([]() {}).OnError(Tag<std::string>{}, [&](const std::string&) { error_called = true; });

  f.Reject(std::string("err"));
  EXPECT_TRUE(error_called);
}

// Reject 后 Then 的回调不执行
TEST_F(PromiseTest, Then_SkippedAfterReject) {
  Promise<int> f;
  bool         then_called = false;

  f.Then([&](int v) {
     then_called = true;
     return v;
   }).OnError(Tag<std::string>{}, [](const std::string&) { return 0; });

  f.Reject(std::string("err"));
  EXPECT_FALSE(then_called);
}

// 基本用法：3 个 Promise 全部 resolve
TEST_F(PromiseTest, All_AllResolved) {
  Promise<int> p1;
  Promise<int> p2;
  Promise<int> p3;

  bool finished = false;

  PromiseAll<int>({p1, p2, p3}).Then([&](std::vector<int> results) {
    finished = true;
    ASSERT_EQ(results.size(), 3u);
    EXPECT_EQ(results[0], 1);
    EXPECT_EQ(results[1], 2);
    EXPECT_EQ(results[2], 3);
  });

  // 按任意顺序 resolve
  p2.Resolve(2);
  p1.Resolve(1);
  p3.Resolve(3);

  EXPECT_TRUE(finished);
}

// // 结果顺序必须和传入顺序一致（即使 resolve 顺序不同）
// TEST_F(PromiseTest, All_ResultOrder) {
//   Promise<int> p1;
//   Promise<int> p2;
//   Promise<int> p3;
//
//   bool finished = false;
//
//   PromiseAll<int>({p1, p2, p3}).Then([&](std::vector<int> results) {
//     finished = true;
//     // 结果顺序 = 传入顺序，不是 resolve 顺序
//     EXPECT_EQ(results[0], 10);
//     EXPECT_EQ(results[1], 20);
//     EXPECT_EQ(results[2], 30);
//   });
//
//   p3.Resolve(30);  // 最后一个先完成
//   p1.Resolve(10);  // 第一个后完成
//   p2.Resolve(20);  // 中间最后完成
//
//   EXPECT_TRUE(finished);
// }
//
// // 任一 Promise reject，PromiseAll<int> 整体失败
// TEST_F(PromiseTest, All_OneReject) {
//   Promise<int> p1;
//   Promise<int> p2;
//   Promise<int> p3;
//
//   bool then_called = false;
//   bool error_called = false;
//
//   PromiseAll<int>({p1, p2, p3})
//       .Then([&](const std::vector<int>&) { then_called = true; })
//       .OnError(Tag<std::string>{}, [&](const std::string& err) {
//         error_called = true;
//         EXPECT_EQ(err, "fail");
//       });
//
//   p1.Resolve(1);
//   p2.Reject(std::string("fail"));  // p2 失败
//   p3.Resolve(3);
//
//   EXPECT_FALSE(then_called);
//   EXPECT_TRUE(error_called);
// }
//
// // 第一个就 reject
// TEST_F(PromiseTest, All_FirstReject) {
//   Promise<int> p1;
//   Promise<int> p2;
//
//   bool error_called = false;
//
//   PromiseAll<int>({p1, p2}).OnError(Tag<std::string>{}, [&](const std::string& err) {
//     error_called = true;
//     EXPECT_EQ(err, "first_fail");
//   });
//
//   p1.Reject(std::string("first_fail"));
//   p2.Resolve(2);
//
//   EXPECT_TRUE(error_called);
// }
//
// // 单个 Promise
// TEST_F(PromiseTest, All_SinglePromise) {
//   Promise<int> p1;
//
//   bool finished = false;
//
//   PromiseAll<int>({p1}).Then([&](std::vector<int> results) {
//     finished = true;
//     ASSERT_EQ(results.size(), 1u);
//     EXPECT_EQ(results[0], 42);
//   });
//
//   p1.Resolve(42);
//
//   EXPECT_TRUE(finished);
// }
//
// // 空向量
// TEST_F(PromiseTest, All_EmptyVector) {
//   bool finished = false;
//
//   PromiseAll<int>({}).Then([&](const std::vector<int>& results) {
//     finished = true;
//     EXPECT_TRUE(results.empty());
//   });
//
//   EXPECT_TRUE(finished);
// }
//
// // 已完成的 Promise 也能 PromiseAll<int>
// TEST_F(PromiseTest, All_AlreadyResolved) {
//   Promise<int> p1;
//   Promise<int> p2;
//
//   p1.Resolve(1);
//   p2.Resolve(2);
//
//   bool finished = false;
//
//   PromiseAll<int>({p1, p2}).Then([&](std::vector<int> results) {
//     finished = true;
//     ASSERT_EQ(results.size(), 2u);
//     EXPECT_EQ(results[0], 1);
//     EXPECT_EQ(results[1], 2);
//   });
//
//   EXPECT_TRUE(finished);
// }

// Promise<void> 的 PromiseAll<int>
// TEST_F(PromiseTest, All_VoidPromises) {
//   Promise<void> p1;
//   Promise<void> p2;
//   Promise<void> p3;
//
//   bool finished = false;
//
//   PromiseAll<void>({p1, p2, p3}).Then([&]() { finished = true; });
//
//   p1.Resolve();
//   p3.Resolve();
//   p2.Resolve();
//
//   EXPECT_TRUE(finished);
// }

}  // namespace spiderweb
