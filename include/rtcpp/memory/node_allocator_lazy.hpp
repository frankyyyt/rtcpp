#pragma once

#include <array>
#include <vector>
#include <utility>
#include <memory>
#include <exception>
#include <type_traits>

#include "node_traits.hpp"
#include "node_alloc_header.hpp"

/*
  Implementation of a node allocator.  It performs constant time
  allocation on a pre-allocated buffer.

  Node allocation can happen only when the node size is greater than
  or equal to the size of a pointer, otherwise it is not possible to
  link nodes together.  Therefore this class has a specialization for
  such types.

  This allocator is lazy, that means, when it is constructed
  only the pointer to the buffer and the size is stored. Only on 
  copy construction this buffer is divided in blocks and linked
  together. Copy construction occurrs inside the container with
  the new rebound allocator type.
*/

namespace rt {

template <class T, std::size_t S = sizeof (T),
          bool B = is_node<T>::value>
class node_allocator_lazy {
  public:
  using value_type = T;
  using pointer = T*;
  using const_pointer = const T*;
  using reference = T&;
  using const_reference = const T&;
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;
  template<class U>
  struct rebind {
    using other = node_allocator_lazy<U, sizeof (U),
      is_node<U>::value>;
  };
  template<class U>
  node_allocator_lazy(const node_allocator_lazy<U, sizeof (U),
                      true>& a) {}
  template<class U>
  void destroy(U* p) {p->~U();}
  template<class U, typename... Args>
  void construct(U* p, Args&&... args)
  {::new((void *)p) U(std::forward<Args>(args)...);}
  pointer allocate(size_type n)
  { return std::allocator<T>().allocate(n); }
  void deallocate(pointer p, size_type n)
  { std::allocator<T>().deallocate(p, n); }
};

template <typename T , std::size_t N>
class node_allocator_lazy<T, N, true> {
  static constexpr std::size_t R = sizeof (T) / sizeof (std::size_t);
  public:
  using value_type = T;
  using pointer = T*;
  using const_pointer = const T*;
  using reference = T&;
  using const_reference = const T&;
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;
  using Index = std::size_t;
  template<class U>
  struct rebind {
    using other = node_allocator_lazy<U, sizeof (U), is_node<U>::value>;
  };
  public:
  std::shared_ptr<node_alloc_header<T, std::size_t, 1024>> header;
  public:

  node_allocator_lazy()
  : header(std::make_shared<node_alloc_header<T, std::size_t, 1024>>())
  {}

  template<class U>
  node_allocator_lazy(const node_allocator_lazy<U, sizeof (U),
                      true>& a)
  {
    if (!header)
      header =
        std::make_shared<node_alloc_header<T, std::size_t, 1024>>();
  }

  template<class U>
  node_allocator_lazy(const node_allocator_lazy<U, sizeof (U),
                      false>& a)
  : header(std::make_shared<node_alloc_header<T, std::size_t, 1024>>())
  {}

  pointer allocate_node()
  {
    const auto i = header->pop(); 
    if (!i)
      throw std::bad_alloc();

    const auto p = header->buffer;
    return reinterpret_cast<pointer>(&p[i * R]);
  }
  pointer allocate(size_type) { return allocate_node(); }
  void deallocate_node(pointer p)
  {
    const auto a = header->buffer;
    const auto b = reinterpret_cast<Index*>(p);
    header->push(((b - a) / R));
  }
  void deallocate(pointer p, size_type) { deallocate_node(p); }
  template<typename U>
  void destroy(U* p) {p->~U();}
  template< typename U, typename... Args>
  void construct(U* p, Args&&... args)
  {::new((void *)p) U(std::forward<Args>(args)...);}
  void swap(node_allocator_lazy& other) noexcept
  {
    std::swap(header, other.header);
  }
  pointer address(reference x) const noexcept
  { return std::addressof(x); }
  const_pointer address(const_reference x) const noexcept
  { return std::addressof(x); }
  constexpr std::size_t max_size() const {return 1;}
};

template <typename T>
bool operator==( const node_allocator_lazy<T>& alloc1
               , const node_allocator_lazy<T>& alloc2)
{return alloc1.header == alloc2.header;}

template <typename T>
bool operator!=( const node_allocator_lazy<T>& alloc1
               , const node_allocator_lazy<T>& alloc2)
{return !(alloc1 == alloc2);}

}

namespace std {

template <typename T>
struct allocator_traits<rt::node_allocator_lazy<T>> {
  using is_always_equal = std::false_type;
  using allocator_type = typename rt::node_allocator_lazy<T>;
  using size_type = typename allocator_type::size_type;
  using pointer = typename allocator_type::pointer;
  using value_type = typename allocator_type::value_type;
  using difference_type = typename allocator_type::difference_type;
  using const_pointer = typename allocator_type::const_pointer;
  using propagate_on_container_copy_assignment = std::true_type;
  using propagate_on_container_move_assignment = std::true_type;
  using void_pointer = typename std::pointer_traits<pointer>::template rebind<void>;
  using const_void_pointer = typename std::pointer_traits<pointer>::template rebind<const void>;
  using propagate_on_container_swap = std::true_type;
  template<typename U>
  using rebind_alloc =
    typename allocator_type::template rebind<U>::other;
  static allocator_type
    select_on_container_copy_construction(const allocator_type& a)
    {return a;}
  static pointer allocate(allocator_type& a, size_type n)
  {return a.allocate(n);}
  static pointer allocate_node(allocator_type& a)
  {return a.allocate_node();}
  static void deallocate( allocator_type& a, pointer p
                        , size_type n) {a.deallocate(p, n);}
  static void deallocate_node( allocator_type& a
                        , pointer p) {a.deallocate_node(p);}
  template<class U>
  static void destroy(allocator_type& a, U* p) {a.destroy(p);}
  template<class U, class... Args >
  static void construct(allocator_type& a, U* p, Args&&... args)
  {a.construct(p, std::forward<Args>(args)...);}
  constexpr std::size_t max_size(allocator_type& a) const {return a.max_size();}
};

template <typename T>
void swap(rt::node_allocator_lazy<T>& s1, rt::node_allocator_lazy<T>& s2)
{
  s1.swap(s2);
}

}

