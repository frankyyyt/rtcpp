#include <iostream>
#include <algorithm>
#include <exception>

#include <rtcpp/memory/node_storage.hpp>
#include <rtcpp/utility/print.hpp>

int main()
{
  using namespace rt;

  using T = std::size_t;
  constexpr T n = 4;

  using Index = std::size_t;
  node_storage<std::size_t, Index, n> header;

  for (auto i = 1; i != n; ++i) {
    auto idx = header.pop();
    std::cout << idx.get_idx() << std::endl;
  }

  if (header.get_n_blocs() != 1)
    return 1;

  std::cout << "OK" << std::endl;
  for (auto i = 1; i != n; ++i) {
    auto idx = header.pop();
    std::cout << idx.get_idx() << std::endl;
  }

  if (header.get_n_blocs() != 2)
    return 1;

  std::cout << "OK" << std::endl;
  for (auto i = 1; i != n; ++i) {
    auto idx = header.pop();
    std::cout << idx.get_idx() << std::endl;
  }

  if (header.get_n_blocs() != 3)
    return 1;

  std::cout << "OK" << std::endl;
  for (auto i = 1; i != n; ++i) {
    auto idx = header.pop();
    std::cout << idx.get_idx() << std::endl;
  }

  return 0;
}

