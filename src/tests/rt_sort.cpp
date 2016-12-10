#include <vector>
#include <limits>
#include <iostream>
#include <exception>
#include <algorithm>
#include <rtcpp/algorithm/bubble_sort.hpp>
#include <rtcpp/utility/make_rand_data.hpp>
#include <rtcpp/algorithm/comp_counting_sort.hpp>
#include <rtcpp/algorithm/insertion_sort.hpp>
#include <rtcpp/algorithm/dist_counting_sort.hpp>
#include <rtcpp/algorithm/straight_selection_sort.hpp>

#define TEST_SORT(name)                                \
void test_##name()                                     \
{                                                      \
  std::vector<int> data =                              \
    rt::make_rand_data<int>(1000, 1,                   \
       std::numeric_limits<int>::max());               \
                                                       \
  rt::name(std::begin(data), std::end(data));          \
                                                       \
  const auto b =                                       \
     std::is_sorted(std::begin(data), std::end(data)); \
  if (!b)                                              \
    throw std::runtime_error(#name);                   \
}

TEST_SORT(bubble_sort);
TEST_SORT(comp_counting_sort);
TEST_SORT(insertion_sort);
TEST_SORT(straight_selection_sort);

void test_dist_count_sort()
{
  const std::size_t N = 200000;
  const int A = -20;
  const int B = 200;
  std::vector<int> data = rt::make_rand_data<int>(N ,A ,B);

  rt::dist_counting_sort(std::begin(data), N, A, B);

  const auto b = std::is_sorted(std::begin(data), std::end(data));
  if (!b)
    throw std::runtime_error("test_dist_count_sort");
}

int main()
{
  try {
    test_bubble_sort();
    test_comp_counting_sort();
    test_insertion_sort();
    test_straight_selection_sort();
  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
  }
}

