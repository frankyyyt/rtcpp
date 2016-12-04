#include <iterator>
#include <iostream>
#include <algorithm>
#include <functional>
#include <unordered_set>

#include <config.h>

#ifdef GNU_FOUND
#include <ext/mt_allocator.h>
#include <ext/pool_allocator.h>
#include <ext/bitmap_allocator.h>
#endif

#include <rtcpp/utility/to_number.hpp>
#include <rtcpp/memory/node_allocator.hpp>
#include <rtcpp/utility/make_rand_data.hpp>
#include <rtcpp/container/set.hpp>

#include "heap_frag.hpp"
#include "print_set_bench.hpp"

using T = unsigned short;
using L = unsigned short;

template <class Allocator>
using set_type = rt::set<T, std::less<T>, Allocator>;

using node_type = typename rt::set<T>::node_type;

using type1 = set_type<std::allocator<T>>;
using type2 = set_type<rt::node_allocator<T, node_type, L>>;
#ifdef GNU_FOUND
using type3 = set_type<__gnu_cxx::__pool_alloc<T>>;
using type4 = set_type<__gnu_cxx::bitmap_allocator<T>>;
using type5 = set_type<__gnu_cxx::__mt_alloc<T>>;
#endif

void print_info()
{
  std::cout << "Size 1: " << sizeof (type1::node_type) << std::endl;
  std::cout << "Size 2: " << sizeof (type2::node_type) << std::endl;
#ifdef GNU_FOUND
  std::cout << "Size 3: " << sizeof (type3::node_type) << std::endl;
  std::cout << "Size 4: " << sizeof (type4::node_type) << std::endl;
  std::cout << "Size 5: " << sizeof (type5::node_type) << std::endl;
#endif
}

int main(int argc, char* argv[])
{
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
    "(0) (1) (2) (3) (4) (5): \n"
    "Where: \n"
    "(0)  Number of elements.\n"
    "(1)  std::set<std::allocator>\n"
    "(2)  std::set<rt::node_allocator>\n"
    "(3)  std::set<__gnu_cxx::__pool_alloc>\n"
    "(4)  std::set<__gnu_cxx::bitmap_alloc>\n"
    "(5)  std::set<__mt_alloc>\n\n";
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
  bench<type1>(N, S, K, data);
  std::cout << "(2)" << std::endl;
  bench<type2>(N, S, K, data);
#ifdef GNU_FOUND
  std::cout << "(3)" << std::endl;
  bench<type3>(N, S, K, data);
  std::cout << "(4)" << std::endl;
  bench<type4>(N, S, K, data);
  std::cout << "(5)" << std::endl;
  bench<type5>(N, S, K, data);
#endif
  std::cout << std::endl;
  std::for_each( std::begin(pointers), std::end(pointers)
               , [](char* p){ delete p;});
  return 0;
}

