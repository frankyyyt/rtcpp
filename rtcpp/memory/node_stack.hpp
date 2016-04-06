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

class node_stack {
  // I do not know whether the standard guarantees the
  // condition below, since I rely on it I will test it.
  static_assert( !((sizeof (std::uintptr_t)) > (sizeof (char*)))
               , "node_stack: Unable to use this class in this platform.");
  private:
  static const std::size_t ptr_size = sizeof (char*);
  node_alloc_header* header;
  // used only when default constructed. Will be soon removed.
  node_alloc_header header_dummy;
  public:
  static constexpr std::size_t memory_use = 0;
  private:
  public:
  node_stack();
  node_stack(node_alloc_header* header, std::size_t S);
  char* pop() noexcept;
  void push(char* p) noexcept;
  bool operator==(const node_stack& rhs) const noexcept {return header == rhs.header;}
  void swap(node_stack& other) noexcept {std::swap(header, other.header);}
};

// Move this to cpp file.
node_stack::node_stack() : header(&header_dummy) {}

// Move this to cpp file.
node_stack::node_stack(node_alloc_header* header, std::size_t S)
: header(header)
{
  std::size_t n = header->size;
  align_if_needed<ptr_size>(header->data, n);
  const std::size_t min_size = 2 * S;
  if (n < min_size)
    throw std::runtime_error("node_stack: There is not enough space.");

  if (header->n_alloc != 0) { // Links only once.
    if (header->block_size < S)
      throw std::runtime_error("node_stack: Avail stack already linked for node with incompatible size.");
  } else { // Links only once.
    header->data = link_stack(header->data, n, S);
    header->block_size = S;
  }
  ++header->n_alloc;
}

inline
char* node_stack::pop() noexcept
{
  char* q = header->data;
  if (q)
    std::memcpy(&header->data, q, ptr_size);

  return q;
}

inline
void node_stack::push(char* p) noexcept
{
  std::memcpy(p, &header->data, ptr_size);
  std::memcpy(&header->data, &p, ptr_size);
}

}

