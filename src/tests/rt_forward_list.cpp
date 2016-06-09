#include <iostream>
#include <vector>
#include <random>
#include <algorithm>
#include <functional>

#include <rtcpp/utility/make_rand_data.hpp>
#include <rtcpp/utility/print.hpp>
#include <rtcpp/container/forward_list.hpp>

int main()
{
  using namespace rt;

  const int size = 1000;

  const int a = 1;
  const int b = size;

  const auto data = make_rand_data<int>(size, a, b, 1);
  forward_list<int> l;

  std::copy(std::begin(data), std::end(data), std::front_inserter(l));

  const bool b1 = std::equal(std::rbegin(data), std::rend(data),
                             std::begin(l));
  if (!b1)
    return 1;

  l.reverse();
  const bool b2 = std::equal(std::begin(data), std::end(data),
                             std::begin(l));
  if (!b2)
    return 1;

  l.reverse();
  const bool b3 = std::equal(std::rbegin(data), std::rend(data),
                             std::begin(l));
  if (!b3)
    return 1;

  l.insertion_sort();

  if (!std::is_sorted(std::begin(l), std::end(l))) {
    //print(l);
    return 1;
  }

  l.remove_if(1);
  l.remove_if(2);
  l.remove_if(4);
  return 0;
}

