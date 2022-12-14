#include "benchmark/benchmark.h"
#include "spiderweb_eventloop.h"

class MyObject : public spiderweb::Object {
 public:
  spiderweb::EventSignal<void(const std::string& a, int b, float c)> voidEvent;
};

static void BM_EventSignalConnectLambda(benchmark::State& state) {
  spiderweb::EventLoop loop;
  MyObject             obj;

  static const auto f = [](const std::string& a, int b, float c) {};

  for (auto _ : state) {
    spiderweb::Object::Connect(&obj, &MyObject::voidEvent, &loop, f);
  }
}
BENCHMARK(BM_EventSignalConnectLambda);

static void BM_EventSignalConnectClassMethod(benchmark::State& state) {
  spiderweb::EventLoop loop;
  MyObject             obj;

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
    spiderweb::Object::Connect(&obj, &MyObject::voidEvent, &reciver, &Struct::myslot);
  }
}
BENCHMARK(BM_EventSignalConnectClassMethod);

static void BM_EventSignalCall(benchmark::State& state) {
  spiderweb::EventLoop loop;
  MyObject             obj;

  uint64_t          called = 0;
  static const auto f = [&](const std::string& /*a*/, int /*b*/, float /*c*/) { called++; };
  spiderweb::Object::Connect(&obj, &MyObject::voidEvent, &loop, f);

  for (auto _ : state) {
    obj.voidEvent("123", 2, static_cast<float>(3.33));
  }
}
BENCHMARK(BM_EventSignalCall);

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
