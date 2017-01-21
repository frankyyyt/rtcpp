#include <string>
#include <iostream>
#include <algorithm>
#include <exception>

#include <rtcpp/utility/print.hpp>
#include <rtcpp/container/set.hpp>
#include <rtcpp/container/tbst.hpp>
#include <rtcpp/memory/node_ptr.hpp>
#include <rtcpp/memory/node_storage.hpp>
#include <rtcpp/memory/node_allocator.hpp>

#include "rt_test.hpp"

template <class Ptr>
RT_TEST(test_nullable_ptr)
{
  Ptr p1; // Default constructable.
  Ptr p2(p1); // Copy costructable.
  Ptr p3 = p1; // Copy assignable.

  // Equality comparable.
  RT_CHECK(p1 == p2);
  RT_CHECK(p3 == p2);
  RT_CHECK(!p3);
}

void test_node_ptr_type()
{
  using namespace rt;

  using node_strg_t = node_storage<unsigned, unsigned, 4>;
  using ptr_t = node_ptr<unsigned, unsigned, 4>;
  using cptr_t = const_node_ptr<unsigned, unsigned, 4>;
  using link_t = node_link<unsigned>;
  using void_ptr_t = void_node_ptr<unsigned>;
  using cvoid_ptr_t = const_void_node_ptr<unsigned>;

  test_nullable_ptr<ptr_t>();
  test_nullable_ptr<cptr_t>();
  test_nullable_ptr<link_t>();
  test_nullable_ptr<void_ptr_t>();
  test_nullable_ptr<cvoid_ptr_t>();

  node_strg_t strg;
  void_ptr_t p1(&strg, 8);
  cvoid_ptr_t p2 = p1;
  (void) p2;
  //void_ptr_t p3 = p2; // Error
}

template <class T, class L>
void test_node_storage()
{
  constexpr unsigned N = 4;

  rt::node_storage<T, L, N> strg;

  // Initially there must be zero blocks.
  if (strg.get_n_blocks() != 0)
    throw std::runtime_error("get_n_blocks: 0");

  auto p1 = strg.pop();
  *p1 = 10;
  // The last element in the stack must be returned.
  if (p1.get_link().get_idx() != 3)
    throw std::runtime_error("pop: 3");

  // After allocating one element we must have one block.
  if (strg.get_n_blocks() != 1)
    throw std::runtime_error("get_n_blocks: 1");

  auto p2 = strg.pop();
  *p2 = 11;
  // The last element in the stack must be returned.
  if (p2.get_link().get_idx() != 2)
    throw std::runtime_error("pop: 2");

  auto p3 = strg.pop();
  *p3 = 12;
  if (p3.get_link().get_idx() != 1)
    throw std::runtime_error("pop: 1");

  // We still must have only one block.
  if (strg.get_n_blocks() != 1)
    throw std::runtime_error("get_n_blocks: 1");

  // These should trigger another block.
  auto p4 = strg.pop();
  *p4 = 13;
  if (p4.get_link().get_idx() != 7)
    throw std::runtime_error("pop: 7");

  // We must have only two blocks now.
  if (strg.get_n_blocks() != 2)
    throw std::runtime_error("get_n_blocks: 2");

  if (*p1 != 10 || *p2 != 11 || *p3 != 12 || *p4 != 13)
    throw std::runtime_error("get_n_blocks: 2");

  // Now we release some memory.
  strg.push(p1);
  strg.push(p2);
  strg.push(p4);

  const auto p5 = strg.pop();
  if (p5.get_link().get_idx() != 7)
    throw std::runtime_error("pop: 7");

  const auto p6 = strg.pop();
  if (p6.get_link().get_idx() != 2)
    throw std::runtime_error("pop: 2");
}

void test_node_ptr()
{
  rt::node_storage<unsigned, unsigned, 4> a;
  rt::node_storage<unsigned, unsigned, 4> b;

  rt::node_ptr<unsigned, unsigned, 4> p1(&a, 1);
  rt::node_ptr<unsigned, unsigned, 4> p2(&b, 2);

  std::swap(p1, p2);

  if (p1.get_strg() != &b)
    throw std::runtime_error("test_node_ptr_swap");

  if (p1.get_link().get_idx() != 2)
    throw std::runtime_error("test_node_ptr_swap");

  if (p2.get_strg() != &a)
    throw std::runtime_error("test_node_ptr_swap");

  if (p2.get_link().get_idx() != 1)
    throw std::runtime_error("test_node_ptr_swap");
}

void print_node_sizes()
{
  using namespace rt;
  constexpr std::size_t S1 = sizeof (tbst::node<unsigned char, void*>);
  constexpr std::size_t S2 = sizeof (tbst::node<short    , void*>);
  constexpr std::size_t S3 = sizeof (tbst::node<int      , void*>);
  constexpr std::size_t S4 = sizeof (tbst::node<long long, void*>);
  constexpr std::size_t S5 = sizeof (tbst::node<char*    , void*>);

  std::cout << "_____" << std::endl;

  std::cout << "tbst::node<uchar    , void*>: " << S1 << std::endl;
  std::cout << "tbst::node<short    , void*>: " << S2 << std::endl;
  std::cout << "tbst::node<int      , void*>: " << S3 << std::endl;
  std::cout << "tbst::node<long long, void*>: " << S4 << std::endl;
  std::cout << "tbst::node<char*    , void*>: " << S5 << std::endl;

  std::cout << "_____" << std::endl;

  using node1 = tbst::node<unsigned char, void_node_ptr<unsigned char>>;
  using node2 = tbst::node<short        , void_node_ptr<short>        >;
  using node3 = tbst::node<int          , void_node_ptr<int>          >;
  using node4 = tbst::node<long long    , void_node_ptr<long long>    >;
  using node5 = tbst::node<char*        , void_node_ptr<char*>        >;

  constexpr std::size_t U1 = sizeof (node1);
  constexpr std::size_t U2 = sizeof (node2);
  constexpr std::size_t U3 = sizeof (node3);
  constexpr std::size_t U4 = sizeof (node4);
  constexpr std::size_t U5 = sizeof (node5);

  std::cout << "tbst::node<uchar    , node_ptr<uchar>>     : " << U1 << std::endl;
  std::cout << "tbst::node<short    , node_ptr<short>>     : " << U2 << std::endl;
  std::cout << "tbst::node<int      , node_ptr<int>>       : " << U3 << std::endl;
  std::cout << "tbst::node<long long, node_ptr<long long>> : " << U4 << std::endl;
  std::cout << "tbst::node<char*    , node_ptr<char*>>     : " << U5 << std::endl;
}

void print_data_type_size()
{
  std::cout << "uchar       : " << sizeof (unsigned char) << std::endl;
  std::cout << "short       : " << sizeof (short) << std::endl;
  std::cout << "int         : " << sizeof (int) << std::endl;
  std::cout << "long long   : " << sizeof (long long) << std::endl;
  std::cout << "char*       : " << sizeof (char*) << std::endl;
  std::cout << "std::string : " << sizeof (std::string) << std::endl;
}

void test_node()
{
  using namespace rt;
  print_data_type_size();
  print_node_sizes();

  using T = unsigned char;
  using L = unsigned char;
  using Node = typename rt::set<T>::node_type;

  using node_type1 = tbst::node<T, void*>;
  std::cout << sizeof (node_type1) << std::endl;

  using alloc_type = node_allocator<T, Node, L, 256>;
  using alloc_traits_type = rt::allocator_traits<alloc_type>;
  using link_type = typename alloc_traits_type::link_type;
  using node_type = tbst::node<T, link_type>;
  using inner_alloc_type =
    typename alloc_type::template rebind<node_type>::other;
  using inner_alloc_traits_type =
    rt::allocator_traits<inner_alloc_type>;

  using value_type = typename inner_alloc_traits_type::value_type;

  std::cout << sizeof (value_type) << std::endl;

  inner_alloc_type inner_alloc;

  auto k = inner_alloc.allocate_node();
  inner_alloc.deallocate_node(k);
}

void test_swap()
{
  using namespace rt;
  using Node = typename rt::set<unsigned>::node_type;

  using alloc_type_tmp = node_allocator<unsigned, Node, unsigned, 2>;
  using node_type = typename alloc_type_tmp::node_type;
  using alloc_type = node_allocator<node_type, Node, unsigned, 2>;

  alloc_type alloc1;
  alloc_type alloc1_copy(alloc1);
  alloc_type alloc2;
  alloc_type alloc2_copy(alloc2);

  if (alloc1.get_n_blocks() != 0)
    throw std::runtime_error("test_swap");

  if (alloc2.get_n_blocks() != 0)
    throw std::runtime_error("test_swap");

  auto p1 = alloc1.allocate_node();

  if (alloc1.get_n_blocks() != 1)
    throw std::runtime_error("test_swap");

  if (alloc1_copy.get_n_blocks() != 1)
    throw std::runtime_error("test_swap");

  alloc1.swap(alloc2);

  if (alloc2.get_n_blocks() != 1)
    throw std::runtime_error("test_swap");

  if (alloc1.get_n_blocks() != 0)
    throw std::runtime_error("test_swap");

  alloc2.deallocate_node(p1);

  if (alloc1 != alloc2_copy)
    throw std::runtime_error("test_swap");

  if (alloc2 != alloc1_copy)
    throw std::runtime_error("test_swap");
}

int main()
{
  try {
    test_node_storage<unsigned char         , unsigned char>();
    test_node_storage<unsigned short        , unsigned short>();
    test_node_storage<unsigned int          , unsigned int>();
    test_node_storage<unsigned long int     , unsigned long int>();
    test_node_storage<unsigned long long int, unsigned long long int>();

    test_node_storage<unsigned char         , unsigned long long int>();
    test_node_storage<unsigned long long int, unsigned char>();

    test_node_ptr();
    test_node_ptr_type();
    test_node();

  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
    return 1;
  }
  return 0;
}

