#pragma once

#include <cstring>

namespace rt {

// Expects the pointer p to have a suitable aliggment for both T and L.

template <class T, class L>
void link_stack(L* p, std::size_t n)
{
  constexpr auto N = sizeof (L);
  constexpr auto S = sizeof (T);
  constexpr auto R = S / N;
  constexpr auto K = sizeof (std::size_t);

  if (N < K) {
    const std::size_t max = R * std::numeric_limits<L>::max();
    if (n > max)
      n = max;
  }

  const auto m = n / R; // Number of blocks of size S available.

  p[0] = m - 1;
  for (std::size_t i = 1; i < m; ++i)
    p[i * R] = i - 1;
}

}

