#pragma once

#include <array>
#include <vector>
#include <limits>

#include "align.hpp"

namespace rt {

// Expects the pointer p to have a suitable alignment for both T and
// L.

template <class T, class L, std::size_t Size>
void link_stack(L* p)
{
  constexpr auto N = sizeof (L);
  constexpr auto S = sizeof (T);
  constexpr auto R = (S < N) ? 1 : S / N;

  constexpr auto M = Size / R; // Number of blocks of size S available.

  p[0] = M - 1;
  for (std::size_t i = 1; i < M; ++i)
    p[i * R] = i - 1;
}

template <class T, class L, std::size_t N>
struct node_alloc_header {
  static_assert((!std::is_signed<L>::value),
  "link_stack: Incompatible type.");

  static_assert((N - 1 <= std::numeric_limits<L>::max()),
  "link_stack: Incompatible size.");

  static_assert((is_power_of_two<N>::value),
  "node_alloc_header: N must be a power of 2.");

  L* buffer;
  static constexpr auto S = std::max(sizeof (T), sizeof (L));
  static constexpr auto M = std::min(sizeof (T), sizeof (L));
  static constexpr auto R = S / M;
  static constexpr auto size = N * R;

  static_assert((size >= 2), "node_alloc_header: Invalid N.");

  node_alloc_header() : buffer(new L[size])
  {
    link_stack<T, L, size>(buffer);
  }

  ~node_alloc_header() { delete [] buffer; }

  L pop() noexcept
  {
    auto p = buffer; // Alias

    const L i = p[0]; // L of the next free node.

    if (i)
      p[0] = p[i * R];

    return i;
  }

  void push(L idx) noexcept
  {
    auto p = buffer; // Alias.
    p[idx * R] = p[0];
    p[0] = idx;
  }

  bool operator==(const node_alloc_header& rhs) const noexcept
  {return buffer == rhs.buffer;}
};

}

