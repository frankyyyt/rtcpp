#include <array>
#include <vector>
#include <limits>
#include <iterator>
#include <iostream>
#include <algorithm>

#include <rtcpp/algorithm/reverse.hpp>
#include <rtcpp/algorithm/algorithm.hpp>
#include <rtcpp/utility/make_rand_data.hpp>
#include <rtcpp/algorithm/insertion_sort.hpp>
#include <rtcpp/algorithm/find_intrusive.hpp>

#include "rt_test.hpp"

RT_TEST(test_binary_search)
{
  std::array<int, 10> data = {1, 20, 32, 44, 51, 69, 70, 87, 91, 101};

  rt::insertion_sort(std::begin(data), std::end(data));

  for (std::size_t i = 0; i < data.size(); ++i)
    RT_CHECK(rt::binary_search(std::begin(data), std::end(data), data[i]))

  RT_CHECK(!rt::binary_search(std::begin(data), std::end(data), 10))
}

RT_TEST(test_find_intrusive)
{
  const int s = 10;
  std::vector<int> data = rt::make_rand_data<int>(s + 1, 0 , s);

  for (auto i: data)
    std::cout << i << " ";
  std::cout << std::endl;

  for (std::size_t i = 0; i < data.size() - 1; ++i) {
    data.back() = data[i];
    auto iter = rt::find_intrusive(std::begin(data), std::end(data),
                                   data[i]);
    std::cout << "K = " << data[i] << "\n";
    if (iter == std::end(data))
      throw std::runtime_error("");
  }
  std::cout << "First part ok." << std::endl;

  // Searches for a number that is not in the array.
  data.back() = s + 1;
  auto iter = rt::find_intrusive(std::begin(data), std::end(data),
                                 s + 1);
  RT_CHECK(iter == std::end(data))
  std::cout << "Second part ok." << std::endl;
}

RT_TEST(test_reverse)
{
  using namespace rt;

  std::array<int, 10> arr1 = {{1, 2, 3, 4, 5, 6, 7, 8, 9, 10}};
  std::array<int, 10> tmp1 = {{10, 9, 8, 7, 6, 5, 4, 3, 2, 1}};
  std::array<int, 11> arr2 = {{1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11}};
  std::array<int, 11> tmp2 = {{11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1}};

  rt::reverse(std::begin(arr1), std::end(arr1));
  rt::reverse(std::begin(arr2), std::end(arr2));

  RT_CHECK(std::equal(std::begin(arr1), std::end(arr1), std::begin(tmp1)))
  RT_CHECK(std::equal(std::begin(arr2), std::end(arr2), std::begin(tmp2)))
}

int main()
{
  test_reverse();
  test_binary_search();
  test_find_intrusive();
}

