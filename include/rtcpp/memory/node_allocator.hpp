#pragma once

#include <utility>
#include <memory>
#include <vector>
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
class node_link;

template <class T, class L>
class node_ptr {
  private:
  static constexpr std::size_t R = sizeof (T) / sizeof (L);
  L* ptr {nullptr};
  L idx {};

  public:
  template <class, class>
  friend class node_link;

  L get_idx() const {return idx;}
  using link_type = L;
  using element_type = T;
  template <class U>
  using rebind = node_ptr<U, L>;
  T& operator*()
  { return *reinterpret_cast<T*>(&ptr[idx * R]); }
  const T& operator*() const
  { return *reinterpret_cast<const T*>(&ptr[idx * R]); }
  T* operator->()
  { return reinterpret_cast<T*>(&ptr[idx * R]); }
  const T* operator->() const
  { return reinterpret_cast<const T*>(&ptr[idx * R]); }
  node_ptr() = default;
  node_ptr& operator=(const node_link<T, L>& rhs)
  {
    idx = rhs.idx;
    return *this;
  }
  node_ptr& operator=(const node_ptr<T, L>& rhs)
  {
    if (this == &rhs)
      return *this;

    idx = rhs.idx;
    ptr = rhs.ptr;
    return *this;
  }
  node_ptr(L* pp, L i) : ptr(pp), idx(i) {}
};

//____________________________________________________
template <class T, class L>
bool operator==( const node_ptr<T, L>& p1
               , const node_ptr<T, L>& p2)
{return p1.get_idx() == p2.get_idx();}

template <class T, class L>
bool operator!=( const node_ptr<T, L>& p1
               , const node_ptr<T, L>& p2)
{return !(p1 == p2);}

template <class T, class L>
class node_link {
  private:
  template <class, class>
  friend class node_ptr;
  //private:
  L idx;
  public:
  L get_idx() const {return idx;}
  using link_type = L;
  using element_type = T;
  template <class U>
  using rebind = node_ptr<U, L>;
  node_link& operator=(const node_ptr<T, L>& rhs)
  {
    idx = rhs.idx;
    return *this;
  }
};

//____________________________________________________
template <class T, class L>
bool operator==( const node_ptr<T, L>& p1
               , const node_link<T, L>& p2)
{return p1.get_idx() == p2.get_idx();}

template <class T, class L>
bool operator!=( const node_ptr<T, L>& p1
               , const node_link<T, L>& p2)
{return !(p1 == p2);}

template <class T, class L>
bool operator==( const node_link<T, L>& p1
               , const node_ptr<T, L>& p2)
{return p1.get_idx() == p2.get_idx();}

template <class T, class L>
bool operator!=( const node_link<T, L>& p1
               , const node_ptr<T, L>& p2)
{return !(p1 == p2);}

//____________________________________________________
template <class L>
class node_ptr_void {
  public:
  using link_type = L;
  using element_type = void;
  template <class U>
  using rebind = node_link<U, L>;
};

}

namespace rt {

template < class T
         , class L = std::size_t
         , std::size_t S = 1024 // Block size.
         , class A = std::allocator<T>
         , bool B = is_node_type<T>::value>
class node_allocator {
  static_assert((is_power_of_two<S>::value),
  "node_allcator: S must be a power of 2.");
  public:
  using node_allocation_only = std::true_type;
  using size_type = typename A::size_type;
  using pointer = typename A::pointer;
  using const_pointer = typename A::const_pointer;
  using reference = typename A::reference;
  using const_reference = typename A::const_reference;
  using value_type = typename A::value_type;
  using void_pointer = node_ptr_void<L>;
  using const_void_pointer = node_ptr_void<L>;
  using link_type = node_link<T, L>;

  template<class U>
  struct rebind { using other = node_allocator<U, L, S, A, is_node_type<U>::value>; };
  A alloc;
  node_allocator(const A& a = A()): alloc(a) {}

  template<class U>
  node_allocator(const node_allocator<U, L, S, A, false>& a)
  : alloc(a.alloc) {}

  pointer allocate(size_type n, const_pointer hint = 0)
  { return alloc.allocate(n, hint); }

  void deallocate(pointer p, size_type n) { alloc.deallocate(p, n); }

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

template <class T, class L, std::size_t S, class A>
class node_allocator<T, L, S, A, true> {
  static_assert((is_power_of_two<S>::value),
  "node_allcator: S must be a power of 2.");
  public:
  using node_allocation_only = std::true_type;
  using size_type = typename A::size_type;
  using pointer = node_ptr<T, L>;
  using const_pointer = node_ptr<T, L>;
  using reference = T&;
  using const_reference = const T&;
  using value_type = T;
  using void_pointer = node_ptr_void<L>;
  using const_void_pointer = node_ptr_void<L>;
  using link_type = node_link<T, L>;

  template<class U>
  struct rebind { using other = node_allocator<U , L, S, A, is_node_type<U>::value>; };

  std::shared_ptr<node_alloc_header<L>> header;
  node_stack<T, L> stack;
  A alloc;

  node_allocator(const A& a = A())
  : header(std::make_shared<node_alloc_header<L>>(S * sizeof (T)))
  , stack(header.get()), alloc(a) {}

  template<class U>
  node_allocator(const node_allocator<U, L, S, A, true>& a)
  : header(a.header), stack(header.get()), alloc(a.alloc) {}

  template<class U>
  node_allocator(const node_allocator<U, L, S, A, false>& a)
  : header(std::make_shared<node_alloc_header<L>>(S * sizeof (T)))
  , stack(header.get()), alloc(a.alloc) {}

  pointer allocate_node()
  {
    const auto i = stack.pop(); 
    if (!i)
      throw std::bad_alloc();

    return pointer(stack.header->buffer, i);
  }

  void deallocate_node(pointer p)
  { stack.push(p.get_idx()); }

  pointer make_pointer(link_type link)
  { return pointer(stack.header->buffer, link.get_idx()); }

  template<class U>
  void destroy(U* p) {p->~U();}
  template<class U, typename... Args>
  void construct(U* p, Args&&... args)
  {::new((void *)p) U(std::forward<Args>(args)...);}
  void swap(node_allocator& other) noexcept
  {
    stack.swap(other.stack);
    std::swap(header, other.header);
    std::swap(alloc, other.alloc);
  }
  pointer address(reference x) const noexcept { return std::addressof(x); }
  const_pointer address(const_reference x) const noexcept
  { return std::addressof(x); }
};

template <class T, class L, std::size_t S, class A, bool B>
bool operator==( const node_allocator<T, L, S, A, B>& alloc1
               , const node_allocator<T, L, S, A, B>& alloc2)
{return alloc1.stack == alloc2.stack;}

template <class T, class L, std::size_t S, class A, bool B>
bool operator!=( const node_allocator<T, L, S, A, B>& alloc1
               , const node_allocator<T, L, S, A, B>& alloc2)
{return !(alloc1 == alloc2);}

template <class T, class L, std::size_t S, class A, bool B>
void swap(rt::node_allocator<T, L, S, A, B>& s1,
          rt::node_allocator<T, L, S, A, B>& s2)
{
  s1.swap(s2); // Put some static assertions here.
}

}

