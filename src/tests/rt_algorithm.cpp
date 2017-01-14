#include <array>
#include <vector>
#include <limits>
#include <iostream>
#include <stdexcept>
#include <algorithm>

#include <rtcpp/algorithm/algorithm.hpp>
#include <rtcpp/utility/make_rand_data.hpp>
#include <rtcpp/algorithm/insertion_sort.hpp>
#include <rtcpp/algorithm/find_intrusive.hpp>

void test_binary_search()
{
  std::array<int, 10> data = {1, 20, 32, 44, 51, 69, 70, 87, 91, 101};

  rt::insertion_sort(std::begin(data), std::end(data));

  for (std::size_t i = 0; i < data.size(); ++i)
    if (!rt::binary_search(std::begin(data), std::end(data), data[i]))
      throw std::runtime_error("");

  if (rt::binary_search(std::begin(data), std::end(data), 10))
    throw std::runtime_error("");
}

void test_find_intrusive()
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
  if (iter != std::end(data))
    throw std::runtime_error("");
  std::cout << "Second part ok." << std::endl;
}

int main()
{
  test_find_intrusive();
  test_binary_search();
}

