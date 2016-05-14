#pragma once

namespace rt
{

template<class ForwardIt>
constexpr ForwardIt max_element(ForwardIt first, ForwardIt last)
{
  if (first == last)
    return last;

  auto max = first;
  while (++first != last)
    if (*first > *max)
      max = first;

  return max;
}

}

