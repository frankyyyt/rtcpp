#include <iostream>
#include <functional>

#include <rtcpp/container/set.hpp>
#include <rtcpp/utility/print.hpp>
#include <rtcpp/memory/node_allocator.hpp>

int main()
{
  using alloc_type = rt::node_allocator<int, rt::set<int>::node_type>;
  using set_type = rt::set<int, std::less<int>, alloc_type>;
  using node_type = typename set_type::node_type;

  std::array<node_type, 13> buffer = {{}};
  rt::node_alloc_header<std::size_t> header(buffer);

  for (auto a: buffer)
    std::cout << a << " ";
  std::cout << std::endl;;

  alloc_type alloc(&header);

  set_type t1(alloc);
  t1 = {1, 5, 1, 7, 3, 1, 1, 4, 9, 1};

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

  auto n = std::count_if(std::begin(buffer), std::end(buffer),
    [](auto o){return o.is_in_use();});
  std::cout << n << std::endl;;

  return 0;
}

