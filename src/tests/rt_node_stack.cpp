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
  constexpr T n = 8;

  using Index = std::size_t;
  node_alloc_header<std::size_t, Index, n> header;

  for (auto i = 1; i != n; ++i) {
    auto idx = header.pop();
    std::cout << idx << std::endl;
  }

  try {
    header.pop();
    std::cout << "ERROR" << std::endl;
    return 1;
  } catch (...) {
    std::cout << "OK" << std::endl;
  }

  std::cout << "___________" << std::endl;
  header.push(3);
  header.push(2);
  header.push(1);
  header.push(7);
  header.push(5);
  header.push(6);
  header.push(4);
  std::cout << "___________" << std::endl;

  for (auto i = 1; i != n; ++i) {
    auto idx = header.pop();
    std::cout << idx << std::endl;
  }


  try {
    header.pop();
    std::cout << "ERROR" << std::endl;
    return 1;
  } catch (...) {
    std::cout << "OK" << std::endl;
  }

  return 0;
}

