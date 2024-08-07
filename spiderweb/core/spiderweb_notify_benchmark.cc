#include "benchmark/benchmark.h"
#include "spiderweb/core/spiderweb_eventloop.h"

class MyObject : public spiderweb::Object {
 public:
  spiderweb::Notify<const std::string&, int, float> voidEvent;
};

static void BM_NotifyConnectLambda(benchmark::State& state) {
  spiderweb::EventLoop loop;

  static const auto f = [](const std::string& a, int b, float c) {};

  for (auto _ : state) {
    MyObject obj;
    spiderweb::Object::Connect(&obj, &MyObject::voidEvent, &loop, f);
  }
}
BENCHMARK(BM_NotifyConnectLambda);

static void BM_NotifyConnectClassMethod(benchmark::State& state) {
  spiderweb::EventLoop loop;

  struct Struct : public spiderweb::Object {
    explicit Struct(spiderweb::Object* parent = nullptr) : Object(parent) {
    }

    void myslot(const std::string& /*a*/, int /*b*/, float /*c*/) {
      ++count;
    }

    int count = 0;
  };

  Struct reciver;
  for (auto _ : state) {
    MyObject obj;
    spiderweb::Object::Connect(&obj, &MyObject::voidEvent, &reciver, &Struct::myslot);
  }
}
BENCHMARK(BM_NotifyConnectClassMethod);

static void BM_NotifyCall(benchmark::State& state) {
  spiderweb::EventLoop loop;

  uint64_t          called = 0;
  static const auto f = [&](const std::string& /*a*/, int /*b*/, float /*c*/) { called++; };

  for (auto _ : state) {
    MyObject obj;
    spiderweb::Object::Connect(&obj, &MyObject::voidEvent, &loop, f);
    obj.voidEvent("123", 2, static_cast<float>(3.33));
  }
}
BENCHMARK(BM_NotifyCall);

static void BM_CallDirectly(benchmark::State& state) {
  uint64_t                                                  called = 0;
  std::function<void(const std::string& a, int b, float c)> s;

  static const auto f = [&](const std::string& /*a*/, int /*b*/, float /*c*/) { called++; };

  s = f;

  for (auto _ : state) {
    s("123", 2, static_cast<float>(3.33));
  }
}
BENCHMARK(BM_CallDirectly);
