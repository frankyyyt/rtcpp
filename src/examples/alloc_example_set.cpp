#include <iostream>
#include <functional>

#include <rtcpp/container/set.hpp>
#include <rtcpp/utility/print.hpp>
#include <rtcpp/memory/node_allocator.hpp>

int main()
{
  using T = unsigned char;
  using L = unsigned char;
  using alloc_type = rt::node_allocator<T, rt::set<T>::node_type, L>;
  using set_type = rt::set<T, std::less<T>, alloc_type>;
  using node_type = typename set_type::node_type;

  std::array<node_type, 13> buffer = {{}};
  std::cout << "Buffer size: " << (sizeof buffer) << std::endl;

  rt::node_alloc_header<L> header(buffer);

  for (auto a: buffer)
    std::cout << a << " ";
  std::cout << std::endl;;

  alloc_type alloc(&header);

  set_type t1(alloc);
  t1 = {'a', 'f', 'a', 'c', 'l', 'l', 'e', 'd', 'f', 'b'};

  for (auto a: buffer)
    std::cout << a << " ";
  std::cout << std::endl;;

  for (auto a: t1)
    std::cout << a << " ";
  std::cout << std::endl;;

  std::copy_if( std::begin(buffer), std::end(buffer)
              , std::ostream_iterator<node_type>(std::cout, " ")
              , [](auto o){return o.is_in_use();});
  std::cout << std::endl;;

  const auto n = std::count_if(std::begin(buffer), std::end(buffer),
    [](auto o){return o.is_in_use();});
  std::cout << n << std::endl;;

  return 0;
}

