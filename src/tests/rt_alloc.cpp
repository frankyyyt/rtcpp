#include <iostream>
#include <iterator>
#include <functional>
#include <array>
#include <algorithm>

#include <rtcpp/memory/node_allocator.hpp>
#include <rtcpp/container/set.hpp>

using T = unsigned int;
using L = unsigned int;

bool test_list()
{
  std::initializer_list<T> data = {2, 3, 4, 5, 19, 33};

  rt::set<T, std::less<T>, rt::node_allocator<T, L>> t1(data);
  rt::set<T, std::less<T>, rt::node_allocator<T, L>> t2(data);
  rt::set<T, std::less<T>, rt::node_allocator<T, L>> t3(data);

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

