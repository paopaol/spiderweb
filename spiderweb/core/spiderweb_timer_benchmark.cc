#include "benchmark/benchmark.h"
#include "spiderweb/core/spiderweb_eventloop.h"
#include "spiderweb/core/spiderweb_timer.h"

static void BM_TimerAdd(benchmark::State &state) {
  spiderweb::EventLoop loop;

  static int value = 0;
  int        count = 0;
  const auto f = [&]() {
    ++value;
    if (count == value) {
      loop.Quit();
    }
  };

  for (auto _ : state) {
    spiderweb::Timer *timer = new spiderweb::Timer();

    timer->SetSingalShot(true);
    timer->SetInterval(1);
    spiderweb::Object::Connect(timer, &spiderweb::Timer::timeout, &loop, f);
    ++count;
    timer->Start();
  }
  printf("value %d count %d\n", value, count);

  loop.Exec();
  printf("value %d count %d\n", value, count);
}

BENCHMARK(BM_TimerAdd);
