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

template <std::size_t S, class Index = std::size_t>
void link_stack(char* p, std::size_t n)
{
  // TODO: check alignment of pointers.

  // Number of blocks of size S available.
  const std::size_t m = n / S;

  // Index type size.
  constexpr std::size_t s = sizeof (Index);

  // How many index types fits into size S;
  constexpr std::size_t r = S / s;

  // Interprets the pointer p as if it were pointing to an array
  // of index types.
  auto idx = reinterpret_cast<Index*>(p);

  idx[0] = (m - 1) * r;
  for (std::size_t i = 1; i < m; ++i)
    idx[i * r] = (i - 1) * r;
}

}

