#include <array>
#include <list>

#include <rtcpp/memory/node_allocator_lazy.hpp>
#include <rtcpp/utility/print.hpp>

int main()
{
  rt::node_alloc_header<std::size_t> header(50);
  rt::node_allocator_lazy<int> alloc(&header);

  std::list<int, rt::node_allocator_lazy<int>> t1(alloc);
  t1 = {5, 3, 7, 20, 1, 44, 22, 8};

  print(t1);
}

