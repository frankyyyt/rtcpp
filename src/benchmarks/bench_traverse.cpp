#include <set>
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

#include "heap_frag.hpp"

int main(int argc, char* argv[])
{
  if (argc != 3) {
    std::cout <<
    "\nUsage: $ ./bench_set N B F\n"
    "N: The start size.\n"
    "B: Chars between.\n"
    << std::endl;
    return 0;
  }

  const std::size_t N = rt::to_number<std::size_t>(argv[1]);
  const std::size_t B = rt::to_number<std::size_t>(argv[2]);

  const bool frag = true;

  auto min = std::numeric_limits<int>::min();
  auto max = std::numeric_limits<int>::max();

  std::vector<int> data = rt::make_rand_data<int>(N, min, max);

  using alloc_type = rt::node_allocator<int, rt::set<int>::node_type, std::size_t>;
  using set_type = rt::set<int, std::less<int>, alloc_type>;
  using node_type = typename set_type::node_type;

  // Buffer to hold the nodes.
  std::vector<node_type> buffer(N + 1);

  const auto a = std::begin(data);
  const auto b = std::end(data);

  //for (auto a: set)
  //  std::cout << a << " ";
  //std::cout << std::endl;;

  const int k = 100;
  int v;
  {
    std::cout << "Traversing rt::set<int>: ";
    std::vector<char*> pointers;
    if (frag) // Fragments the heap.
      pointers = rt::heap_frag<rt::set<int>>(B, data);

    rt::set<int> rt_set(a, b);
    {
      rt::timer t;
      for (int i = 0; i < k; ++i)
        v = std::accumulate(std::begin(rt_set), std::end(rt_set), 0);
    }
    std::for_each( std::begin(pointers), std::end(pointers)
                 , [](char* p){ delete p;});
    std::cout << v << std::endl;
  }
  std::cout << "Traversing rt::vector<int>: ";
  {
    rt::timer t;
    for (int i = 0; i < k; ++i)
      v = std::accumulate(std::begin(data), std::end(data), 0);
  }
  std::cout << v << std::endl;

  rt::node_alloc_header<std::size_t> header(buffer);
  alloc_type alloc(&header);

  set_type set(a, b, alloc);
  std::cout << "Traversing rt::set<int, node_allocator>: ";
  {
    rt::timer t;
    for (int i = 0; i < k; ++i)
      v = std::accumulate(std::begin(set), std::end(set), 0);
  }
  std::cout << v << std::endl;

  auto func = [](int init, const node_type& node)
  { return node.is_in_use() ? init + node.key : init; };

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

