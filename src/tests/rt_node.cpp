#include <string>
#include <iostream>

#include <rtcpp/container/tbst.hpp>
#include <rtcpp/memory/node_traits.hpp>

namespace rt
{

template <class, class>
class link_ptr;

template <class T, class Index>
class idx_ptr {
  private:
  Index* p {nullptr};
  Index idx {};

  public:
  template <class, class>
  friend class link_ptr;

  using index_type = Index;
  using element_type = T;
  template <class U>
  using rebind = idx_ptr<U, Index>;
  using link_pointer = idx_ptr<T, Index>;
  T& operator*() { return *reinterpret_cast<T*>(&p[idx]); }
  const T& operator*() const { return *reinterpret_cast<const T*>(&p[idx]); }
  T* operator->() { return reinterpret_cast<T*>(&p[idx]); }
  const T* operator->() const { return reinterpret_cast<const T*>(&p[idx]); }
  idx_ptr() = default;
  idx_ptr& operator=(const link_ptr<T, Index>& rhs)
  {
    idx = rhs.idx;
    return *this;
  }
  idx_ptr& operator=(const idx_ptr<T, Index>& rhs)
  {
    if (this == &rhs)
      return *this;

    idx = rhs.idx;
    p = rhs.p;
    return *this;
  }
};

template <class T, class Index>
class link_ptr {
  private:
  template <class, class>
  friend class idx_ptr;
  //private:
  Index idx;
  public:
  using index_type = Index;
  using element_type = T;
  template <class U>
  using rebind = idx_ptr<U, Index>;
  link_ptr& operator=(const idx_ptr<T, Index>& rhs)
  {
    idx = rhs.idx;
    return *this;
  }
};

template <class Index>
class idx_ptr_void {
  public:
  using index_type = Index;
  using element_type = void;
  template <class U>
  using rebind = link_ptr<U, Index>;
};

}

namespace std
{

template <class T, class Ptr>
struct pointer_traits<rt::tbst::node<T, Ptr>> {
  template <class U>
  using rebind = rt::link_ptr<T, typename U::index_type>;
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

  using value_type = unsigned char;
  using index_type = unsigned char;
  using node_type = tbst::node<value_type, idx_ptr_void<index_type>>;
  using node_link_type = typename node_type::link_type;
  using node_pointer = idx_ptr<node_type, index_type>;
  node_type node;
  node_link_type p;
  node_pointer q;
  p = q;
  q = p;

  return 0;
}

