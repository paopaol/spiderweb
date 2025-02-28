#include "benchmark/benchmark.h"
#include "spiderweb/type/spiderweb_variant.h"

using VVV = absl::variant<spiderweb::detail::Monostate, int16_t, uint16_t, int32_t, uint32_t,
                          int64_t, uint64_t, std::string, float, double, bool, absl::any>;

float ToFloat(const VVV &v) {
  return absl::get<float>(v);
}

static void BM_ToFloat(benchmark::State &state) {
  spiderweb::Variant v(1.f);

  int count = 0;
  for (auto _ : state) {
    count = v.ToFloat() + count;
  }
}

BENCHMARK(BM_ToFloat);

void set(VVV &s) {
  s = 1.f;
}

static void BM_ToFloat2(benchmark::State &state) {
  absl::variant<spiderweb::detail::Monostate, int16_t, uint16_t, int32_t, uint32_t, int64_t,
                uint64_t, std::string, float, double, bool, absl::any>
      v = spiderweb::detail::Monostate{};
  set(v);

  int count = 0;
  for (auto _ : state) {
    count = ToFloat(v) + count;
  }
}

BENCHMARK(BM_ToFloat2);

static void BM_ToFloatUnion(benchmark::State &state) {
  union Value {
    float f;
    int   i;
  };

  Value v;
  v.f = 123.f;

  for (auto _ : state) {
    void(v.f);
  }
}

BENCHMARK(BM_ToFloatUnion);

static void BM_GetString(benchmark::State &state) {
  spiderweb::Variant v("1234567890");

  for (auto _ : state) {
    const auto &g = v.Get<std::string>();
    g.begin();
  }
}

BENCHMARK(BM_GetString);

static void BM_GetStringRaw(benchmark::State &state) {
  std::string v("1234567890");

  for (auto _ : state) {
    const auto &g = v;
    g.begin();
  }
}

BENCHMARK(BM_GetStringRaw);
