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

namespace rt {

template <class T, class L, bool isNode = is_node_type<T>::value>
struct type_support {
  using size_type = std::size_t;
  using pointer = T*;
  using const_pointer = const T*;
  using const_reference = const T&;
  using reference = T&;
  using value_type = T;
  using void_pointer = void*;
  using const_void_pointer = const void*;
  //using void_pointer = idx_ptr_void<L>;
  //using const_void_pointer = idx_ptr_void<L>;
};

template <class T, class L>
struct type_support<T, L, true> {
  using size_type = std::size_t;
  using pointer = T*;
  using const_pointer = const T*;
  using const_reference = const T&;
  using reference = T&;
  using value_type = T;
  using void_pointer = void*;
  using const_void_pointer = const void*;
};

//template <class T, class L>
//struct type_support<T, L, true> {
//  using size_type = std::size_t;
//  using const_reference = const T&;
//  using reference = T&;
//  using value_type = T;
//  using pointer = idx_ptr<T, L>;
//  using const_pointer = idx_ptr<T, L>;
//  using void_pointer = idx_ptr_void<L>;
//  using const_void_pointer = idx_ptr_void<L>;
//};

template <typename T, typename NodeType, class L = std::size_t>
class node_allocator {
  public:
  using node_allocation_only = std::true_type;

  using size_type = typename type_support<T, L>::size_type;
  using pointer = typename type_support<T, L>::pointer;
  using const_pointer = typename type_support<T, L>::const_pointer;
  using reference = typename type_support<T, L>::reference;
  using const_reference = typename type_support<T, L>::const_reference;
  using value_type = typename type_support<T, L>::value_type;
  using link_type = L;
  using void_pointer = typename type_support<T, L>::void_pointer;
  using const_void_pointer = typename type_support<T, L>::const_void_pointer;

  template<class U>
  struct rebind { using other = node_allocator<U , NodeType, L>; };
  node_alloc_header<L>* header;
  node_stack<T, std::size_t> stack;
  node_allocator(node_alloc_header<L>* p) : header(p) {}
  // Constructor for the node type with a different pointer type.
  template<typename U, typename K = T>
  node_allocator( const node_allocator<U, NodeType, L>& alloc
                , typename std::enable_if<is_same_node_type<K, NodeType>::value, void*>::type p = 0)
  : header(alloc.header)
  , stack(header) {}
  template<typename U, typename K = T>
  node_allocator( const node_allocator<U, NodeType, L>& alloc
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

    const auto p = stack.header->buffer;
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
    const auto a = stack.header->buffer;
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

template <class T, class K, class U, class V, class L>
bool operator==( const node_allocator<T, K, L>& alloc1
               , const node_allocator<U, V, L>& alloc2)
{return alloc1.stack == alloc2.stack;}

template <class T, class K, class U, class V, class L>
bool operator!=( const node_allocator<T, K, L>& alloc1
               , const node_allocator<U, V, L>& alloc2)
{return !(alloc1 == alloc2);}

template <class T, class K, class U, class V, class L>
void swap(rt::node_allocator<T, K, L>& s1, rt::node_allocator<U, V, L>& s2)
{
  s1.swap(s2); // Put some static assert here.
}

}

