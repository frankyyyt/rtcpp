#pragma once

#include <algorithm>

#include <rtcpp/utility/timer.hpp>

namespace rt {

template <typename C, typename Iter>
void print_set_bench(C c, Iter begin, std::size_t n)
{
  // This function is designed to benchmark an std::set.  Insertions
  // and deletions are made toguether to maximize cache misses.

  const std::size_t s = n / 2;
  int sum = 0;
  int repeat = 10;
  c.insert(begin, begin + s); // Inserts the first half of the data.
  {
    rt::timer t;
    for (std::size_t i = 0; i <= s; ++i) {
      c.erase(begin[i]); // Removes the first half.
      c.insert(begin[n - i - 1]); // Inserts the second half.
      if (i % repeat == 0)
        sum = std::accumulate(std::begin(c), std::end(c), 0);
    }
    for (std::size_t i = 0; i <= s; ++i) {
      c.erase(begin[n - i - 1]); // Removes the second half.
      c.insert(begin[i]); // Inserts the first half again.
      if (i % repeat == 0)
        sum = std::accumulate(std::begin(c), std::end(c), 0);
    }
  }
}

}

