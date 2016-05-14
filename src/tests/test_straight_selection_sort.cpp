#include <vector>
#include <limits>
#include <algorithm>

#include <rtcpp/utility/make_rand_data.hpp>
#include <rtcpp/algorithm/straight_selection_sort.hpp>

int main()
{
  std::vector<int> data =
    rt::make_rand_data<int>(10000, 1, std::numeric_limits<int>::max());

  rt::straight_selection_sort(std::begin(data), std::end(data));

  return std::is_sorted(std::begin(data), std::end(data)) ? 0 : 1;
}

