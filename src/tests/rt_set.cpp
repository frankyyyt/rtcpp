#include <iostream>
#include <set>
#include <iterator>
#include <random>
#include <functional>
#include <algorithm>
#include <iterator>
#include <limits>
#include <array>
#include <vector>

#include <rtcpp/container/set.hpp>
#include <rtcpp/memory/node_allocator.hpp>
#include <rtcpp/utility/make_rand_data.hpp>
#include <rtcpp/utility/print.hpp>

template <class A>
void test_move()
{
  std::initializer_list<int> data1 = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

  rt::set<int, std::less<int>, A> t1(data1);

  if (!std::equal(std::begin(data1), std::end(data1), std::begin(t1)))
    throw std::runtime_error("test_move");

  rt::set<int, std::less<int>, A> t2(std::move(t1));

  if (!std::equal(std::begin(data1), std::end(data1), std::begin(t2)))
    throw std::runtime_error("test_move");

  if (!t1.empty())
    throw std::runtime_error("test_move");

  print(t1);
  print(t2);
}

template <class A>
void test_reverse_traversal()
{
  constexpr int size = 10;

  constexpr std::array<int, size> o =
  {{0, 1, 2, 3, 4, 5, 6, 7, 8, 9}};

  constexpr std::array<int, size> arr =
  {{o[5], o[2], o[7], o[9], o[8], o[3], o[0], o[4], o[6], o[1]}};

  rt::set<int, std::less<int>, A> t1(std::begin(arr), std::end(arr));

  auto iter = t1.end();

  // We should be now at the rbegin.

  for (int i = size - 1; i != -1; --i )
    if (*--iter != o[i])
      throw std::runtime_error("test_reverse_traversal");
}

template <class A>
void test_swap()
{
  std::initializer_list<int> data1 = {1, 2, 3, 4, 5};
  std::initializer_list<int> data2 = {6, 7, 8, 9, 10};

  using set_type = rt::set<int, std::less<int>, A>;
  set_type t1(data1);
  const set_type t1_copy = t1;
  set_type t2(data2);
  const set_type t2_copy = t2;

  std::cout << "_______" << std::endl;
  rt::print(t1);
  rt::print(t1_copy);
  rt::print(t2);
  rt::print(t2_copy);

  std::swap(t1, t2);

  std::cout << "_______" << std::endl;
  rt::print(t1);
  rt::print(t1_copy);
  rt::print(t2);
  rt::print(t2_copy);
  std::cout << "_______" << std::endl;

  std::cout << "test_swap" << std::endl;

  if (t1 != t2_copy)
    throw std::runtime_error("test_swap");

  std::cout << "test_swap" << std::endl;

  if (t2 != t1_copy)
    throw std::runtime_error("test_swap");

  std::cout << "test_swap" << std::endl;
}

template <class A>
void test_erase(const std::vector<int>& arr)
{
  rt::set<int, std::less<int>, A> t1;

  t1.insert(std::begin(arr), std::end(arr));
  for (auto v : arr) { // Removes forward
    t1.erase(v);
    if (t1.find(v) != std::end(t1))
      throw std::runtime_error("test_erase");
  }

  if (!t1.empty())
      throw std::runtime_error("test_erase");

  t1.insert(std::begin(arr), std::end(arr));
  for (std::size_t i = 0; i < arr.size(); ++i) { // Removes backwards.
    const auto v = arr[arr.size() - i - 1];
    t1.erase(v);
    if (t1.find(v) != std::end(t1))
      throw std::runtime_error("test_move");
  }

  if (!t1.empty())
    throw std::runtime_error("test_move");
}

template <class A>
void test_count(const std::vector<int>& arr)
{
  rt::set<int, std::less<int>, A> t1;
  t1.insert(std::begin(arr), std::end(arr));
  if (!std::all_of(std::begin(arr), std::end(arr), [&](int a){ return t1.count(a) == 1;}))
    throw std::runtime_error("test_count");
}

template <class A>
void test_find(const std::vector<int>& arr)
{
  rt::set<int, std::less<int>, A> t1;
  t1.insert(std::begin(arr), std::end(arr));
  auto func = [&](int a) -> bool
  {
    auto iter = t1.find(a);
    if (iter == t1.end())
      return false;
    if (*iter != a)
      return false;
    return true;
  };
  const bool b1 = std::all_of(std::begin(arr), std::end(arr), func);
  if (!b1)
    throw std::runtime_error("test_find");
}

template <class A>
void test_basic(const std::vector<int>& tmp)
{
  using set_type = rt::set<int, std::less<int>, A>;
  set_type t1(std::begin(tmp), std::end(tmp));
  // TODO: This member function is not working
  // and must be debugged.
  //t1.insert(std::begin(tmp), std::end(tmp));

  const auto s = t1.size();
  if (s != tmp.size())
    throw std::runtime_error("test_basic");

  if (!std::is_sorted(std::begin(t1), std::end(t1)))
    throw std::runtime_error("test_basic");

  set_type t3(t1);
  if (t3 != t1)
    throw std::runtime_error("test_basic");

  set_type t4 = t3;
  if (t4 != t1)
    throw std::runtime_error("test_basic");

  if (std::adjacent_find(std::begin(t1), std::end(t1)) != std::end(t1))
    throw std::runtime_error("test_basic");

  t1.clear();
  if (!t1.empty())
    throw std::runtime_error("test_basic");
}

template <class A>
void run_tests(const std::vector<int>& tmp)
{
  test_basic<A>(tmp);
  test_count<A>(tmp);
  test_erase<A>(tmp);
  test_find<A>(tmp);
  test_swap<A>();
  test_reverse_traversal<A>();
  test_move<A>();
}

int main()
{
  try {
    const int size = 10;
    const int a = 1;
    const int b = std::numeric_limits<int>::max();

    // Random unique integers in the range [a, b].
    const std::vector<int> tmp = rt::make_rand_data<int>(size, a, b);

    using Node = rt::set<int>::node_type;

    using A1 = std::allocator<int>;
    using A2 = rt::node_allocator<int, Node, unsigned, 2>;
    using A3 = rt::node_allocator<int, Node, unsigned, 4>;
    using A4 = rt::node_allocator<int, Node, unsigned, 128>;

    run_tests<A1>(tmp);
    run_tests<A2>(tmp);
    run_tests<A3>(tmp);
    run_tests<A4>(tmp);
  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
    return 1;
  }
  return 0;
}

