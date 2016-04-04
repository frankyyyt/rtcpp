#pragma once

#include <array>
#include <vector>
#include <utility>
#include <memory>
#include <exception>
#include <type_traits>

#include "node_stack.hpp"
#include "node_traits.hpp"

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
  static constexpr std::size_t reserved() {return node_stack::memory_use;}
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
  char* m_data;
  std::size_t m_size;
  node_stack m_stack;
  std::allocator<T> std_alloc; // Used for array allocations.
  public:
  template <class U>
  node_allocator(U* data, std::size_t size)
  : m_data(reinterpret_cast<char*>(data))
  , m_size(size * sizeof (U))
  {}
  template <class U, std::size_t I>
  explicit node_allocator(std::array<U, I>& arr)
  : node_allocator( reinterpret_cast<char*>(&arr.front())
                  , arr.size() * sizeof (U)) {}
  template <typename Alloc>
  explicit node_allocator(std::vector<char, Alloc>& arr)
  : node_allocator(&arr.front(), arr.size())
  {}
  // Constructor for the node type with a different pointer type.
  template<typename U, typename K = T>
  node_allocator( const node_allocator<U, NodeType>& alloc
                , typename std::enable_if<is_same_node_type<K, NodeType>::value, void*>::type p = 0)
  : m_data(alloc.m_data)
  , m_size(alloc.m_size)
  , m_stack(m_data, m_size, sizeof (T))
  {}
  template<typename U, typename K = T>
  node_allocator( const node_allocator<U, NodeType>& alloc
                , typename std::enable_if<!is_same_node_type<K, NodeType>::value, void*>::type p = 0)
  : m_data(alloc.m_data)
  , m_size(alloc.m_size)
  {}
  template <typename U = T>
  typename std::enable_if<
    is_same_node_type<U, NodeType>::value, pointer>::type
  allocate_node()
  {
    char* p = m_stack.pop(); 
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
  { m_stack.push(reinterpret_cast<char*>(p)); }
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
    m_stack.swap(other.m_stack);
    std::swap(m_data, other.m_data);
    std::swap(m_size, other.m_size);
  }
  pointer address(reference x) const noexcept { return std::addressof(x); }
  const_pointer address(const_reference x) const noexcept
  { return std::addressof(x); }
};

template <typename T, typename K, typename U, typename V>
bool operator==( const node_allocator<T, K>& alloc1
               , const node_allocator<U, V>& alloc2)
{return alloc1.m_stack == alloc2.m_stack;}

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

