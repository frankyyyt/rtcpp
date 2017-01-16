#include <iostream>
#include <functional>

#include <rtcpp/container/set.hpp>
#include <rtcpp/utility/print.hpp>
#include <rtcpp/memory/node_allocator.hpp>

int main()
{
  using L = unsigned char;
  using T = L;
  using Node = rt::set<T>::node_type;
  using A = rt::node_allocator<T, Node, L, 4>;
  using C = rt::set<T, std::less<T>, A>;
  using node_type = typename C::node_type;

  static_assert(sizeof (node_type) == 4);

  C t1 {'k', 'd', 'u', 'c', 'l', 'e', 'g', 'b', 'h', 'i'};

  for (auto a: t1)
    std::cout << a << " ";

  std::cout << std::endl;;

  auto alloc = t1.get_allocator().get_node_storage();

  for (auto a: *alloc)
    std::cout << a.key << " ";

  std::cout << std::endl;

  return 0;
}

