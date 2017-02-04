#pragma once

#include <memory>
#include <vector>
#include <utility>
#include <exception>
#include <type_traits>

#include "node_traits.hpp"
#include "node_storage.hpp"

namespace rt {

template < class T
         , class Node
         , class Index = std::size_t
         , std::size_t S = 256 // Block size.
         , class A = std::allocator<T>
         >
class node_allocator {
  static_assert((is_power_of_two<S>::value),
  "node_allocator: S must be a power of 2.");
  static_assert((std::is_unsigned<Index>::value),
  "node_allocator: Link type must be unsigned.");
  static_assert((S - 1 <= std::numeric_limits<Index>::max()),
  "node_allocator: Incompatible block size.");
public:
  using link_type = node_link<Index>;
  using node_type =
    typename Node::template rebind< typename Node::value_type
                                  , link_type>;

  using storage_type = node_storage<node_type, Index, S>;

  using size_type =
    typename if_type< std::is_same<T, node_type>::value
                    , Index , std::size_t>::type;

  using pointer =
    typename if_type< std::is_same<T, node_type>::value
                    , typename storage_type::pointer
                    , typename A::pointer>::type;

  using const_pointer =
    typename if_type< std::is_same<T, node_type>::value
                    , typename storage_type::const_pointer
                    , typename A::const_pointer>::type;

  using reference = typename A::reference;
  using const_reference = typename A::const_reference;
  using value_type = typename A::value_type;

  using iterator = typename storage_type::iterator;
  using const_iterator = typename storage_type::const_iterator;

  template<class U>
  struct rebind {
    using other = node_allocator< U, Node, Index, S, A>;
  };

  std::shared_ptr<storage_type> header;
  A alloc;
  node_allocator(const A& a = A())
  : header(std::make_shared<storage_type>())
  , alloc(a)
  {}

  auto get_node_storage() const {return header;}

  auto begin() { return header->begin(); }
  auto end() { return header->end(); }

  template<class U, class V>
  node_allocator(const node_allocator<U, V, Index, S, A>& a)
  : header(a.header)
  , alloc(a.alloc)
  {}

  template <class T2 = T>
  typename std::enable_if<
    !std::is_same<T2, node_type>::value, pointer>::type
  allocate(size_type n, const_pointer hint = 0)
  { return alloc.allocate(n, hint); }

  template <class T2 = T>
  typename std::enable_if<
    std::is_same<T2, node_type>::value, pointer>::type
  allocate_node() { return header->pop(); }

  template <class T2 = T>
  typename std::enable_if<
    !std::is_same<T2, node_type>::value>::type
  deallocate(pointer p, size_type n)
  { return alloc.deallocate(p, n); }

  template <class T2 = T>
  typename std::enable_if<
    std::is_same<T2, node_type>::value>::type
  deallocate_node(pointer p)
  { return header->push(p); }

  std::size_t get_n_blocks() const { return header->get_n_blocks();}

  template<class U>
  void destroy(U* p) {p->~U();}

  template<class U, typename... Args>
  void construct(U* p, Args&&... args)
  {::new((void *)p) U(std::forward<Args>(args)...);}

  void swap(node_allocator& other) noexcept
  { std::swap(alloc, other.alloc); }

  pointer address(reference x) const noexcept { return std::addressof(x); }

  const_pointer address(const_reference x) const noexcept
  { return std::addressof(x); }
};

template <class T, class V, class Index, std::size_t S, class A>
bool operator==( const node_allocator<T, V, Index, S, A>& alloc1
               , const node_allocator<T, V, Index, S, A>& alloc2)
{return alloc1.header == alloc2.header;}

template <class T, class V, class Index, std::size_t S, class A>
bool operator!=( const node_allocator<T, V, Index, S, A>& alloc1
               , const node_allocator<T, V, Index, S, A>& alloc2)
{return !(alloc1 == alloc2);}

}

namespace std {

template <class T, class V, class Index, std::size_t S, class A>
void swap( const rt::node_allocator<T, V, Index, S, A>& alloc1
         , const rt::node_allocator<T, V, Index, S, A>& alloc2)
{ alloc1.swap(alloc2); }

}

