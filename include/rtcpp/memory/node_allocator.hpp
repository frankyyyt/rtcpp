#pragma once

#include <utility>
#include <memory>
#include <vector>
#include <exception>
#include <type_traits>

#include "node_traits.hpp"
#include "node_storage.hpp"

/*
  This is the prototype allocator I have implemented for the proposal.
  Please, read the proposal in doc/proposal_allocator.pdf
*/

namespace rt {

template < class T
         , class Node
         , class L = std::size_t
         , std::size_t S = 256 // Block size.
         , class A = std::allocator<T>
         , bool B = is_node_type<T>::value
         >
class node_allocator {
  static_assert((is_power_of_two<S>::value),
  "node_allocator: S must be a power of 2.");
  static_assert((std::is_unsigned<L>::value),
  "node_allocator: Link type must be unsigned.");
  static_assert((S - 1 <= std::numeric_limits<L>::max()),
  "node_allocator: Incompatible block size.");
public:
  using node_allocation_only = std::true_type;
  using size_type = typename A::size_type;
  using pointer = typename A::pointer;
  using const_pointer = typename A::const_pointer;
  using reference = typename A::reference;
  using const_reference = typename A::const_reference;
  using value_type = typename A::value_type;

  using link_type = node_link<T, L, S>;
  using node_type =
    typename Node::template rebind< typename Node::value_type
                                  , link_type>::other;

  using storage_type = node_storage<node_type, L, S>;

  template<class U>
  struct rebind {
    using other = node_allocator< U, Node, L, S, A
                                , is_node_type<U>::value>;
  };
  std::shared_ptr<storage_type> header;
  A alloc;
  node_allocator(const A& a = A())
  : header(std::make_shared<storage_type>())
  , alloc(a)
  {}

  template<class U, class V>
  node_allocator(const node_allocator<U, V, L, S, A, false>& a)
  : header(a.header)
  , alloc(a.alloc)
  {}

  pointer allocate(size_type n, const_pointer hint = 0)
  { return alloc.allocate(n, hint); }

  void deallocate(pointer p, size_type n) { alloc.deallocate(p, n); }

  std::size_t get_n_blocks() const
  { return header->get_n_blocks();}

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

template < class T
         , class Node
         , class L
         , std::size_t S
         , class A>
class node_allocator<T, Node, L, S, A, true> {
  static_assert((is_power_of_two<S>::value),
  "node_allcator: S must be a power of 2.");
  public:
  using node_allocation_only = std::true_type;
  using size_type = typename A::size_type;
  using storage_type = node_storage<T, L, S>;
  using pointer = typename storage_type::pointer;
  using const_pointer = typename storage_type::const_pointer;
  using reference = T&;
  using const_reference = const T&;
  using value_type = T;
  using void_pointer = typename storage_type::void_pointer;
  using const_void_pointer = typename storage_type::const_void_pointer;
  using link_type = typename storage_type::link_type;
  using node_type = typename storage_type::value_type;

  static_assert((std::is_same<T, node_type>::value),
  "node_allocator: Error1.");

  static_assert((!std::is_same<T, Node>::value),
  "node_allocator: Error2.");

  static_assert((!std::is_same<node_type, Node>::value),
  "node_allocator: Error3.");

  template<class U>
  struct rebind {
    using other = node_allocator< U, Node, L, S, A
                                , is_node_type<U>::value>;
  };

  std::shared_ptr<storage_type> header;
  A alloc;

  node_allocator(const A& a = A())
  : header(std::make_shared<storage_type>())
  , alloc(a) {}

  template<class U, class V>
  node_allocator(const node_allocator<U, V, L, S, A, true>& a)
  : header(a.header), alloc(a.alloc) {}

  template<class U, class V>
  node_allocator(const node_allocator<U, V, L, S, A, false>& a)
  : header(a.header)
  , alloc(a.alloc) {}

  pointer allocate_node() { return header->pop(); }

  void deallocate_node(pointer p)
  { header->push(p); }

  pointer make_ptr(link_type link)
  {
    return pointer(header.get(), link.get_idx());
  }
  std::size_t get_n_blocks() const
  { return header->get_n_blocks();}
  template<class U>
  void destroy(U* p) {p->~U();}
  template<class U, typename... Args>
  void construct(U* p, Args&&... args)
  {::new((void *)p) U(std::forward<Args>(args)...);}
  void swap(node_allocator& other) noexcept
  {
    std::swap(header, other.header);
    std::swap(alloc, other.alloc);
  }
  pointer address(reference x) const noexcept { return std::addressof(x); }
  const_pointer address(const_reference x) const noexcept
  { return std::addressof(x); }
};

template <class T, class V, class L, std::size_t S, class A, bool B>
bool operator==( const node_allocator<T, V, L, S, A, B>& alloc1
               , const node_allocator<T, V, L, S, A, B>& alloc2)
{return alloc1.header == alloc2.header;}

template <class T, class V, class L, std::size_t S, class A, bool B>
bool operator!=( const node_allocator<T, V, L, S, A, B>& alloc1
               , const node_allocator<T, V, L, S, A, B>& alloc2)
{return !(alloc1 == alloc2);}

}

namespace std {

template <class T, class V, class L, std::size_t S, class A, bool B>
void swap( const rt::node_allocator<T, V, L, S, A, B>& alloc1
         , const rt::node_allocator<T, V, L, S, A, B>& alloc2)
{ alloc1.swap(alloc2); }

}

