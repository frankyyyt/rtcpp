#pragma once

namespace rt
{

// Faster than std::find.
// Assumes the last element is set to the searched value.
// std::distance(begin, last) is assumed to be at least 1.
// HAS A BUG.

template <typename ForwardIter, typename T>
ForwardIter
find_intrusive(ForwardIter begin, ForwardIter end, const T& v)
{
  if (begin == end) // Empty range.
    return end;

  while (*begin != v)
    ++begin;

  if (begin == std::prev(end))
    return end;

  return begin;
}

}

