#include "benchmark/benchmark.h"
#include "spiderweb/type/spiderweb_binary_view.h"

static void BM_ViewAsUint16(benchmark::State &state) {
  uint8_t buf[2] = {0x01, 0x02};
  for (auto _ : state) {
    spiderweb::type::BinaryView::ViewAs<uint16_t>(buf, 2, spiderweb::arch::ArchType::kBig);
  }
}

BENCHMARK(BM_ViewAsUint16);
