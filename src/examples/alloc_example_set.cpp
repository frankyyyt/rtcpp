#include <iostream>
#include <functional>

#include <rtcpp/container/set.hpp>
#include <rtcpp/utility/print.hpp>
#include <rtcpp/memory/node_allocator.hpp>

int main()
{
  using T = unsigned char;
  using L = unsigned char;
  using alloc_type = rt::node_allocator<T, rt::set<T>::node_type, L>;
  using set_type = rt::set<T, std::less<T>, alloc_type>;
  using node_type = typename set_type::node_type;

  rt::node_alloc_header<L> header(50);

  alloc_type alloc(&header);

  set_type t1(alloc);
  t1 = {'a', 'f', 'a', 'c', 'l', 'l', 'e', 'd', 'f', 'b'};

  for (auto a: t1)
    std::cout << a << " ";
  std::cout << std::endl;;

  return 0;
}

