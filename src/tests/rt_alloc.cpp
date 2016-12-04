#include <iostream>
#include <iterator>
#include <functional>
#include <array>
#include <algorithm>

#include <rtcpp/memory/node_allocator.hpp>
#include <rtcpp/container/set.hpp>

using T = unsigned char;
using L = unsigned char;

using node_type = typename rt::set<T>::node_type;

bool test()
{
  std::initializer_list<T> data =
  {2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17};

  rt::set< T
         , std::less<T>
         , rt::node_allocator<T, node_type, L, 2>
         > t1(data);

  if (!std::equal(std::begin(data), std::end(data), std::begin(t1)))
    return false;

  t1.clear();

  t1 = data;

  if (!std::equal(std::begin(data), std::end(data), std::begin(t1)))
    return false;

  return true;
}

int main()
{
  try {
    if (!test())
      return 1;
  } catch (const std::exception& e) {
    std::cout << e.what() << std::endl;
    return 1;
  }
  return 0;
}

