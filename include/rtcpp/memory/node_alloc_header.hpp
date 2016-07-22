#pragma once

#include <array>
#include <vector>

namespace rt {

template <class L>
struct node_alloc_header {
  char* buffer;
  std::size_t buffer_size; // In bytes
  std::size_t n_alloc; // Number of allocators using this header.
  std::size_t block_size; // Size of blocks returned by allocate_node

  template <class U>
  node_alloc_header(U* data, std::size_t size)
  : buffer(reinterpret_cast<char*>(data))
  , buffer_size(size * sizeof (U))
  , n_alloc(0)
  , block_size(0)
  {
    if (buffer_size < sizeof (char*))
      throw std::runtime_error("node_alloc_header: Incompatible buffer size.");
  }

  template <class U, std::size_t I>
  explicit node_alloc_header(std::array<U, I>& arr)
  : node_alloc_header( reinterpret_cast<char*>(&arr.front())
                     , arr.size() * sizeof (U)) {}

  template <class U, typename Alloc>
  explicit node_alloc_header(std::vector<U, Alloc>& arr)
  : node_alloc_header( reinterpret_cast<char*>(&arr.front())
                     , arr.size() * sizeof (U)) {}

  node_alloc_header() : buffer(0) , buffer_size(0) , n_alloc(0) {}
};

}

