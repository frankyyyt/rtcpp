#pragma once

#include <array>
#include <vector>
#include <utility>
#include <memory>
#include <exception>
#include <type_traits>

#include "node_stack.hpp"
#include "node_traits.hpp"
#include "node_alloc_header.hpp"

/*
  This is the prototype allocator I have implemented for the proposal.
  Please, read the proposal in doc/proposal_allocator.pdf
*/

namespace rt {

template <typename T, typename NodeType>
class node_allocator {
  static_assert( ((sizeof (NodeType)) >= (sizeof (char*)))
               , "node_allocator: incompatible node size.");
  public:
  using size_type = std::size_t;
  using node_allocation_only = std::true_type;
  using pointer = T*;
  using const_pointer = const T*;
  using const_reference = const T&;
  using reference = T&;
  using value_type = T;
  template<class U>
  struct rebind { using other = node_allocator<U , NodeType>; };
  public:
  node_alloc_header* header;
  node_stack stack;
  std::allocator<T> std_alloc; // Used for array allocations.
  public:
  node_allocator(node_alloc_header* p) : header(p) {}
  // Constructor for the node type with a different pointer type.
  template<typename U, typename K = T>
  node_allocator( const node_allocator<U, NodeType>& alloc
                , typename std::enable_if<is_same_node_type<K, NodeType>::value, void*>::type p = 0)
  : header(alloc.header)
  , stack(header, sizeof (T)) {}
  template<typename U, typename K = T>
  node_allocator( const node_allocator<U, NodeType>& alloc
                , typename std::enable_if<!is_same_node_type<K, NodeType>::value, void*>::type p = 0)
  : header(alloc.header) {}
  template <typename U = T>
  typename std::enable_if<
    is_same_node_type<U, NodeType>::value, pointer>::type
  allocate_node()
  {
    char* p = stack.pop(); 
    if (!p)
      throw std::bad_alloc();
    return reinterpret_cast<pointer>(p); 
  }
  template <typename U = T>
  typename std::enable_if<
    !is_same_node_type<U, NodeType>::value, pointer>::type
  allocate(size_type n, std::allocator<void>::const_pointer hint = 0)
  {return std_alloc.allocate(n, hint);}
  template <typename U = T>
  typename std::enable_if<std::is_same<U, NodeType>::value>::type
  deallocate_node(pointer p)
  { stack.push(reinterpret_cast<char*>(p)); }
  template <typename U = T>
  typename std::enable_if<!std::is_same<U, NodeType>::value>::type
  deallocate(pointer p, size_type n)
  { std_alloc.deallocate(p, n); }
  template<typename U>
  void destroy(U* p) {p->~U();}
  template< typename U, typename... Args>
  void construct(U* p, Args&&... args)
  {::new((void *)p) U(std::forward<Args>(args)...);}
  void swap(node_allocator& other) noexcept
  {
    stack.swap(other.stack);
    std::swap(header, other.header);
  }
  pointer address(reference x) const noexcept { return std::addressof(x); }
  const_pointer address(const_reference x) const noexcept
  { return std::addressof(x); }
};

template <typename T, typename K, typename U, typename V>
bool operator==( const node_allocator<T, K>& alloc1
               , const node_allocator<U, V>& alloc2)
{return alloc1.stack == alloc2.stack;}

template <typename T, typename K, typename U, typename V>
bool operator!=( const node_allocator<T, K>& alloc1
               , const node_allocator<U, V>& alloc2)
{return !(alloc1 == alloc2);}

template <typename T, typename K, typename U, typename V>
void swap(rt::node_allocator<T, K>& s1, rt::node_allocator<U, V>& s2)
{
  s1.swap(s2); // Put some static assert here.
}

}

