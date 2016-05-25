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
#include <silly/Slice.h>

#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

#include "BSON.h"

std::string json_files[6] = {
    "../../data/canada.json",  "../../data/mock.json",
    "../../data/type.json",    "../../data/allOf.json",
    "../../data/default.json", "../../data/anyOf.json",
};

uint64_t bytesCount[3];
int64_t totalTime[3];

template <class F, int testCnt> void JSON_Benchmark(benchmark::State& state) {
  typedef std::istreambuf_iterator<char> iterator_t;
  std::ifstream ifs(json_files[state.range_x()]);
  std::string json(iterator_t(ifs), (iterator_t()));
  auto t1 = std::chrono::high_resolution_clock::now();
  F func;

  while (state.KeepRunning()) {
    func(json.data());
  }
  auto t2 = std::chrono::high_resolution_clock::now();
  bytesCount[testCnt] += state.iterations() * json.length() / 1000;
  totalTime[testCnt] +=
      t2.time_since_epoch().count() - t1.time_since_epoch().count();
}

struct StrDup {
  void operator()(const silly::Slice& s) {
    strdup(s.RawData());
  }
};

struct RapidJSON {
  void operator()(const silly::Slice& s) {
    using namespace rapidjson;
    Document d;
    d.Parse(s.RawData());
    StringBuffer buffer;
    Writer<StringBuffer> writer(buffer);
    d.Accept(writer);
  }
};

struct BSONCpp11 {
  void operator()(const silly::Slice& s) {
    bson::FromJSON(s);
  }
};

BENCHMARK_TEMPLATE2(JSON_Benchmark, StrDup, 0)
    ->Arg(0)
    ->Arg(1)
    ->Arg(2)
    ->Arg(3)
    ->Arg(4)
    ->Arg(5);

BENCHMARK_TEMPLATE2(JSON_Benchmark, RapidJSON, 1)
    ->Arg(0)
    ->Arg(1)
    ->Arg(2)
    ->Arg(3)
    ->Arg(4)
    ->Arg(5);

BENCHMARK_TEMPLATE2(JSON_Benchmark, BSONCpp11, 2)
    ->Arg(0)
    ->Arg(1)
    ->Arg(2)
    ->Arg(3)
    ->Arg(4)
    ->Arg(5);

int main(int argc, const char** argv) {
  ::benchmark::Initialize(&argc, argv);
  ::benchmark::RunSpecifiedBenchmarks();
  fprintf(stderr, "The strdup rate is at %lf kb/s\n",
          (double)(bytesCount[0]) / (totalTime[0] / 1000000000));

  fprintf(stderr, "The rapidjson rate is at %lf kb/s\n",
          (double)(bytesCount[1]) / (totalTime[1] / 1000000000));

  fprintf(stderr, "The bson-cpp rate is at %lf kb/s\n",
          (double)(bytesCount[2]) / (totalTime[2] / 1000000000));
}