#include <array>
#include <vector>
#include <limits>
#include <algorithm>

#include <rtcpp/utility/make_rand_data.hpp>
#include <rtcpp/algorithm/insertion_sort.hpp>
#include <rtcpp/algorithm/algorithm.hpp>

int main()
{
  std::array<int, 10> data = {1, 20, 32, 44, 51, 69, 70, 87, 91, 101};

  rt::insertion_sort(std::begin(data), std::end(data));

  for (std::size_t i = 0; i < data.size(); ++i) {
    if (!rt::binary_search(std::begin(data), std::end(data), data[i]))
      return 1;
  }

    if (rt::binary_search(std::begin(data), std::end(data), 10))
      return 1;

  return 0;
}

