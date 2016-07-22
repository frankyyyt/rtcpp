#pragma once

#include <iterator>
#include <utility>
#include <cstring>
#include <exception>
#include <array>
#include <memory>
#include <algorithm>

#include "link_stack.hpp"
#include "align.hpp"
#include "node_alloc_header.hpp"

namespace rt {

template <class T, class L>
class node_stack {
  public:
  static const std::size_t ptr_size = sizeof (char*);
  node_alloc_header<L>* header;
  // used only when default constructed. Will be soon removed.
  node_alloc_header<L> header_dummy;
  public:
  node_stack() : header(&header_dummy) {}
  node_stack(node_alloc_header<L>* header);
  L pop() noexcept;
  void push(L p) noexcept;
  bool operator==(const node_stack& rhs) const noexcept
  {return header == rhs.header;}
  void swap(node_stack& other) noexcept
  {std::swap(header, other.header);}
};

template <class T, class L>
node_stack<T, L>::node_stack(node_alloc_header<L>* header)
: header(header)
{
  constexpr auto ST = sizeof (T);
  constexpr auto SI = sizeof (L);
  constexpr auto S = std::max(ST, SI);

  constexpr auto AT = alignof (T);
  constexpr auto AI = alignof (L);
  constexpr auto A = std::max(AT, AI);

  void* pa = reinterpret_cast<void*>(header->buffer);
  auto p = std::align(A, S, pa, header->buffer_size);
  header->buffer = reinterpret_cast<char*>(p);

  const std::size_t n = header->buffer_size;

  if (n < 2 * S)
    throw std::runtime_error("node_stack: There is not enough space.");

  if (header->n_alloc != 0) { // Links only once.
    if (header->block_size < S)
      throw std::runtime_error("node_stack: Avail stack already linked for node with incompatible size.");
  } else { // Links only once.
    auto p = reinterpret_cast<L*>(header->buffer);
    link_stack<T, L>(p, n / SI);
    header->block_size = S;
  }
  ++header->n_alloc;
}

template <class T, class L>
L node_stack<T, L>::pop() noexcept
{
  auto p = reinterpret_cast<L*>(header->buffer);

  const L i = p[0]; // L of the next free node.

  if (i)
    p[0] = p[i];

  return i;
}

template <class T, class L>
void node_stack<T, L>::push(L idx) noexcept
{
  auto p = reinterpret_cast<L*>(header->buffer);
  p[idx] = p[0];
  p[0] = idx;
}

}

