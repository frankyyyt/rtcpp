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
  std::array<T, n> arr = {{}};

  node_alloc_header header(arr);

  using Index = std::size_t;
  node_stack<T, Index> stack(&header);

  print(arr);
  for (T i = n; i != 0; --i) {
    Index idx = stack.pop();
    if (idx)
      arr[idx] = i + 20;
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

  print(arr);
  for (T i = n; i != 0; --i) {
    Index idx = stack.pop();
    if (idx)
      arr[idx] = i + 20;
  }
  std::array<T, n> check = {0,24,23,22,29,27,28,26,30,25};

  print(arr);
  if (!std::equal(std::begin(check), std::end(check), std::begin(arr)))
    return 1;

  try {
    Index i = stack.pop();
    if (i)
      return 1; 
  } catch (const std::bad_alloc& e) {
  }

  print(arr);

  return 0;
}

