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
  constexpr auto SL = sizeof (L);
  constexpr auto S = std::max(ST, SL);

  constexpr auto AT = alignof (T);
  constexpr auto AL = alignof (L);
  constexpr auto A = std::max(AT, AL);

  void* pa = reinterpret_cast<void*>(header->buffer);
  std::size_t bytes = header->buffer_size * SL;
  auto p = std::align(A, S, pa, bytes);
  header->buffer = reinterpret_cast<L*>(p);
  header->buffer_size = bytes / SL;

  if (header->buffer_size < 2)
    throw std::runtime_error("node_stack: There is not enough space.");

  if (header->n_alloc != 0) { // Links only once.
    if (header->block_size < S)
      throw std::runtime_error("node_stack: Avail stack already linked for node with incompatible size.");
  } else { // Links only once.
    link_stack<T, L>(header->buffer, header->buffer_size);
    header->block_size = S;
  }
  ++header->n_alloc;
}

template <class T, class L>
L node_stack<T, L>::pop() noexcept
{
  auto p = header->buffer; // Alias

  const L i = p[0]; // L of the next free node.

  if (i)
    p[0] = p[i];

  return i;
}

template <class T, class L>
void node_stack<T, L>::push(L idx) noexcept
{
  auto p = header->buffer; // Alias.
  p[idx] = p[0];
  p[0] = idx;
}

}

