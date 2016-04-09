#include <vector>
#include <iostream>
#include <iterator>
#include <algorithm>
#include <functional>

#include <rtcpp/container/set.hpp>
#include <rtcpp/utility/to_number.hpp>
#include <rtcpp/utility/make_rand_data.hpp>
#include <rtcpp/utility/timer.hpp>
#include <rtcpp/memory/node_allocator.hpp>

int main(int argc, char* argv[])
{
  if (argc != 2) {
    std::cout <<
    "\nUsage: $ ./bench_find N\n"
    "N: The start size.\n"
    << std::endl;

    return 0;
  }

  // Number of elements to be inserted in the set.
  const std::size_t n = rt::to_number<std::size_t>(argv[1]);

  auto min = std::numeric_limits<int>::min();
  auto max = std::numeric_limits<int>::max();

  std::vector<int> data = rt::make_rand_data<int>(n, min, max);

  using alloc_type = rt::node_allocator<int, rt::set<int>::node_type>;
  using set_type = rt::set<int, std::less<int>, alloc_type>;
  using node_type = typename set_type::node_type;

  // Buffer to hold the nodes.
  std::vector<node_type> buffer(n + 1);

  rt::node_alloc_header header(buffer);
  alloc_type alloc(&header);

  const auto a = std::begin(data);
  const auto b = std::end(data);
  set_type set(a, b, alloc);

  //for (auto a: set)
  //  std::cout << a << " ";
  //std::cout << std::endl;;

  //for (auto a: buffer)
  //  std::cout << a << " ";
  //std::cout << std::endl;;

  const int k = 100;
  int v;
  std::cout << "Traversing rt::vector<int>: ";
  {
    rt::timer t;
    for (int i = 0; i < k; ++i)
      v = std::accumulate(std::begin(data), std::end(data), 0);
  }
  std::cout << v << std::endl;

  std::cout << "Traversing rt::set<int>: ";
  {
    rt::timer t;
    for (int i = 0; i < k; ++i)
      v = std::accumulate(std::begin(set), std::end(set), 0);
  }
  std::cout << v << std::endl;

  auto func = [](int init, const node_type& node)
  { return rt::test_in_use(node) ? init + node.key : init; };

  std::cout << "Traversing std::vector<node_type>: ";
  {
    rt::timer t;
    for (int i = 0; i < k; ++i)
      v = std::accumulate( std::begin(buffer), std::end(buffer)
                         , 0, func);
  }
  std::cout << v << std::endl;

  return 0;
}

