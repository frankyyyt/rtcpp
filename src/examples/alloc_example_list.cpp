#include <array>
#include <list>

#include <rtcpp/memory/node_allocator_lazy.hpp>
#include <rtcpp/utility/print.hpp>

int main()
{
  std::list<int, rt::node_allocator_lazy<int>> t1;
  t1 = {5, 3, 7, 20, 1, 44, 22, 8};

  print(t1);
}

