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
    Struct(spiderweb::Object* parent = nullptr) : Object(parent) {}

    void myslot(const std::string& a, int b, float c) { ++count; }

    int count = 0;
  };

  Struct reciver;
  for (auto _ : state) {
    spiderweb::Object::Connect(&obj, &MyObject::voidEvent, &reciver,
                               &Struct::myslot);
  }
}
BENCHMARK(BM_EventSignalConnectClassMethod);

static void BM_EventSignalCall(benchmark::State& state) {
  spiderweb::EventLoop loop;
  MyObject             obj;

  uint64_t          called = 0;
  static const auto f = [&](const std::string& a, int b, float c) { called++; };
  spiderweb::Object::Connect(&obj, &MyObject::voidEvent, &loop, f);

  for (auto _ : state) {
    obj.voidEvent("123", 2, float(3.33));
  }
}
BENCHMARK(BM_EventSignalCall);
