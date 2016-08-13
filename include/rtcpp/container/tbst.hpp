#pragma once

#include <memory>
#include <ostream>

#include <rtcpp/memory/allocator_traits.hpp>

  /*

  Definition of a node for a threaded binary search tree and its fundamental
  operations.

  */

namespace rt { namespace tbst {

constexpr int dir[2] = {1, 0};

template <typename T, typename Ptr>
class node {
  public:
  using value_type = T;
  using link_type = typename std::pointer_traits<Ptr>::template
    rebind<node<T, Ptr>>;

  private:
  static constexpr unsigned char null_link_bit = 1;
  static constexpr unsigned char in_use_bit = 4;
  unsigned char tag = 0;

  public:
  link_type link[2];
  T key;

  template<class U, class K>
  struct rebind { using other = node<U , K>; };

  template <int I>
  void set_link_null() noexcept
  { tag |= null_link_bit << I; }

  void set_link_null(int bit) noexcept
  { tag |= bit; }

  template <int I>
  void unset_link_null() noexcept
  { tag &= ~(null_link_bit << I); }

  template <int I>
  void reset_one_link_bit() noexcept
  { tag &= (null_link_bit << I) | in_use_bit; }

  template <int I>
  constexpr unsigned char get_null_link() const noexcept
  { return tag & (null_link_bit << I); }

  void mark_in_use() noexcept { tag |= in_use_bit; }
  void mark_free() noexcept { tag &= ~in_use_bit; }
  bool is_in_use() const noexcept { return tag & in_use_bit; }
};

template <class T, class Ptr>
std::ostream&
operator<<(std::ostream& os, const node<T, Ptr>& o)
{
  os << o.key;
  return os;
}

template <std::size_t I, class Ptr, class A> 
Ptr inorder(Ptr p, A& a) noexcept
{
  // I = 0: predecessor. I = 1: sucessor.

  using alloc_traits = rt::allocator_traits<A>;

  if (p->template get_null_link<I>())
    return alloc_traits::make_pointer(a, p->link[I]);

  Ptr q = alloc_traits::make_pointer(a, p->link[I]);
  while (!q->template get_null_link<dir[I]>())
    q = q->link[dir[I]];

  return q;
}

template <std::size_t I, class Ptr, class A>
Ptr inorder_parent(Ptr p, A& a) noexcept
{
  // I = 0: predecessor. I = 1: sucessor.

  using alloc_traits = rt::allocator_traits<A>;

  if (p->template get_null_link<I>())
    return alloc_traits::make_pointer(a, p->link[I]);

  Ptr pq = p;
  Ptr q = alloc_traits::make_pointer(a, p->link[I]);
  while (!q->template get_null_link<dir[I]>()) {
    pq = q;
    q = q->link[dir[I]];
  }

  return pq;
}

template <class Ptr, class A>
Ptr preorder_successor(Ptr p, A& a) noexcept
{
  using alloc_traits = rt::allocator_traits<A>;

  if (!p->template get_null_link<0>())
    return alloc_traits::make_pointer(a, p->link[0]);

  if (!p->template get_null_link<1>())
    return alloc_traits::make_pointer(a, p->link[1]);

  // This is a leaf node.
  Ptr q = alloc_traits::make_pointer(a, p->link[1]);
  while (q->template get_null_link<1>())
    q = q->link[1];

  return alloc_traits::make_pointer(a, q->link[1]);
}

template <std::size_t I, class Ptr, class A>
void attach_node(Ptr p, Ptr q, A& a) noexcept
{
  // Attaches node q on the left of p.
  // Does not check if pointers are valid.
  q->link[I] = p->link[I];
  q->template reset_one_link_bit<dir[I]>();
  q->set_link_null(p->template get_null_link<I>());
  p->link[I] = q;
  p->template reset_one_link_bit<dir[I]>();
  q->link[dir[I]] = p;
  q->template set_link_null<dir[I]>();

  if (!q->template get_null_link<I>()) {
    Ptr qs = inorder<I>(q, a);
    qs->link[dir[I]] = q;
  }
}

template <std::size_t I, class Ptr, class Ptr2, class A>
Ptr2 erase_node_lr_non_null(Ptr* linker, Ptr2 q, A& a) noexcept
{
  // I = 0: The inorder predecessor replaces the erased node.
  // I = 1: The inorder sucessor replaces the erased node.
  using alloc_traits = rt::allocator_traits<A>;

  auto u = inorder_parent<I>(q, a);
  auto s = alloc_traits::make_pointer(a, q->link[I]);
  if (u != q)
    s = u->link[dir[I]];
  auto p = inorder<dir[I]>(q, a);
  s->link[dir[I]] = q->link[dir[I]];;
  s->template unset_link_null<dir[I]>();
  p->link[I] = s;
  if (s->template get_null_link<I>())
    u->template set_link_null<dir[I]>();
  else
    u->link[dir[I]] = s->link[I];;
  if (u != q) {
    s->link[I] = q->link[I];;
    s->template unset_link_null<I>();
  }
  *linker = s;
  return q;
}

template <std::size_t I, class Ptr, class Ptr2, class A>
Ptr2 erase_node_one_null(Ptr* linker, Ptr2 q, A& a) noexcept
{
  using alloc_traits = rt::allocator_traits<A>;

  auto u = inorder_parent<dir[I]>(q, a);
  auto s = alloc_traits::make_pointer(a, q->link[dir[I]]);

  if (u != q)
    s = u->link[I];
  s->link[I] = q->link[I];;
  if (s->template get_null_link<dir[I]>())
    u->template set_link_null<I>();
  else
    u->link[I] = s->link[dir[I]];;
  if (u != q) {
    s->link[dir[I]] = q->link[dir[I]];;
    s->template unset_link_null<dir[I]>();
  }
  *linker = s;
  return q;
}

template <std::size_t I, class Ptr, class A>
Ptr erase_node(Ptr pq, Ptr q, A& a) noexcept
{
  // p is parent of q. We do not handle the case p = q
  // Returns the erased node to be released elsewhere.
  // WARNING: Still unfinished.
  auto linker = &pq->link[dir[I]];
  if (pq->link[I] == q)
    linker = &pq->link[I];
  if (!q->template get_null_link<dir[I]>() && !q->template get_null_link<I>())
    return erase_node_lr_non_null<I>(linker, q, a);

  if (q->template get_null_link<dir[I]>() && !q->template get_null_link<I>())
    return erase_node_one_null<dir[I]>(linker, q, a);

  if (!q->template get_null_link<dir[I]>() && q->template get_null_link<I>())
    return erase_node_one_null<I>(linker, q, a);

  if (pq->link[dir[I]] == q) { // Both links are null
    pq->template set_link_null<dir[I]>();
    pq->link[dir[I]] = q->link[dir[I]];
  } else {
    pq->template set_link_null<I>();
    pq->link[I] = q->link[I];
  }

  return q;
}

template <class Ptr, class K, class Comp, class A>
std::pair<Ptr, Ptr>
find_with_parent(Ptr head, const K& key, const Comp& comp, A& a)
{
  using alloc_traits = rt::allocator_traits<A>;

  if (head->template get_null_link<0>()) // The tree is empty
    return std::make_pair(head, head); // end iterator.

  Ptr u = head; // pointer to the parent pointer.
  Ptr p = alloc_traits::make_pointer(a, head->link[0]);
  for (;;) {
    if (comp(key, p->key)) {
      if (!p->template get_null_link<0>()) {
        u = p;
        p = p->link[0];
      } else {
        return std::make_pair(head, head);
      }
    } else if (comp(p->key, key)) {
      if (!p->template get_null_link<1>()) {
        u = p;
        p = p->link[1];
      } else {
        return std::make_pair(head, head);
      }
    } else {
      return std::make_pair(p, u); // equivalent element.
    }
  }
}

}
}

