#include <vector>
#include <limits>
#include <algorithm>
#include <iostream>
#include <rtcpp/utility/make_rand_data.hpp>
#include <rtcpp/algorithm/find_intrusive.hpp>

int main()
{
  const int s = 1000;
  std::vector<int> data = rt::make_rand_data<int>(s + 1, 0 , s);

  for (auto i: data)
    std::cout << i << " ";
  std::cout << std::endl;

  for (int i = 0; i < s; ++i) {
    data.back() = data[i];
    auto iter = rt::find_intrusive(std::begin(data), std::end(data),
                                   data[i]);
    if (iter == std::end(data))
      return 1;
  }

  // Searches for a number that is not in the array.
  data.back() = s + 1;
  auto iter = rt::find_intrusive(std::begin(data), std::end(data),
                                 s + 1);
  if (iter != std::end(data))
    return 1;

  return 0;
}

