#include <set>
#include <vector>
#include <tuple>
#include <iterator>
#include <iostream>
#include <algorithm>
#include <functional>

#include <config.h>

#include <rtcpp/utility/to_number.hpp>
#include <rtcpp/memory/node_allocator.hpp>
#include <rtcpp/utility/make_rand_data.hpp>
#include <rtcpp/container/set.hpp>

#include "heap_frag.hpp"
#include "print_set_bench.hpp"

using T = unsigned int;
using L = std::size_t;

template <class Allocator>
using set_type = rt::set<T, std::less<T>, Allocator>;

using type1 = set_type<std::allocator<T>>;

using alloc_type = rt::node_allocator<T, typename type1::node_type, L>;

using type2 = set_type<alloc_type>;

const int node_size1 = sizeof (type1::node_type);
const int node_size2 = sizeof (type2::node_type);

void print_info()
{
  std::cout << "Size 1: " << node_size1 << std::endl;
  std::cout << "Size 2: " << node_size2 << std::endl;
}

int main(int argc, char* argv[])
{
  print_info();

  if ((argc != 5) && (argc != 6)) {
    std::cout <<
    "\nUsage: $ ./bench_set N S K B F\n"
    "N: The start size.\n"
    "S: The step size.\n"
    "K: How many steps.\n"
    "B: Chars between.\n"
    "F: Optional (any value). If provided will not fragment the\n"
    "   heap before benchmarks.\n\n"
    "The program outputs has the following layout: \n"
    "(0) (1) (2): \n"
    "Where: \n"
    "(0)  Number of elements.\n"
    "(1)  std::set<std::alloc>\n"
    "(2)  std::set<rt::alloc>\n";
    return 0;
  }

  using namespace rt;

  std::cout << argv[0] << " "
            << argv[1] << " "
            << argv[2] << " "
            << argv[3] << " "
            << argv[4] << "\n";
            
  const int N = to_number<int>(argv[1]);
  const int S = to_number<int>(argv[2]);
  const int K = to_number<int>(argv[3]);
  const int B = to_number<int>(argv[4]);
  const bool frag = !(argc == 6);

  const std::vector<T> data =
    rt::make_rand_data<T>( N + (K - 1) * S, 1
                           , std::numeric_limits<T>::max());

  std::vector<char*> pointers;
  if (frag) // Fragments the heap.
    pointers = heap_frag<rt::set<T>>(B, data);

  std::cout << "(1)" << std::endl;
  for (int i = 0; i < K; ++i) {
    const int n = N + i * S;
    std::cout << n << " ";
    print_set_bench(type1(), std::begin(data), n); // (1)
    std::cout << std::endl;
  }
  std::cout << "(2)" << std::endl;
  for (int i = 0; i < K; ++i) {
    const int n = N + i * S;
    std::cout << n << " ";
    std::vector<char> buffer((n + 2) * node_size2, 0);
    rt::node_alloc_header<L> header(buffer);
    typename type2::allocator_type alloc(&header);
    type2 s(alloc);
    print_set_bench(s, std::begin(data), n);
    std::cout << std::endl;
  }
  std::cout << std::endl;
  std::for_each( std::begin(pointers), std::end(pointers)
               , [](char* p){ delete p;});
  return 0;
}

