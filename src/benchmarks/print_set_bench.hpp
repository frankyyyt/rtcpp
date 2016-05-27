#pragma once

#include <algorithm>

#include <rtcpp/utility/timer.hpp>

namespace rt {

template <typename C, typename RandomAccessIter>
void print_set_bench(C c, RandomAccessIter begin, std::size_t n)
{
  // c: Set instance, can be an std::set, std::unordered_set, etc.
  // begin: Iterator pointing to random data.
  // n: Size of random data.
  //
  // This function is designed to benchmark an sets.  Insertions
  // and deletions are made toguether to maximize cache misses.

  const std::size_t s = n / 2;
  int sum = 0; // Dummy variable.
  int repeat = 10;
  // Inserts the first half of the random data in the set. do not
  // participate in the benchmark.
  c.insert(begin, begin + s);
  {
    rt::timer t;
    for (std::size_t i = 0; i <= s; ++i) {
      c.erase(begin[i]); // Removes already inserted random data.
      c.insert(begin[n - i - 1]); // Inserts the second half of rando data.
      // Traverses the container every *repeat* times
      if (i % repeat == 0)
        sum = std::accumulate(std::begin(c), std::end(c), 0);
    }
    // Same purpose as the loop above.
    for (std::size_t i = 0; i <= s; ++i) {
      c.erase(begin[n - i - 1]); // Removes the second half.
      c.insert(begin[i]); // Inserts the first half again.
      if (i % repeat == 0)
        sum = std::accumulate(std::begin(c), std::end(c), 0);
    }
  }
}

}

