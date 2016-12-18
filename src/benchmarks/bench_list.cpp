#include <list>
#include <vector>
#include <iostream>
#include <iterator>
#include <algorithm>
#include <functional>
#include <type_traits>

#include <rtcpp/utility/timer.hpp>
#include <rtcpp/utility/to_number.hpp>
#include <rtcpp/memory/node_allocator.hpp>
#include <rtcpp/container/forward_list.hpp>
#include <rtcpp/utility/make_rand_data.hpp>

namespace rt {

template <class C>
void
print_list_bench(const std::vector<typename C::value_type>& data)
{
  C l(std::begin(data), std::end(data));
  rt::timer t;
  l.sort();
}

}

int main(int argc, char* argv[])
{
  if (argc != 4) {
    std::cout <<
    "\nUsage: $ ./bench_set N S K\n"
    "N: The start size.\n"
    "S: The step size.\n"
    "K: How many steps.\n"
    << std::endl;
    std::cout <<
    "The program output has the following layout: \n"
    "(0) (1): \n"
    "Where: \n"
    "(0)  Number of elements.\n"
    "(1)  std::forward_list\n"
    << std::endl;

    return 0;
  }

  const std::size_t N = rt::to_number<std::size_t>(argv[1]);
  const std::size_t S = rt::to_number<std::size_t>(argv[2]);
  const std::size_t K = rt::to_number<std::size_t>(argv[3]);

  using T = short;

  const std::vector<T> data =
    rt::make_rand_data<T>( N + (K - 1) * S
                         , 1
                         , std::numeric_limits<T>::max());

  using node_type  = rt::forward_list<T>::node_type;
  using alloc_type =
    rt::node_allocator< T
                      , node_type
                      , std::make_unsigned<T>::type
                      , 1024>;

  using cont_type1 = rt::forward_list<T>;
  using cont_type2 = rt::forward_list<T, alloc_type>;

  for (std::size_t i = 0; i < K; ++i) {
    const unsigned n = N + i * S;
    std::cout << n << " ";
    rt::print_list_bench<cont_type1>(data);
    std::cout << std::endl;
  }

  std::cout << "__________" << std::endl;

  for (std::size_t i = 0; i < K; ++i) {
    const unsigned n = N + i * S;
    std::cout << n << " ";
    rt::print_list_bench<cont_type2>(data);
    std::cout << std::endl;
  }
  return 0;
}

