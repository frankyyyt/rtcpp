#include <array>
#include <unordered_set>

#include <rtcpp/memory/node_allocator_lazy.hpp>
#include <rtcpp/utility/print.hpp>

int main()
{
  std::array<char, 2000> buffer = {{}};
  rt::node_alloc_header<std::size_t> header(buffer);
  rt::node_allocator_lazy<int> alloc(&header);

  std::unordered_set<int, std::hash<int>, std::equal_to<int>,
                     rt::node_allocator_lazy<int>> t1(alloc);
  t1 = {5, 3, 7, 20, 1, 44, 22, 8};

  print(t1);
}

