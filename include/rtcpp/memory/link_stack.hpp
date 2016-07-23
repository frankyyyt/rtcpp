#pragma once

#include <cstring>

namespace rt {

// Expects the pointer p to have a suitable aliggment for both T and L.

template <class T, class L>
void link_stack(L* p, std::size_t n)
{
  const std::size_t max = std::numeric_limits<L>::max();
  n = (n > max) ? max : n;

  constexpr auto N = sizeof (L);
  constexpr auto S = sizeof (T);
  constexpr auto r = S / N;
  const auto m = n / r; // Number of blocks of size S available.

  p[0] = (m - 1) * r;
  for (std::size_t i = 1; i < m; ++i)
    p[i * r] = (i - 1) * r;
}

}

