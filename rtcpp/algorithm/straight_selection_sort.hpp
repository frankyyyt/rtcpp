#include <rtcpp/algorithm/max_element.hpp>

namespace rt
{

template <class Iter>
void straight_selection_sort(Iter begin, Iter end)
{
  const auto N = std::distance(begin, end);

  if (N < 2)
    return;

  for (int j = N; j > 1; --j) {
    auto max = begin[j - 1];
    int i = j; 
    while (i--) // Finds the max element.
      if (begin[i - 1] >= max)
        max = begin[i - 1];
    std::swap(max, begin[j - 1]);
  }
}

}

