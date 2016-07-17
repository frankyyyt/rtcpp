#pragma once

#include <iterator>

#include "algorithm.hpp"

namespace rt
{

template <class Iter>
void rotate(Iter begin, Iter end)
{
  while (begin != end) {
    std::iter_swap(begin, end);
    ++begin;
  }
}

template <class Iter, class Comp>
void insertion_sort(Iter begin, Iter end, Comp comp)
{
  for (auto iter = begin; iter != end; ++iter)
    rotate(rt::lower_bound(begin, iter, *iter), iter);
}

template <class Iter>
void insertion_sort(Iter begin, Iter end)
{
  insertion_sort(begin, end, std::less<decltype(*begin)>());
}

}

