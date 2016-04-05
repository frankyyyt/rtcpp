#include <iostream>
#include <functional>

#include <rtcpp/container/set.hpp>
#include <rtcpp/utility/print.hpp>
#include <rtcpp/memory/node_allocator.hpp>

int main()
{
  using alloc_type = rt::node_allocator<int, rt::set<int>::node_type>;
  using set_type = rt::set<int, std::less<int>, alloc_type>;
  using node_type = typename set_type::node_type;

  std::array<node_type, 12> buffer = {{}};
  rt::node_alloc_header header(buffer);

  alloc_type alloc(&header);

  set_type t1(alloc);
  t1 = {5, 3, 7, 20, 1, 44, 10, 22, 8};
  print(t1);
  return 0;
}

