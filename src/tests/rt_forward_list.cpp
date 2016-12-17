#include <iostream>
#include <vector>
#include <random>
#include <algorithm>
#include <functional>

#include <rtcpp/utility/print.hpp>
#include <rtcpp/memory/node_allocator.hpp>
#include <rtcpp/container/forward_list.hpp>
#include <rtcpp/utility/make_rand_data.hpp>

using namespace rt;

struct foo {
  static int n_default_ctor;
  static int n_dctor;
  static int n_copy_ctor;
  static int n_mv_ctor;
  static int n_copy_assignment;
  static int n_mv_assignment;
  static void reset()
  {
    n_default_ctor = 0;
    n_dctor = 0;
    n_copy_ctor = 0;
    n_mv_ctor = 0;
    n_copy_assignment = 0;
    n_mv_assignment = 0;
  }
  foo()
  {
    std::cout << "foo()" << std::endl;
    ++n_default_ctor;
  }

  ~foo()
  {
    std::cout << "~foo()" << std::endl;
    ++n_dctor;
  }

  foo(const foo&)
  {
    std::cout << "foo(const foo&)" << std::endl;
    ++n_copy_ctor;
  }

  foo(foo&&)
  {
    std::cout << "foo(foo&&)" << std::endl;
    ++n_mv_ctor;
  }

  foo& operator=(const foo&)
  {
    std::cout << "foo& operator=(const foo&)" << std::endl;
    ++n_copy_assignment;
    return *this;
  }

  foo& operator=(foo&&)
  {
    std::cout << "foo& operator=(foo&&)" << std::endl;
    ++n_mv_assignment;
    return *this;
  }
};

int foo::n_default_ctor = 0;
int foo::n_dctor = 0;
int foo::n_copy_ctor = 0;
int foo::n_mv_ctor = 0;
int foo::n_copy_assignment = 0;
int foo::n_mv_assignment = 0;

void test_push_front_copy()
{
  std::cout << "test_push_front_copy" << std::endl;
  foo::reset();
  // This container will allocate the head node and therefore
  // has to be taken into account.
  forward_list<foo> l;
  foo bar;
  l.push_front(bar);
  const foo bar2;
  l.push_front(bar2);

  if (foo::n_default_ctor != 3)
    throw std::runtime_error("forward_list");
  if (foo::n_copy_ctor != 2)
    throw std::runtime_error("forward_list");
  if (foo::n_mv_ctor != 1)
    throw std::runtime_error("forward_list");
  if (foo::n_copy_assignment != 0)
    throw std::runtime_error("forward_list");
  if (foo::n_mv_assignment != 0)
    throw std::runtime_error("forward_list");
}

void test_push_front_move()
{
  std::cout << "test_push_front_move" << std::endl;
  foo::reset();
  forward_list<foo> l;
  foo bar;
  l.push_front(std::move(bar));
  foo bar1;
  l.push_front(bar1);
  l.push_front(foo());
  const foo bar2;
  l.push_front(bar2);
  if (foo::n_default_ctor != 5)
    throw std::runtime_error("forward_list");
  if (foo::n_copy_ctor != 2)
    throw std::runtime_error("forward_list");
  if (foo::n_mv_ctor != 3)
    throw std::runtime_error("forward_list");
  if (foo::n_copy_assignment != 0)
    throw std::runtime_error("forward_list");
  if (foo::n_mv_assignment != 0)
    throw std::runtime_error("forward_list");
}

template <class A>
void test_basic()
{
    using value_type = typename A::value_type;
    const value_type size = 60;

    const value_type a = 1;
    const value_type b = size;

    const auto data = make_rand_data<value_type>(size, a, b, 1);
    forward_list<value_type, A> l;

    std::copy(std::begin(data), std::end(data), std::front_inserter(l));

    const auto b1 = std::equal(std::rbegin(data), std::rend(data),
                               std::begin(l));
    if (!b1)
      throw std::runtime_error("forward_list");

    l.reverse();
    const auto b2 = std::equal(std::begin(data), std::end(data),
                               std::begin(l));
    if (!b2)
      throw std::runtime_error("forward_list");

    l.reverse();
    const auto b3 = std::equal(std::rbegin(data), std::rend(data),
                               std::begin(l));
    if (!b3)
      throw std::runtime_error("forward_list");

    l.insertion_sort();

    if (!std::is_sorted(std::begin(l), std::end(l)))
      throw std::runtime_error("forward_list");

    print(l);
    l.sorted_insertion(3);
    l.sorted_insertion(-1);
    l.sorted_insertion(20);
    l.sorted_insertion(7);
    print(l);

    if (!std::is_sorted(std::begin(l), std::end(l)))
      throw std::runtime_error("forward_list");

    l.push_front(10101);

    if (std::is_sorted(std::begin(l), std::end(l)))
      throw std::runtime_error("forward_list");

    l.remove_if(1);
    l.remove_if(2);
    l.remove_if(4);

    l.clear();
    if (!l.empty())
      throw std::runtime_error("forward_list");

    l.clear();
    l.insert_after(l.begin(), 3, 10);
    l.push_front(7);
    l.insert_after(l.begin(), 3, 20);
    l.push_front(7);
    print(l);
}

int main()
{
  try {

    using Node = rt::forward_list<int>::node_type;

    using A1 = std::allocator<int>;
    using A2 = rt::node_allocator<int, Node, unsigned, 2>;
    //using A3 = rt::node_allocator<int, Node, unsigned, 4>;
    //using A4 = rt::node_allocator<int, Node, unsigned, 128>;

    test_basic<A1>();
    test_basic<A2>();
    //test_basic<A3>();
    //test_basic<A4>();
    test_push_front_copy();
    test_push_front_move();
  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
    return 1;
  }

  return 0;
}

