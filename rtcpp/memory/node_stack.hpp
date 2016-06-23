#pragma once

#include <iterator>
#include <utility>
#include <cstring>
#include <exception>
#include <array>
#include <algorithm>

#include "link_stack.hpp"
#include "align.hpp"
#include "node_alloc_header.hpp"

namespace rt {

template <class T, class Index>
class node_stack {
  public:
  static const std::size_t ptr_size = sizeof (char*);
  node_alloc_header* header;
  // used only when default constructed. Will be soon removed.
  node_alloc_header header_dummy;
  public:
  node_stack() : header(&header_dummy) {}
  node_stack(node_alloc_header* header);
  Index pop() noexcept;
  void push(Index p) noexcept;
  bool operator==(const node_stack& rhs) const noexcept
  {return header == rhs.header;}
  void swap(node_stack& other) noexcept
  {std::swap(header, other.header);}
};

template <class T, class Index>
node_stack<T, Index>::node_stack(node_alloc_header* header)
: header(header)
{
  constexpr std::size_t S = sizeof (T);
  std::size_t n = header->buffer_size;
  align_if_needed<ptr_size>(header->buffer, n);
  const std::size_t min_size = 2 * S;
  if (n < min_size)
    throw std::runtime_error("node_stack: There is not enough space.");

  if (header->n_alloc != 0) { // Links only once.
    if (header->block_size < S)
      throw std::runtime_error("node_stack: Avail stack already linked for node with incompatible size.");
  } else { // Links only once.
    link_stack<S, Index>(header->buffer, n);
    header->block_size = S;
  }
  ++header->n_alloc;
}

template <class T, class Index>
Index node_stack<T, Index>::pop() noexcept
{
  auto p = reinterpret_cast<Index*>(header->buffer);

  const Index i = p[0]; // Index of the next free node.

  if (i)
    p[0] = p[i];

  return i;
}

template <class T, class Index>
void node_stack<T, Index>::push(Index idx) noexcept
{
  auto p = reinterpret_cast<Index*>(header->buffer);
  p[idx] = p[0];
  p[0] = idx;
}

}

