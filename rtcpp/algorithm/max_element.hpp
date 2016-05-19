#pragma once

#include <functional>

namespace rt
{

// Assumes the range is not empty.
template<class ForwardIt, class Compare>
constexpr ForwardIt
min_or_max_unchecked(ForwardIt first, ForwardIt last, Compare comp)
{
  auto max = first;
  while (++first != last)
    if (comp(*first, *max))
      max = first;

  return max;
}

template<class ForwardIt>
constexpr ForwardIt max_element(ForwardIt first, ForwardIt last)
{
  if (first == last)
    return last;

  return min_or_max_unchecked(first, last,
          std::greater<decltype(*first)>());
}

template<class ForwardIt>
constexpr ForwardIt min_element(ForwardIt first, ForwardIt last)
{
  if (first == last)
    return last;

  return min_or_max_unchecked(first, last,
          std::less<decltype(*first)>());
}

}

