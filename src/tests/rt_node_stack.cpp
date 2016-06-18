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
  const T n = 10;
  std::array<T, n> arr = {{}};

  node_alloc_header header(arr);

  node_stack<T> stack(&header);

  //print(arr);
  node_stack<T>::pointer p;
  for (T i = n; i != 0; --i) {
    p = stack.pop();
    if (p)
      *p = i;
  }

  std::array<T, n> check = {1,2,3,4,5,6,7,8,9,10};

  print(arr);
  if (!std::equal(std::begin(check), std::end(check), std::begin(arr)))
    return 1;

  try {
    p = stack.pop();
    if (p)
      return 1; 
  } catch (const std::bad_alloc& e) {
  }

  print(arr);

  return 0;
}

