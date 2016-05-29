#pragma once

#include <type_traits>

#include "node_traits.hpp"

namespace rt {

template <typename Alloc>
struct allocator_traits {
  using allocator_type = Alloc;
  using is_always_equal = std::false_type;
  using const_reference = typename Alloc::const_reference;
  using pointer = typename Alloc::pointer;
  using size_type = std::size_t;
  using value_type = typename Alloc::value_type;
  using difference_type = std::ptrdiff_t;
  using const_void_pointer =
    typename std::pointer_traits<pointer>::template
      rebind<const void>;
  using void_pointer =
    typename std::pointer_traits<pointer>::template rebind<void>;
  using const_pointer = typename allocator_type::const_pointer;
  using propagate_on_container_copy_assignment = std::true_type;
  using propagate_on_container_move_assignment = std::true_type;
  using propagate_on_container_swap = std::true_type;
  template<typename U>
  using rebind_alloc =
    typename allocator_type::template rebind<U>::other;
  static allocator_type
    select_on_container_copy_construction(const allocator_type& a)
    {return a;}

  template <typename Alloc2 = allocator_type>
  static typename std::enable_if<
    has_allocate_node<Alloc2>::value, pointer>::type
  allocate_node(Alloc2& a) {return a.allocate_node();}

  template <typename Alloc2 = allocator_type>
  static typename std::enable_if<
    !has_allocate_node<Alloc2>::value, pointer>::type
  allocate_node(Alloc2& a) {return a.allocate(1);}

  template <typename Alloc2 = allocator_type>
  static typename
  std::enable_if<rt::has_allocate_node<Alloc2>::value>::type
  deallocate_node(Alloc2& a, pointer p) {a.deallocate_node(p);}

  template <typename Alloc2 = allocator_type>
  static typename
  std::enable_if<!rt::has_allocate_node<Alloc2>::value>::type
  deallocate_node(Alloc2& a, pointer p) {a.deallocate(p, 1);}

  template<class U>
  static void destroy(allocator_type& a, U* p) {a.destroy(p);}
  template<class U, class... Args >
  static void construct(allocator_type& a, U* p, Args&&... args)
  {a.construct(p, std::forward<Args>(args)...);}
};

}

