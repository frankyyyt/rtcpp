#include <iostream>
#include <iterator>
#include <functional>
#include <array>
#include <set>
#include <algorithm>
#include <list>

#include <rtcpp/memory/node_allocator_lazy.hpp>
#include <rtcpp/container/set.hpp>

bool test_buffer_size()
{
  std::array<char, 1> buffer = {{}};
  try {
    rt::node_alloc_header<std::size_t> header(buffer);
  } catch (const std::exception& e) {
    std::cout << e.what() << std::endl;
    return true;
  }
  return false;
}

bool test_link_diff_node_size()
{
  std::array<char, 900> buffer = {{}};
  rt::node_alloc_header<std::size_t> header(buffer);
  rt::node_allocator_lazy<int> alloc(&header);
  try {
    rt::set< int
           , std::less<int>, rt::node_allocator_lazy<int>> t2(alloc);
    std::set< int, std::less<int>
            , rt::node_allocator_lazy<int>> t1(std::less<int>(), alloc);
  } catch (const std::exception& e) {
    std::cout << e.what() << std::endl;
    return true;
  }
  return false;
}

bool test_list()
{
  std::array<int, 6> data = {{10, 3, 2, 8, 19, 33}};

  std::array<char, 5000> buffer = {{}};

  rt::node_alloc_header<std::size_t> header(buffer);
  rt::node_allocator_lazy<int> alloc(&header);

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

bool test_deallocate()
{
  const std::size_t data_size = sizeof (std::size_t);
  std::array<char, 3 * data_size> buffer = {{}};

  rt::node_alloc_header<std::size_t> header(buffer);
  rt::node_allocator_lazy<int> alloc1(&header);
  // links the node_allocator_lazy.
  rt::node_allocator_lazy<std::size_t> alloc2(alloc1);

  std::size_t* p1 = alloc2.allocate_node();
  *p1 = 10; // To avoid a warning.
  std::size_t* p2 = alloc2.allocate_node();
  try {
    std::size_t* p3 = alloc2.allocate_node(); // Should throw.
    *p3 = 100; // To avoid a warning.
    return false;
  } catch (...) {
    alloc2.deallocate_node(p2);
  }
  return true;
}

int main()
{
  try {
    if (!test_buffer_size())
      return 1;

    if (!test_link_diff_node_size())
      return 1;

    if (!test_list())
      return 1;

    if (!test_deallocate())
      return 1;
  } catch (const std::exception& e) {
    std::cout << e.what() << std::endl;
    return 1;
  }
  return 0;
}

