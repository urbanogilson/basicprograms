#include "tictactoe/example.h"

#include <benchmark/benchmark.h>

namespace {

void bm_add(benchmark::State& state) {
    for (auto _ : state) {
        // Without DoNotOptimize the optimizer folds the call away and we time
        // an empty loop.
        benchmark::DoNotOptimize(tictactoe::add(2, 3));
    }
}

BENCHMARK(bm_add);

} // namespace
