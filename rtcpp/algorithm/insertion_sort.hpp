#pragma once

#include <iterator>

namespace rt
{

template <class Iter>
void insertion_sort(Iter begin, Iter end)
{
  const auto N = std::distance(begin, end);
  for (int j = 1; j < N; ++j) {
    int i = j - 1;
    const auto K = begin[j];
    while (!(K >= begin[i])) {
      begin[i + 1] = begin[i];
      if (--i == -1)
        break;
    }
    begin[i + 1] = K;
  }
}

}

