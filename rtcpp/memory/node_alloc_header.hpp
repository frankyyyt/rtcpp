#pragma once

#include <array>
#include <vector>

namespace rt {

struct node_alloc_header {
  char* data;
  std::size_t size;

  template <class U>
  node_alloc_header(U* data, std::size_t size)
  : data(reinterpret_cast<char*>(data))
  , size(size * sizeof (U)) {}

  template <class U, std::size_t I>
  explicit node_alloc_header(std::array<U, I>& arr)
  : node_alloc_header( reinterpret_cast<char*>(&arr.front())
                     , arr.size() * sizeof (U)) {}

  template <typename Alloc>
  explicit node_alloc_header(std::vector<char, Alloc>& arr)
  : node_alloc_header(&arr.front(), arr.size()) {}
};

}

