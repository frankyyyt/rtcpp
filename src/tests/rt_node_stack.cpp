#include <iostream>
#include <algorithm>
#include <exception>

#include <rtcpp/memory/node_alloc_header.hpp>
#include <rtcpp/memory/node_stack.hpp>
#include <rtcpp/utility/print.hpp>

int main()
{
  using namespace rt;

  using T = std::size_t;
  const std::size_t N = sizeof (T);
  static_assert((sizeof N) == sizeof (char*));
  constexpr T n = 10;

  node_alloc_header<std::size_t> header(n);

  using Index = std::size_t;
  node_stack<T, Index> stack(&header);

  for (T i = n; i != 0; --i) {
    Index idx = stack.pop();
  }
  stack.push(3);
  stack.push(2);
  stack.push(1);
  stack.push(9);
  stack.push(7);
  stack.push(5);
  stack.push(6);
  stack.push(4);
  stack.push(8);

  for (T i = n; i != 0; --i) {
    Index idx = stack.pop();
  }
  std::array<T, n> check = {0,24,23,22,29,27,28,26,30,25};

  try {
    Index i = stack.pop();
    if (i)
      return 1; 
  } catch (const std::bad_alloc& e) {
  }

  return 0;
}

