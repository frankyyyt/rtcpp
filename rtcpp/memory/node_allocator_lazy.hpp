#pragma once

#include <array>
#include <vector>
#include <utility>
#include <memory>
#include <exception>
#include <type_traits>

#include "node_stack.hpp"
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

template < typename T
         , std::size_t S = sizeof (T)
         , bool B = !(S < sizeof (char*))
         >
class node_allocator_lazy {
  public:
  node_alloc_header* header;
  public:
  static constexpr std::size_t memory_use = node_stack::memory_use;
  using use_node_allocation = std::true_type;
  using value_type = T;
  using pointer = T*;
  using const_pointer = const T*;
  using reference = T&;
  using const_reference = const T&;
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;
  template<typename U>
  struct rebind {
    using other = node_allocator_lazy< U
                                     , sizeof (U)
                                     , !(sizeof (U) < sizeof (char*))>;
  };
  void swap(node_allocator_lazy& other) noexcept
  {
    std::swap(header, other.header);
  }
  node_allocator_lazy(node_alloc_header* p) : header(p) {}
  template<typename U>
  node_allocator_lazy(const node_allocator_lazy< U, sizeof (U)
                           , !(sizeof (U) < sizeof (char*))>& alloc)
  : header(alloc.header) {}
  template<typename U>
  void destroy(U* p) {p->~U();}
  template< typename U, typename... Args>
  void construct(U* p, Args&&... args)
  {::new((void *)p) U(std::forward<Args>(args)...);}
};

template < typename T
         , std::size_t N>
class node_allocator_lazy<T, N, true> {
  public:
  using use_node_allocation = std::true_type;
  using value_type = T;
  using pointer = T*;
  using const_pointer = const T*;
  using reference = T&;
  using const_reference = const T&;
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;
  template<class U>
  struct rebind {
    using other = node_allocator_lazy< U
                                     , sizeof (U)
                                     , !(sizeof (U) < sizeof (char*))>;
  };
  public:
  node_alloc_header* header;
  node_stack stack;
  public:
  node_allocator_lazy(node_alloc_header* p) : header(p) {}
  template<typename U>
  node_allocator_lazy(const node_allocator_lazy< U
                                               , sizeof (U)
                                               , !(sizeof (U) < sizeof (char*))>& alloc)
  : header(alloc.header)
  , stack(header->data, header->size, N)
  {}
  pointer allocate_node()
  {
    char* p = stack.pop(); 
    if (!p)
      throw std::bad_alloc();
    return reinterpret_cast<pointer>(p); 
  }
  pointer allocate(size_type)
  {
    return allocate_node();
  }
  void deallocate_node(pointer p)
  { stack.push(reinterpret_cast<char*>(p)); }
  void deallocate(pointer p, size_type)
  { deallocate_node(p); }
  template<typename U>
  void destroy(U* p) {p->~U();}
  template< typename U, typename... Args>
  void construct(U* p, Args&&... args)
  {::new((void *)p) U(std::forward<Args>(args)...);}
  void swap(node_allocator_lazy& other) noexcept
  {
    std::swap(header, other.header);
    stack.swap(other.stack);
  }
  pointer address(reference x) const noexcept { return std::addressof(x); }
  const_pointer address(const_reference x) const noexcept
  { return std::addressof(x); }
  constexpr std::size_t max_size() const {return 1;}
};

template <typename T>
bool operator==( const node_allocator_lazy<T>& alloc1
               , const node_allocator_lazy<T>& alloc2)
{return alloc1.stack == alloc2.stack;}

template <typename T>
bool operator!=( const node_allocator_lazy<T>& alloc1
               , const node_allocator_lazy<T>& alloc2)
{return !(alloc1 == alloc2);}

}

namespace std {

template <typename T>
struct allocator_traits<rt::node_allocator_lazy<T>> {
  using use_node_allocation = typename rt::node_allocator_lazy<T>::use_node_allocation;
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
  static pointer allocate(allocator_type& a, size_type)
  {return a.allocate_node();}
  static pointer allocate_node(allocator_type& a)
  {return a.allocate_node();}
  static void deallocate( allocator_type& a, pointer p
                        , size_type) {a.deallocate_node(p);}
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

