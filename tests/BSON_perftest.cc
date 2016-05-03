/**
 * Copyright (C) 2016, Wu Tao All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <fstream>
#include <benchmark/benchmark.h>
#include <glog/logging.h>

#include "BSON.h"

using namespace bson;

std::string json_files[6] = {
    "../../data/canada.json",  "../../data/mock.json",
    "../../data/type.json",    "../../data/allOf.json",
    "../../data/default.json", "../../data/anyOf.json",
};

uint64_t bytesCount = 0, totalTime = 0;

static void BM_MyBSON(benchmark::State& state) {
  typedef std::istreambuf_iterator<char> iterator_t;

  std::ifstream ifs(json_files[state.range_x()]);
  std::string json(iterator_t(ifs), (iterator_t()));

  auto t1 = std::chrono::high_resolution_clock::now();

  while (state.KeepRunning()) {
    Object obj = FromJSON(json);
    assert(obj.begin() != obj.end());
  }

  auto t2 = std::chrono::high_resolution_clock::now();

  bytesCount += state.iterations() * json.length();
  totalTime += t2.time_since_epoch().count() - t1.time_since_epoch().count();
}

uint64_t bytesCount2 = 0, totalTime2 = 0;

static void BM_Strlen(benchmark::State& state) {
  typedef std::istreambuf_iterator<char> iterator_t;

  std::ifstream ifs(json_files[state.range_x()]);
  std::string json(iterator_t(ifs), (iterator_t()));

  auto t1 = std::chrono::high_resolution_clock::now();

  while (state.KeepRunning()) {
    strlen(json.data());
  }

  auto t2 = std::chrono::high_resolution_clock::now();

  bytesCount2 += state.iterations() * json.length();
  totalTime2 += t2.time_since_epoch().count() - t1.time_since_epoch().count();
}

BENCHMARK(BM_MyBSON)->Arg(0)->Arg(1)->Arg(2)->Arg(3)->Arg(4)->Arg(5);
BENCHMARK(BM_Strlen)->Arg(0)->Arg(1)->Arg(2)->Arg(3)->Arg(4)->Arg(5);

int main(int argc, const char** argv) {
  ::benchmark::Initialize(&argc, argv);
  ::benchmark::RunSpecifiedBenchmarks();
  fprintf(stderr, "The parsing rate is at %lf kb/s\n",
          (double)(bytesCount / 1000) / (totalTime / 1000000000));

  fprintf(stderr, "The strlen rate is at %lf kb/s\n",
          (double)(bytesCount2 / 1000) / (totalTime2 / 1000000000));
}