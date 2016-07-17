#include <string>
#include <iostream>

#include <rtcpp/container/tbst.hpp>
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

  using node1 = tbst::node<unsigned char, idx_ptr_void<unsigned char>>;
  using node2 = tbst::node<short        , idx_ptr_void<short>        >;
  using node3 = tbst::node<int          , idx_ptr_void<int>          >;
  using node4 = tbst::node<long long    , idx_ptr_void<long long>    >;
  using node5 = tbst::node<char*        , idx_ptr_void<char*>        >;

  constexpr std::size_t U1 = sizeof (node1);
  constexpr std::size_t U2 = sizeof (node2);
  constexpr std::size_t U3 = sizeof (node3);
  constexpr std::size_t U4 = sizeof (node4);
  constexpr std::size_t U5 = sizeof (node5);

  std::cout << "tbst::node<uchar    , idx_ptr<uchar>>     : " << U1 << std::endl;
  std::cout << "tbst::node<short    , idx_ptr<short>>     : " << U2 << std::endl;
  std::cout << "tbst::node<int      , idx_ptr<int>>       : " << U3 << std::endl;
  std::cout << "tbst::node<long long, idx_ptr<long long>> : " << U4 << std::endl;
  std::cout << "tbst::node<char*    , idx_ptr<char*>>     : " << U5 << std::endl;
}

int main()
{
  print_data_type_size();
  print_node_sizes();

  using value_type = unsigned char;
  using index_type = unsigned char;

  using node_type1 = tbst::node<value_type, void*>;

  //using alloc_type = node_allocator<value_type, node_type1>;
  //using alloc_traits_type = rt::allocator_traits<alloc_type>;
  //using void_pointer = typename alloc_traits_type::void_pointer;
  //using node_type = tbst::node<value_type, void_pointer>;
  //using inner_alloc_type =
  //  typename alloc_traits_type::template rebind_alloc<node_type>;
  //using inner_alloc_traits_type =
  //  rt::allocator_traits<inner_alloc_type>;
  //using node_pointer = typename inner_alloc_traits_type::pointer;

  //constexpr auto s = sizeof (node_type);
  //std::cout << s << std::endl;

  //using node_link_type = typename node_type::link_type;
  //using node_pointer = idx_ptr<node_type, index_type>;
  //node_type node;
  //node_link_type p;
  //node_pointer q;
  //p = q;
  //q = p;


  //using inner_alloc_type =
  //  typename alloc_traits_type::template rebind_alloc<node_type>;

  //node_alloc_header header;
  //alloc_type alloc(&header);
  //inner_alloc_type inner_alloc(alloc);

  //auto k = inner_alloc.allocate_node();

  return 0;
}

