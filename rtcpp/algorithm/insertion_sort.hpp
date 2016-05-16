#pragma once

#include <iterator>

namespace rt
{

template <class Iter, class Comp>
void insertion_sort(Iter begin, Iter end, Comp comp)
{
  const auto rend = std::prev(begin);
  for (Iter iter = ++begin; iter != end; ++iter) {
    Iter prev = std::prev(iter, 1);
    const auto K = *iter;
    while (comp(K, *prev)) {
      *std::next(prev, 1) = *prev;
      if (--prev == rend)
        break;
    }
    *std::next(prev, 1) = K;
  }
}

template <class Iter>
void insertion_sort(Iter begin, Iter end)
{
  using value_type = typename std::iterator_traits<Iter>::value_type;
  insertion_sort(begin, end, std::less<value_type>());
}

}

