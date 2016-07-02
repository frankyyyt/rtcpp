#include <string>
#include <iostream>

#include <rtcpp/container/tbst.hpp>
#include <rtcpp/memory/node_traits.hpp>

namespace rt
{

template <class T, class Index, bool InNode = false>
class index_ptr {
  public:
  using index_type = Index;
  using element_type = T;
  template <class U>
  using rebind = index_ptr<U, Index, InNode>;
  T& operator*()
  {
    return *reinterpret_cast<T*>(&p[idx]);
  }
  const T& operator*() const
  {
    return *reinterpret_cast<const T*>(&p[idx]);
  }
  T* operator->()
  {
    return reinterpret_cast<T*>(&p[idx]);
  }
  const T* operator->() const
  {
    return reinterpret_cast<const T*>(&p[idx]);
  }
  index_ptr() = default;
  index_ptr& operator=(const index_ptr<T, Index, true>& rhs)
  {
    idx = rhs.idx;
    return *this;
  }
  index_ptr& operator=(const index_ptr<T, Index, false>& rhs)
  {
    //if (this == &rhs)
    //  return *this;

    idx = rhs.idx;
    p = rhs.p;

    return *this;
  }
  private:
  Index* p{nullptr};
  Index idx{};
};

template <class T, class Index>
class index_ptr<T, Index, true> {
  public:
  using index_type = Index;
  using element_type = T;
  template <class U>
  using rebind = index_ptr<U, Index, is_node_type<U>::value>;
  template <bool InNode>
  index_ptr& operator=(const index_ptr<T, Index, InNode>& rhs)
  {
    idx = rhs.idx;
    return *this;
  }
  private:
  index_type idx;
};

}

namespace std
{

template <class T, class Ptr>
struct pointer_traits<rt::bst_node<T, Ptr>> {
  using rebind = rt::index_ptr<T, typename Ptr::index_type, false>;
  using element_type = T;
};

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

using namespace rt;

int main()
{

  void print_data_type_size();

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

  using node1 = bst_node<unsigned char, index_ptr<int, unsigned char>>;
  using node2 = bst_node<short        , index_ptr<int, short>        >;
  using node3 = bst_node<int          , index_ptr<int, int>          >;
  using node4 = bst_node<long long    , index_ptr<int, long long>    >;
  using node5 = bst_node<char*        , index_ptr<int, char*>        >;

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

  using node = bst_node<unsigned, index_ptr<unsigned, unsigned>>;
  index_ptr<node, unsigned> p;
  node q;

  p = q.link[0];

  return 0;
}

