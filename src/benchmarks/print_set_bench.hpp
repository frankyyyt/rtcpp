#pragma once

#include <algorithm>

#include <rtcpp/utility/timer.hpp>

namespace rt {

template <class C>
void print_set_bench(const std::vector<typename C::value_type>& data)
{
  // c: Set instance, can be an std::set, std::unordered_set, etc.
  // data: Iterator pointing to random data.
  // n: Size of random data.
  //
  // This function is designed to benchmark an sets.  Insertions
  // and deletions are made together to maximize cache misses.
  const auto n = data.size();
  const std::size_t s = n / 2;
  int repeat = 10;
  // Inserts the first half of the random data in the set. do not
  // participate in the benchmark.
  C c;
  c.insert(data.begin(), data.begin() + s);
  {
    rt::timer t;
    for (std::size_t i = 0; i <= s; ++i) {
      c.erase(data[i]); // Removes already inserted random data.
      c.insert(data[n - i - 1]); // Inserts the second half of random data.
      // Traverses the container every *repeat* times
      if (i % repeat == 0)
        (void)std::accumulate(std::begin(c), std::end(c), 0);
    }
    // Same purpose as the loop above.
    for (std::size_t i = 0; i <= s; ++i) {
      c.erase(data[n - i - 1]); // Removes the second half.
      c.insert(data[i]); // Inserts the first half again.
      if (i % repeat == 0)
        (void)std::accumulate(std::begin(c), std::end(c), 0);
    }
  }
}

template <class T>
void bench(int N, int S, int K,
  const std::vector<typename T::value_type>& data)
{
  for (int i = 0; i < K; ++i) {
    const int n = N + i * S;
    std::cout << n << " ";
    print_set_bench<T>(data);
    std::cout << std::endl;
  }
}

}

