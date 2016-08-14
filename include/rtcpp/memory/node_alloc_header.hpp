#pragma once

#include <array>
#include <vector>
#include <limits>

#include "align.hpp"

namespace rt {

// Expects the pointer p to have a suitable alignment for both T and
// L.

template <class T, class L, std::size_t N>
void link_stack(L* p, std::size_t offset)
{
  static constexpr auto SL = sizeof (L);
  static constexpr auto ST = sizeof (T);
  static constexpr auto R = (SL < ST) ? ST / SL : 1;

  for (std::size_t i = 0; i < N; ++i)
    p[i * R] = offset + i;
}

template <class T, class L, std::size_t N>
class node_alloc_header {
  private:

  static_assert((!std::is_signed<L>::value),
  "node_alloc_header: Incompatible type.");

  static_assert((N - 1 <= std::numeric_limits<L>::max()),
  "node_alloc_header: Incompatible size.");

  static_assert((is_power_of_two<N>::value),
  "node_alloc_header: N must be a power of 2.");

  static constexpr auto SL = sizeof (L);
  static constexpr auto ST = sizeof (T);
  static constexpr auto R = (SL < ST) ? ST / SL : 1;
  static constexpr auto S = N * R; // Size of allocated array.

  public:
  L* buffer;

  static_assert((S >= 2), "node_alloc_header: Invalid N.");

  L* get_base_ptr(L)
  {
    return buffer;
  }

  node_alloc_header() : buffer(new L[S])
  {
    buffer[0] = N - 1;
    for (std::size_t i = 1; i < N; ++i)
      buffer[i * R] = i - 1;
  }

  ~node_alloc_header() { delete [] buffer; }

  L pop()
  {
    auto p = buffer; // Alias

    const L i = p[0];

    if (!i)
      throw std::bad_alloc();

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

