#pragma once

#include <vector>
#include <iterator>
#include <algorithm>
#include <functional>

namespace rt
{

template<class RandomAccessIter, class T>
RandomAccessIter
lower_bound(RandomAccessIter begin, RandomAccessIter end, const T& K)
{
  if (begin == end)
    return end;

  const auto N = std::distance(begin, end);
  int l = 1;
  int u = N;

  for (;;) {
    if (u < l)
      return begin + u;

    int i = (l + u) >> 1;

    if (K < begin[i - 1]) {
      u = i - 1;
      continue;
    }

    if (begin[i - 1] < K) {
      l = i + 1;
      continue;
    }

    return begin + i - 1;
  }
}

template<class RandomAccessIter, class T>
bool binary_search(RandomAccessIter begin, RandomAccessIter end, const T& K)
{
  auto iter = rt::lower_bound(begin, end, K);
  return iter != end && !(K < *iter);
}

// Assumes the range is not empty.
template<class ForwardIt, class Compare>
ForwardIt
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

template <typename Iter>
void comp_counting_sort(Iter begin, Iter end)
{
  const auto n = std::distance(begin, end);
  std::vector<std::size_t> buffer(n, 0);

  for (auto i = n - 1; i > 0; --i)
    for (auto j = i - 1; j >= 0; --j)
      if (begin[i] < begin[j])
        ++buffer[j];
      else
        ++buffer[i];

  std::vector<std::size_t> buffer2(n, 0);
  for (auto i = 0; i < n; ++i)
    buffer2[buffer[i]] = begin[i];

  std::copy(buffer2.begin(), buffer2.end(), begin);
}

template <typename Iter>
void
dist_counting_sort(Iter begin, std::size_t N
  , typename std::iterator_traits<Iter>::value_type A
  , typename std::iterator_traits<Iter>::value_type B)
{
  const std::size_t count_size = B - A + 1;
  std::vector<std::size_t> count(count_size, 0);
  // Counts the number of elements of each value.
  for (std::size_t i = 0; i < N; ++i)
    ++count[begin[i] - A];

  // calculates the final position in the array.
  --count[0]; // In c the first index is zero not 1.
  for (std::size_t i = 1; i < count_size; ++i)
    count[i] += count[i - 1];

  typedef typename std::iterator_traits<Iter>::value_type value_type;
  std::vector<value_type> out(N, 0);
  for (std::size_t i = 0; i < N; ++i) {
    const auto idx = begin[i] - A; // Index in the count array.
    const auto pos = count[idx]; // Final position in output array.
    out[pos] = begin[i]; // Assigns to the output array.
    --count[idx];
  }

  std::copy(std::begin(out), std::end(out), begin);
}

template<typename Iter>
void reverse(Iter begin, Iter end) noexcept
{
  while (begin < end) {
    std::iter_swap(begin, --end);
    ++begin;
  }
}
        
// Faster than std::find.
// Assumes the last element is set to the searched value.
// std::distance(begin, last) is assumed to be at least 1.

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

template <class Iter>
void straight_selection_sort(Iter begin, Iter end)
{
  for (; begin != end; ++begin)
    std::iter_swap(min_or_max_unchecked(begin, end,
      std::less<decltype(*begin)>()), begin);
}

}

