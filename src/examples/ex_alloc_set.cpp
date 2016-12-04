#include <iostream>
#include <functional>

#include <rtcpp/container/set.hpp>
#include <rtcpp/utility/print.hpp>
#include <rtcpp/memory/node_allocator.hpp>

int main()
{
  using T = unsigned char;
  using L = unsigned char;
  using Node = rt::set<T>::node_type;

  rt::set<T, std::less<T>, rt::node_allocator<T, Node, L, 4>> t1;
  t1 = {'k', 'd', 'c', 'l', 'e', 'g', 'b', 'h', 'i',
  'j', 'a', 'h','m'};

  for (auto a: t1)
    std::cout << a << " ";
  std::cout << std::endl;;

  return 0;
}

