#pragma once

#include <array>
#include <vector>

namespace rt {

template <class L>
struct node_alloc_header {
  L* buffer;
  std::size_t buffer_size; // In bytes
  std::size_t n_alloc; // Number of allocators using this header.
  std::size_t block_size; // Size of blocks returned by allocate_node

  node_alloc_header(std::size_t size)
  : buffer(new L[size])
  , buffer_size(size)
  , n_alloc(0)
  , block_size(0)
  {
    if (buffer_size < 1)
      throw std::runtime_error("node_alloc_header: Incompatible buffer size.");
  }

  node_alloc_header() : buffer(0) , buffer_size(0) , n_alloc(0) {}

  ~node_alloc_header()
  {
    delete [] buffer;
  }
};

}

