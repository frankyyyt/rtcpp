#pragma once

#include <iterator>

#include "algorithm.hpp"

namespace rt
{

template <class Iter>
void rotate(Iter begin, Iter end)
{
  auto prev = std::prev(end);
  while (end != begin) {
    *end = *prev;
    --end;
    --prev;
  }
}

template <class Iter, class Comp>
void insertion_sort(Iter begin, Iter end, Comp comp)
{
  if (begin == end)
    return;

  auto iter = begin;
  for (++iter; iter != end; ++iter) {
    auto l = rt::lower_bound(begin, iter, *iter);
    auto K = *iter;
    rotate(l, iter);
    *l = K;
  }
}

template <class Iter>
void insertion_sort(Iter begin, Iter end)
{
  using value_type = typename std::iterator_traits<Iter>::value_type;
  insertion_sort(begin, end, std::less<value_type>());
}

}

