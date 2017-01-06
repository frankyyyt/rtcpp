#pragma once

#include <type_traits>

#include "node_traits.hpp"

namespace rt {

template <class T>
struct pointer_traits {
  using pointer = T;
  using element_type = typename T::element_type;
  using difference_type = typename T::difference_type;
  template <class U>
  using rebind = typename T::template rebind<U>;
};

template <class T>
struct pointer_traits<T*> {
  using pointer = T*;
  using element_type = T;
  using difference_type = std::ptrdiff_t;
  template <class U>
  using rebind = U*;
};

template <typename Alloc>
struct allocator_traits {
  using allocator_type = Alloc;
  using is_always_equal = std::false_type;
  using const_reference = typename Alloc::const_reference;
  using pointer = typename Alloc::pointer;
  using size_type = std::size_t;
  using value_type = typename Alloc::value_type;
  using difference_type = std::ptrdiff_t;
  using void_pointer = typename void_pointer_type<Alloc>::type;
  using link_type = typename link_type<Alloc>::type;
  using const_void_pointer =
    typename const_void_pointer_type<Alloc>::const_void_pointer;
  using const_pointer = typename Alloc::const_pointer;
  using propagate_on_container_copy_assignment = std::true_type;
  using propagate_on_container_move_assignment = std::true_type;
  using propagate_on_container_swap = std::true_type;
  template<typename U>
  using rebind_alloc = typename Alloc::template rebind<U>::other;
  static Alloc select_on_container_copy_construction(const Alloc& a)
    {return a;}

  template <typename Alloc2 = Alloc>
  static typename std::enable_if<
    has_allocate_node<Alloc2>::value, pointer>::type
  allocate_node(Alloc2& a) {return a.allocate_node();}

  template <typename Alloc2 = Alloc>
  static typename std::enable_if<
    !has_allocate_node<Alloc2>::value, pointer>::type
  allocate_node(Alloc2& a) {return a.allocate(1);}

  template <typename Alloc2 = Alloc>
  static typename std::enable_if<
    has_link_type<Alloc2>::value, pointer>::type
  make_ptr(Alloc2& a, typename Alloc2::link_type l)
  {return a.make_ptr(l);}

  template <typename Alloc2 = Alloc>
  static typename std::enable_if<
    !has_link_type<Alloc2>::value, pointer>::type
  make_ptr(Alloc2&, typename Alloc2::pointer l)
  {return l;}

  static pointer allocate(Alloc& a, size_type n)
  {return a.allocate(n);}

  template <typename Alloc2 = Alloc>
  static typename
  std::enable_if<rt::has_allocate_node<Alloc2>::value>::type
  deallocate_node(Alloc2& a, pointer p) {a.deallocate_node(p);}

  template <typename Alloc2 = Alloc>
  static typename
  std::enable_if<!rt::has_allocate_node<Alloc2>::value>::type
  deallocate_node(Alloc2& a, pointer p) {a.deallocate(p, 1);}

  static void deallocate(Alloc& a, pointer p, size_type n)
  {a.deallocate(p, n);}

  template<class U>
  static void destroy(Alloc& a, U* p) {a.destroy(p);}
  template<class U, class... Args >
  static void construct(Alloc& a, U* p, Args&&... args)
  {a.construct(p, std::forward<Args>(args)...);}
};

}

