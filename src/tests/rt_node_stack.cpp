#include <iostream>
#include <algorithm>
#include <exception>

#include <rtcpp/memory/node_alloc_header.hpp>
#include <rtcpp/utility/print.hpp>

int main()
{
  using namespace rt;

  using T = std::size_t;
  const std::size_t N = sizeof (T);
  constexpr T n = 4;

  using Index = std::size_t;
  node_alloc_header<std::size_t, Index, n> header;
  using pointer = 
    typename node_alloc_header<std::size_t, Index, n>::pointer;

  for (auto i = 1; i != n; ++i) {
    auto idx = header.pop();
    std::cout << idx.get_idx() << std::endl;
  }

  if (header.get_n_blocs() != 1)
    return 1;

  header.pop();

  if (header.get_n_blocs() != 2)
    return 1;

  std::cout << "___________" << std::endl;
  header.push(pointer(header.get_base_ptr(3), 3));
  header.push(pointer(header.get_base_ptr(2), 2));
  header.push(pointer(header.get_base_ptr(1), 1));
  std::cout << "___________" << std::endl;

  for (auto i = 1; i != n; ++i) {
    auto idx = header.pop();
    std::cout << idx.get_idx() << std::endl;
  }


  if (header.get_n_blocs() != 2)
    return 1;

  return 0;
}

