#include <iostream>
#include <functional>

#include <rtcpp/container/set.hpp>
#include <rtcpp/utility/print.hpp>
#include <rtcpp/memory/node_allocator.hpp>

int main()
{
  using T = unsigned char;
  using L = unsigned char;

  rt::set<T, std::less<T>, rt::node_allocator<T, L, 4>> t1;
  t1 = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i',
  'j', 'k', 'l','m'};

  for (auto a: t1)
    std::cout << a << " ";
  std::cout << std::endl;;

  return 0;
}

