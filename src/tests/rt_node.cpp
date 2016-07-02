#include <string>
#include <iostream>

#include <rtcpp/container/tbst.hpp>

namespace rt
{

template <class Index>
struct index_ptr {
  using index_type = Index;
  Index* p;
  Index idx;
};

template <class T, class Ptr>
struct index_ptr<bst_node<T, Ptr>> {
  using index_type = typename Ptr::index_type;
  index_type idx;
};

}

namespace std 
{

template <class T, class Ptr>
struct pointer_traits<rt::bst_node<T, Ptr>> {
};

}

using namespace rt;

int main()
{
  std::cout << "uchar       : " << sizeof (unsigned char) << std::endl;
  std::cout << "short       : " << sizeof (short) << std::endl;
  std::cout << "int         : " << sizeof (int) << std::endl;
  std::cout << "long long   : " << sizeof (long long) << std::endl;
  std::cout << "char*       : " << sizeof (char*) << std::endl;
  std::cout << "std::string : " << sizeof (std::string) << std::endl;

  constexpr std::size_t S1 = sizeof (bst_node<unsigned char, void*>);
  constexpr std::size_t S2 = sizeof (bst_node<short    , void*>);
  constexpr std::size_t S3 = sizeof (bst_node<int      , void*>);
  constexpr std::size_t S4 = sizeof (bst_node<long long, void*>);
  constexpr std::size_t S5 = sizeof (bst_node<char*    , void*>);

  std::cout << "_____" << std::endl;

  std::cout << "bst_node<uchar    , void*>: " << S1 << std::endl;
  std::cout << "bst_node<short    , void*>: " << S2 << std::endl;
  std::cout << "bst_node<int      , void*>: " << S3 << std::endl;
  std::cout << "bst_node<long long, void*>: " << S4 << std::endl;
  std::cout << "bst_node<char*    , void*>: " << S5 << std::endl;

  std::cout << "_____" << std::endl;

  using node1 = bst_node<unsigned char, index_ptr<unsigned char>>;
  using node2 = bst_node<short        , index_ptr<short>        >;
  using node3 = bst_node<int          , index_ptr<int>          >;
  using node4 = bst_node<long long    , index_ptr<long long>    >;
  using node5 = bst_node<char*        , index_ptr<char*>        >;

  using idx_ptr = index_ptr<char>;

  constexpr std::size_t U1 = sizeof (node1);
  constexpr std::size_t U2 = sizeof (node2);
  constexpr std::size_t U3 = sizeof (node3);
  constexpr std::size_t U4 = sizeof (node4);
  constexpr std::size_t U5 = sizeof (node5);

  std::cout << "bst_node<uchar    , index_ptr<uchar>>     : " << U1 << std::endl;
  std::cout << "bst_node<short    , index_ptr<short>>     : " << U2 << std::endl;
  std::cout << "bst_node<int      , index_ptr<int>>       : " << U3 << std::endl;
  std::cout << "bst_node<long long, index_ptr<long long>> : " << U4 << std::endl;
  std::cout << "bst_node<char*    , index_ptr<char*>>     : " << U5 << std::endl;
  return 0;
}

