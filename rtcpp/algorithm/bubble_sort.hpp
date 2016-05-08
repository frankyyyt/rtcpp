#pragma once

#include <iterator>

namespace rt
{

template <class Iter>
void bubble_sort(Iter begin, Iter end)
{
  const int N = std::distance(begin, end);

  if (N < 2)
    return;

  int B = N;

  for (;;) {
    int t = 0;
    for (int j = 1; j < B; ++j) {
      if (begin[j - 1] > begin[j]) {
        std::swap(begin[j - 1], begin[j]);
        t = j;
      }
    }
    if (t == 0)
      break;
    B = t;
  }
}

}

