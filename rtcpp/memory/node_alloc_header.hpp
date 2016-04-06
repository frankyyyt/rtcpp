#pragma once

#include <array>
#include <vector>

namespace rt {

struct node_alloc_header {
  char* data;
  std::size_t size;
  std::size_t n_alloc;
  std::size_t block_size;

  template <class U>
  node_alloc_header(U* data, std::size_t size)
  : data(reinterpret_cast<char*>(data))
  , size(size * sizeof (U))
  , n_alloc(0)
  , block_size(0)
  {}

  template <class U, std::size_t I>
  explicit node_alloc_header(std::array<U, I>& arr)
  : node_alloc_header( reinterpret_cast<char*>(&arr.front())
                     , arr.size() * sizeof (U)) {}

  template <typename Alloc>
  explicit node_alloc_header(std::vector<char, Alloc>& arr)
  : node_alloc_header(&arr.front(), arr.size()) {}

  node_alloc_header() : data(0) , size(0) , n_alloc(0) {}
};

}

