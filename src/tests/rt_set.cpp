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
#include <rtcpp/memory/node_allocator_lazy.hpp>
#include <rtcpp/memory/node_allocator.hpp>
#include <rtcpp/utility/make_rand_data.hpp>
#include <rtcpp/utility/print.hpp>

bool test_move()
{
  rt::set<int> t1 = {1, 2, 3, 4, 5};
  rt::set<int> t2(std::move(t1));

  if (!t1.empty())
    return false;

  //print(t1);
  //print(t2);

  return true;
}
bool test_reverse_traversal()
{
  constexpr int size = 10;
  constexpr std::array<int, size> o = {{0, 1, 2, 3, 4, 5, 6, 7, 8, 9}};
  constexpr std::array<int, size> arr = {{o[5], o[2], o[7], o[9], o[8], o[3], o[0], o[4], o[6], o[1]}};
  rt::set<int> t1(std::begin(arr), std::end(arr));

  // Now the tree has three items 3, 2, 4. Lets test if the iterators can get
  // us to the right point.
  auto iter = t1.end();

  // We should be now at the rbegin.

  for (int i = size - 1; i != -1; --i )
    if (*--iter != o[i])
      return false;

  return true;
}

template <typename T>
bool test_swap(const std::vector<T>& arr)
{
  rt::set<T> t1(std::begin(arr), std::end(arr));
  rt::set<T> t1_copy = t1;
  rt::set<T> t2 = {1, 4, 8, 2, 7};
  rt::set<T> t2_copy = t2;
  std::swap(t1, t2);

  if (t1 != t2_copy)
    return false;

  if (t2 != t1_copy)
    return false;

  return true;
}

template <typename C>
bool test_erase(C& t1, const std::vector<typename C::value_type>& arr)
{
  t1.clear();

  t1.insert(std::begin(arr), std::end(arr));
  for (auto v : arr) { // Removes forward
    t1.erase(v);
    if (t1.find(v) != std::end(t1))
      return false;
  }

  if (!t1.empty())
    return false;

  t1.insert(std::begin(arr), std::end(arr));
  for (std::size_t i = 0; i < arr.size(); ++i) { // Removes backwards.
    const auto v = arr[arr.size() - i - 1];
    t1.erase(v);
    if (t1.find(v) != std::end(t1))
      return false;
  }

  if (!t1.empty())
    return false;

  return true;
}

template <typename C>
bool test_count(C& t1, const std::vector<typename C::value_type>& arr)
{
  typedef typename C::value_type value_type;
  t1.clear();
  t1.insert(std::begin(arr), std::end(arr));
  if (!std::all_of(std::begin(arr), std::end(arr), [&](value_type a){ return t1.count(a) == 1;}))
    return false;

  return true;
}

template <typename C>
bool test_find(C& t1,const std::vector<typename C::value_type>& arr)
{
  typedef typename C::value_type value_type;
  t1.clear();
  t1.insert(std::begin(arr), std::end(arr));
  auto func = [&](value_type a) -> bool
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
    return false;

  return true;
}

template <typename C>
bool test_basic(C& t1, const std::vector<typename C::value_type>& tmp)
{
  t1.clear();
  t1.insert(std::begin(tmp), std::end(tmp));

  if (t1.size() != tmp.size())
    return false;

  if (!std::is_sorted(std::begin(t1), std::end(t1)))
    return false;

  C t3(t1);
  if (t3 != t1)
    return false;

  C t4 = t3;
  if (t4 != t1)
    return false;

  if (std::adjacent_find(std::begin(t1), std::end(t1)) != std::end(t1))
    return false;

  t1.clear();
  if (!t1.empty())
    return false;

  return true;
}

template <typename C>
bool run_tests(C& t1, const std::vector<typename C::value_type>& tmp)
{
  typedef typename C::value_type value_type;
  if (!test_basic(t1, tmp))
    return false;

  if (!test_count(t1, tmp))
    return false;

  if (!test_erase(t1, tmp))
    return false;

  if (!test_find(t1, tmp))
    return false;

  if (!test_swap<value_type>(tmp))
    return false;

  if (!test_reverse_traversal())
    return false;

  if (!test_move())
    return false;

  return true;
}

template <class T, class A>
using set_type = rt::set<T, std::less<T>, A>;

bool test_node_alloc_size()
{
  using T = unsigned char;
  using L = unsigned char;
  using alloc_type1 = std::allocator<T>;
  using type1 = set_type<T, alloc_type1>;

  using alloc_type3 = rt::node_allocator<T, L>;
  using type3 = set_type<T, alloc_type3>;
  const auto node_size3 =
    sizeof (typename set_type<T, alloc_type3>::node_type);

  const T max = std::numeric_limits<T>::max();

  // Random unique integers in the range [a, b].
  std::vector<T> tmp(max - 2);
  std::iota(std::begin(tmp), std::end(tmp), 0);

  type3 t3;

  t3.insert(std::begin(tmp), std::end(tmp));
  print(t3);
  std::cout << std::endl;
  //std::cout << "aaaaaaaaa" << std::endl;

  return true;
}

template <class T, class L>
bool test_with_node_alloc()
{
  using alloc_type1 = std::allocator<T>;
  using type1 = set_type<T, alloc_type1>;

  using alloc_type3 = rt::node_allocator<T, L>;
  using type3 = set_type<T, alloc_type3>;
  const auto node_size3 =
    sizeof (typename set_type<T, alloc_type3>::node_type);

  const T size = 10;
  const T a = std::numeric_limits<T>::min();
  const T b = std::numeric_limits<T>::max();

  // Random unique integers in the range [a, b].
  const std::vector<T> tmp = rt::make_rand_data<T>(size, a, b);
  const auto bsize = 10 * (tmp.size() + 1) * node_size3;

  type3 t3;

  return run_tests(t3, tmp);
}

template <class T>
bool run_tests_all()
{
  using L = std::size_t;
  using alloc_type1 = std::allocator<T>;
  using type1 = set_type<T, alloc_type1>;
  const auto node_size1 =
    sizeof (typename set_type<T, alloc_type1>::node_type);

  using alloc_type2 = rt::node_allocator_lazy<T>;
  using type2 = set_type<T, alloc_type2>;
  const auto node_size2 =
    sizeof (typename set_type<T, alloc_type2>::node_type);

  const T size = 1000;
  const T a = 1;
  const T b = std::numeric_limits<T>::max();

  // Random unique integers in the range [a, b].
  const std::vector<T> tmp = rt::make_rand_data<T>(size, a, b);

  // Should be enough for rt::set.
  const auto bsize = 10 * (tmp.size() + 1) * node_size1;

  type1 t1;
  if (!run_tests(t1, tmp))
    return false;

  alloc_type2 alloc1(bsize);
  type2 t2(alloc1);
  if (!run_tests(t2, tmp))
    return false;

  if (!test_with_node_alloc<std::size_t, std::size_t>())
    return false;

  if (!test_with_node_alloc<unsigned int, std::size_t>())
    return false;

  if (!test_with_node_alloc<unsigned short, std::size_t>())
    return false;

  if (!test_with_node_alloc<unsigned char, std::size_t>())
    return false;

  if (!test_with_node_alloc<unsigned int, unsigned int>())
    return false;

  if (!test_with_node_alloc<unsigned short, unsigned short>())
    return false;

  if (!test_with_node_alloc<unsigned char, unsigned char>())
    return false;

  return true;
}

int main()
{
  const bool b1 = run_tests_all<int>();
  const bool b2 = run_tests_all<long long int>();
  const bool b3 = test_node_alloc_size();
  return (b1 && b2 && b3) ? 0 : 1;
}

