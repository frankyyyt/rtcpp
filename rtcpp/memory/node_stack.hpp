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

template <class T>
class node_stack {
  private:
  static_assert( !((sizeof (std::uintptr_t)) > (sizeof (char*)))
     , "node_stack: Unable to use this class in this platform.");
  static const std::size_t ptr_size = sizeof (char*);
  node_alloc_header* header;
  // used only when default constructed. Will be soon removed.
  node_alloc_header header_dummy;
  public:
  using pointer = T*;
  node_stack() : header(&header_dummy) {}
  node_stack(node_alloc_header* header);
  pointer pop() noexcept;
  void push(pointer p) noexcept;
  bool operator==(const node_stack& rhs) const noexcept
  {return header == rhs.header;}
  void swap(node_stack& other) noexcept
  {std::swap(header, other.header);}
};

template <class T>
node_stack<T>::node_stack(node_alloc_header* header)
: header(header)
{
  const std::size_t S = sizeof (T);
  std::size_t n = header->buffer_size;
  align_if_needed<ptr_size>(header->buffer, n);
  const std::size_t min_size = 2 * S;
  if (n < min_size)
    throw std::runtime_error("node_stack: There is not enough space.");

  if (header->n_alloc != 0) { // Links only once.
    if (header->block_size < S)
      throw std::runtime_error("node_stack: Avail stack already linked for node with incompatible size.");
  } else { // Links only once.
    header->buffer = link_stack(header->buffer, n, S);
    header->block_size = S;
  }
  ++header->n_alloc;
}

template <class T>
typename node_stack<T>::pointer node_stack<T>::pop() noexcept
{
  pointer q = reinterpret_cast<pointer>(header->buffer);
  if (q)
    std::memcpy(&header->buffer, q, ptr_size);

  return q;
}

template <class T>
void node_stack<T>::push(typename node_stack<T>::pointer p) noexcept
{
  std::memcpy(p, &header->buffer, ptr_size);
  std::memcpy(&header->buffer, &p, ptr_size);
}

}

