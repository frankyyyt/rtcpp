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
    idx = rhs.idx;
    p = rhs.p;
    return *this;
  }
  //private:
  Index* p{nullptr};
  Index idx{};
};

template <class T, class Index>
class index_ptr<T, Index, true> {
  public:
  using index_type = Index;
  using element_type = T;
  template <class U>
  using rebind = index_ptr<U, Index, false>;
  template <bool InNode>
  index_ptr& operator=(const index_ptr<T, Index, InNode>& rhs)
  {
    idx = rhs.idx;
    return *this;
  }
  //private:
  index_type idx;
};

template <class Index>
class index_ptr<void, Index, false> {
  public:
  using index_type = Index;
  using element_type = void;
  template <class U>
  using rebind = index_ptr<U, Index, true>;
};

}

namespace std
{

template <class T, class Ptr>
struct pointer_traits<rt::tbst::node<T, Ptr>> {
  template <class U, class Ptr2>
  using rebind = rt::index_ptr<U, typename Ptr2::index_type, true>;
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

  using node1 = tbst::node<unsigned char, index_ptr<void, unsigned char>>;
  using node2 = tbst::node<short        , index_ptr<void, short>        >;
  using node3 = tbst::node<int          , index_ptr<void, int>          >;
  using node4 = tbst::node<long long    , index_ptr<void, long long>    >;
  using node5 = tbst::node<char*        , index_ptr<void, char*>        >;

  constexpr std::size_t U1 = sizeof (node1);
  constexpr std::size_t U2 = sizeof (node2);
  constexpr std::size_t U3 = sizeof (node3);
  constexpr std::size_t U4 = sizeof (node4);
  constexpr std::size_t U5 = sizeof (node5);

  std::cout << "tbst::node<uchar    , index_ptr<uchar>>     : " << U1 << std::endl;
  std::cout << "tbst::node<short    , index_ptr<short>>     : " << U2 << std::endl;
  std::cout << "tbst::node<int      , index_ptr<int>>       : " << U3 << std::endl;
  std::cout << "tbst::node<long long, index_ptr<long long>> : " << U4 << std::endl;
  std::cout << "tbst::node<char*    , index_ptr<char*>>     : " << U5 << std::endl;

  using value_type = unsigned char;
  using index_type = unsigned char;
  using node_type = tbst::node<value_type, index_ptr<void, index_type>>;
  using node_self_pointer = typename node_type::self_pointer;
  using node_pointer = index_ptr<node_type, index_type>;
  node_type node;
  node_self_pointer p;
  node_pointer q;
  p = q;
  q = p;

  return 0;
}

