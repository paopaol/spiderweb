#include "benchmark/benchmark.h"
#include "spiderweb_eventloop.h"

static void BM_EventSignalConnectLambda(benchmark::State& state) {
  spiderweb::EventSignal<void(const std::string& a, int b, float c)> voidEvent;
  spiderweb::EventLoop                                               loop;

  static const auto f = [](const std::string& a, int b, float c) {};

  for (auto _ : state) {
    spiderweb::Base::Connect(voidEvent, &loop, f);
  }
}
BENCHMARK(BM_EventSignalConnectLambda);

static void BM_EventSignalConnectClassMethod(benchmark::State& state) {
  spiderweb::EventSignal<void(const std::string& a, int b, float c)> voidEvent;
  spiderweb::EventLoop                                               loop;

  struct Struct : public spiderweb::Base {
    Struct(spiderweb::Base* parent = nullptr) : Base(parent) {}

    void myslot(const std::string& a, int b, float c) { ++count; }

    int count = 0;
  };

  Struct reciver;
  for (auto _ : state) {
    spiderweb::Base::Connect(voidEvent, &reciver, &Struct::myslot);
  }
}
BENCHMARK(BM_EventSignalConnectClassMethod);
