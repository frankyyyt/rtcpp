#pragma once

#include <cstring>

namespace rt {

  /*

  This function takes a pointer p to an array of n bytes.  The address
  to which p points is expected to be aligned on a N = sizeof (char*)
  boundary. The minimum value of n expected is 2 * S, where S >= N.
  The buffer is split into blocks of size S and the blocks are linked
  in a LIFO fashion (stack). The address of the next block is recorded
  in the first N bytes of the block. The block sizes S are expected to
  be at least the size of a pointer.

  returns a pointer to the top of the stack.

  */

template <class T, class I>
void link_stack(I* p, std::size_t n)
{
  // TODO: check alignment of pointers.
  constexpr auto N = sizeof (I);
  constexpr auto S = sizeof (T);
  constexpr auto r = S / N;
  const auto m = n / r; // Number of blocks of size S available.

  p[0] = (m - 1) * r;
  for (std::size_t i = 1; i < m; ++i)
    p[i * r] = (i - 1) * r;
}

}

