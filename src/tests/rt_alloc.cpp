#include <iostream>
#include <iterator>
#include <functional>
#include <array>
#include <set>
#include <algorithm>
#include <list>

#include <rtcpp/memory/node_allocator_lazy.hpp>
#include <rtcpp/container/set.hpp>

bool test_list()
{
  std::array<int, 6> data = {{10, 3, 2, 8, 19, 33}};

  rt::node_allocator_lazy<int> alloc;

  std::list<int, rt::node_allocator_lazy<int>> t1(std::begin(data), std::end(data), alloc);
  std::list<int, rt::node_allocator_lazy<int>> t2(std::begin(data), std::end(data), alloc);
  std::list<int, rt::node_allocator_lazy<int>> t3(std::begin(data), std::end(data), alloc);

  if (!std::equal(std::begin(data), std::end(data), std::begin(t1)))
    return false;

  if (!std::equal(std::begin(data), std::end(data), std::begin(t2)))
    return false;

  if (!std::equal(std::begin(data), std::end(data), std::begin(t3)))
    return false;

  return true;
}

int main()
{
  try {
    if (!test_list())
      return 1;
  } catch (const std::exception& e) {
    std::cout << e.what() << std::endl;
    return 1;
  }
  return 0;
}

