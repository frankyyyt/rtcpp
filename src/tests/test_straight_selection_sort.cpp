#include <vector>
#include <limits>
#include <iostream>
#include <algorithm>

#include <rtcpp/utility/make_rand_data.hpp>
#include <rtcpp/algorithm/straight_selection_sort.hpp>

int main()
{
  std::vector<int> data =
    rt::make_rand_data<int>(1000, 1, std::numeric_limits<int>::max());

  for (auto i: data)
    std::cout << i << " ";
  std::cout << std::endl;

  rt::straight_selection_sort(std::begin(data), std::end(data));

  for (auto i: data)
    std::cout << i << " ";
  std::cout << std::endl;

  return std::is_sorted(std::begin(data), std::end(data)) ? 0 : 1;
}

