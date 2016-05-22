#include <list>
#include <vector>
#include <limits>
#include <iterator>
#include <algorithm>

#include <rtcpp/utility/make_rand_data.hpp>
#include <rtcpp/algorithm/insertion_sort.hpp>

int main()
{
  const int N = 1000;
  std::vector<int> data1 =
    rt::make_rand_data<int>(N, 1, std::numeric_limits<int>::max());

  std::list<int> data2;
  std::copy(std::begin(data1), std::end(data1),
            std::back_inserter(data2));

  rt::insertion_sort(std::begin(data1), std::end(data1));
  rt::insertion_sort(std::begin(data2), std::end(data2));

  const bool r1 = std::is_sorted(std::begin(data1), std::end(data1));
  const bool r2 = std::is_sorted(std::begin(data2), std::end(data2));
  return r1 && r2 ? 0 : 1;
}

