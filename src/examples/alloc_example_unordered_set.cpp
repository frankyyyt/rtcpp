#include <array>
#include <unordered_set>

#include <rtcpp/memory/node_allocator_lazy.hpp>
#include <rtcpp/utility/print.hpp>

int main()
{
  rt::node_allocator_lazy<int> alloc(50);

  std::unordered_set<int, std::hash<int>, std::equal_to<int>,
                     rt::node_allocator_lazy<int>> t1(alloc);
  t1 = {5, 3, 7, 20, 1, 44, 22, 8};

  print(t1);
}

