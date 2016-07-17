#pragma once

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

template <class T>
struct type_support {
  using size_type = std::size_t;
  using pointer = T*;
  using const_pointer = const T*;
  using const_reference = const T&;
  using reference = T&;
  using value_type = T;
  using link_type = std::size_t;
  using void_pointer = void*;
  using const_void_pointer = const void*;
};

template <typename T, typename NodeType>
class node_allocator {
  static_assert( ((sizeof (NodeType)) >= (sizeof (char*)))
               , "node_allocator: incompatible node size.");
  public:
  using node_allocation_only = std::true_type;

  using size_type = typename type_support<T>::size_type;
  using pointer = typename type_support<T>::pointer;
  using const_pointer = typename type_support<T>::const_pointer;
  using reference = typename type_support<T>::reference;
  using const_reference = typename type_support<T>::const_reference;
  using value_type = typename type_support<T>::value_type;
  using link_type = typename type_support<T>::link_type;
  using void_pointer = typename type_support<T>::void_pointer;
  using const_void_pointer = typename type_support<T>::const_void_pointer;

  template<class U>
  struct rebind { using other = node_allocator<U , NodeType>; };
  node_alloc_header* header;
  node_stack<T, std::size_t> stack;
  node_allocator(node_alloc_header* p) : header(p) {}
  // Constructor for the node type with a different pointer type.
  template<typename U, typename K = T>
  node_allocator( const node_allocator<U, NodeType>& alloc
                , typename std::enable_if<is_same_node_type<K, NodeType>::value, void*>::type p = 0)
  : header(alloc.header)
  , stack(header) {}
  template<typename U, typename K = T>
  node_allocator( const node_allocator<U, NodeType>& alloc
                , typename std::enable_if<!is_same_node_type<K, NodeType>::value, void*>::type p = 0)
  : header(alloc.header) {}
  template <typename U = T>
  typename std::enable_if<
    is_same_node_type<U, NodeType>::value, pointer>::type
  allocate_node()
  {
    const auto i = stack.pop(); 
    if (!i)
      throw std::bad_alloc();

    const auto p = reinterpret_cast<link_type*>(stack.header->buffer);
    return reinterpret_cast<pointer>(&p[i]);
  }
  pointer make_pointer(pointer l)
  {
    return l;
  }
  template <typename U = T>
  typename std::enable_if<
    !is_same_node_type<U, NodeType>::value, pointer>::type
  allocate(size_type n, std::allocator<void>::const_pointer hint = 0)
  {
    return std::allocator<T>().allocate(n, hint);
  }
  template <typename U = T>
  typename std::enable_if<std::is_same<U, NodeType>::value>::type
  deallocate_node(pointer p)
  {
    const auto a = reinterpret_cast<link_type*>(stack.header->buffer);
    const auto b = reinterpret_cast<link_type*>(p);
    stack.push(b - a);
  }
  template <typename U = T>
  typename std::enable_if<!std::is_same<U, NodeType>::value>::type
  deallocate(pointer p, size_type n)
  { 
    std::allocator<T>().deallocate(p, n);
  }
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

