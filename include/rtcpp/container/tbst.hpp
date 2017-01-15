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

template <class T, class Ptr>
class node {
public:
  using value_type = T;
  using link_type = typename decide_link_type<node, T, Ptr>::type;

private:
  static constexpr unsigned char null_link_bit = 1;
  static constexpr unsigned char in_use_bit = 4;
  unsigned char tag = 0;

public:
  link_type link[2];
  T key;

  template<class U, class K>
  using rebind = node<U , K>;

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

template <std::size_t I, class Ptr> 
Ptr inorder(Ptr p) noexcept
{
  // I = 0: predecessor. I = 1: sucessor.

  if (p->template get_null_link<I>()) {
    p = p->link[I];
    return p;
  }

  Ptr q = p;
  q = p->link[I];
  while (!q->template get_null_link<dir[I]>())
    q = q->link[dir[I]];

  return q;
}

template <std::size_t I, class Ptr>
Ptr inorder_parent(Ptr p) noexcept
{
  // I = 0: predecessor. I = 1: sucessor.

  if (p->template get_null_link<I>()) {
    p = p->link[I];
    return p;
  }

  Ptr pq = p;
  Ptr q = p;
  q = p->link[I];
  while (!q->template get_null_link<dir[I]>()) {
    pq = q;
    q = q->link[dir[I]];
  }

  return pq;
}

template <class Ptr>
Ptr preorder_successor(Ptr p) noexcept
{
  if (!p->template get_null_link<0>()) {
    p = p->link[0];
    return p;
  }

  if (!p->template get_null_link<1>()) {
    p = p->link[1];
    return p;
  }

  // This is a leaf node.
  Ptr q = p;
  q = p->link[1];
  while (q->template get_null_link<1>())
    q = q->link[1];

  q = q->link[1];
  return q;
}

template <std::size_t I, class Ptr>
void attach_node(Ptr p, Ptr q) noexcept
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
    Ptr qs = inorder<I>(q);
    qs->link[dir[I]] = q;
  }
}

template <std::size_t I, class Ptr, class Ptr2>
Ptr2 erase_node_lr_non_null(Ptr* linker, Ptr2 q) noexcept
{
  // I = 0: The inorder predecessor replaces the erased node.
  // I = 1: The inorder sucessor replaces the erased node.

  auto u = inorder_parent<I>(q);
  auto s = q;
  s = q->link[I];
  if (u != q)
    s = u->link[dir[I]];
  auto p = inorder<dir[I]>(q);
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

template <std::size_t I, class Ptr, class Ptr2>
Ptr2 erase_node_one_null(Ptr* linker, Ptr2 q) noexcept
{
  auto u = inorder_parent<dir[I]>(q);
  auto s = q;
  s = q->link[dir[I]];

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

template <std::size_t I, class Ptr>
Ptr erase_node(Ptr pq, Ptr q) noexcept
{
  // p is parent of q. We do not handle the case p = q
  // Returns the erased node to be released elsewhere.
  // WARNING: Still unfinished.
  auto linker = &pq->link[dir[I]];
  if (pq->link[I] == q)
    linker = &pq->link[I];
  if (!q->template get_null_link<dir[I]>() && !q->template get_null_link<I>())
    return erase_node_lr_non_null<I>(linker, q);

  if (q->template get_null_link<dir[I]>() && !q->template get_null_link<I>())
    return erase_node_one_null<dir[I]>(linker, q);

  if (!q->template get_null_link<dir[I]>() && q->template get_null_link<I>())
    return erase_node_one_null<I>(linker, q);

  if (pq->link[dir[I]] == q) { // Both links are null
    pq->template set_link_null<dir[I]>();
    pq->link[dir[I]] = q->link[dir[I]];
  } else {
    pq->template set_link_null<I>();
    pq->link[I] = q->link[I];
  }

  return q;
}

template <class Ptr, class K, class Comp>
std::pair<Ptr, Ptr>
find_with_parent(Ptr head, const K& key, const Comp& comp)
{
  if (head->template get_null_link<0>()) // The tree is empty
    return std::make_pair(head, head); // end iterator.

  Ptr u = head; // pointer to the parent pointer.
  Ptr p = head;
  p = head->link[0];
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

