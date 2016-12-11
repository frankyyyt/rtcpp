#include <string>
#include <iostream>

#include <rtcpp/container/tbst.hpp>
#include <rtcpp/container/set.hpp>
#include <rtcpp/memory/node_allocator.hpp>

void print_data_type_size()
{
  std::cout << "uchar       : " << sizeof (unsigned char) << std::endl;
  std::cout << "short       : " << sizeof (short) << std::endl;
  std::cout << "int         : " << sizeof (int) << std::endl;
  std::cout << "long long   : " << sizeof (long long) << std::endl;
  std::cout << "char*       : " << sizeof (char*) << std::endl;
  std::cout << "std::string : " << sizeof (std::string) << std::endl;
}

using namespace rt;

void print_node_sizes()
{
  constexpr std::size_t N = 8;
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

  using node1 = tbst::node<unsigned char, node_ptr_void<unsigned char, N>>;
  using node2 = tbst::node<short        , node_ptr_void<short, N>        >;
  using node3 = tbst::node<int          , node_ptr_void<int, N>          >;
  using node4 = tbst::node<long long    , node_ptr_void<long long, N>    >;
  using node5 = tbst::node<char*        , node_ptr_void<char*, N>        >;

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

void test_node()
{
  print_data_type_size();
  print_node_sizes();

  using T = unsigned char;
  using link_type = unsigned char;
  using Node = typename rt::set<T>::node_type;

  using node_type1 = tbst::node<T, void*>;
  std::cout << sizeof (node_type1) << std::endl;

  using alloc_type = node_allocator<T, Node, link_type, 256>;
  using alloc_traits_type = rt::allocator_traits<alloc_type>;
  using void_pointer = typename alloc_traits_type::void_pointer;
  using node_type = tbst::node<T, void_pointer>;
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
  test_node();

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
    test_node();
    test_swap();
  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
    return 1;
  }
}

