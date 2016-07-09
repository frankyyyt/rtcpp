#pragma once

#include <memory>
#include <ostream>

  /*

  Definition of a node for a threaded binary search tree and its fundamental
  operations.

  */

namespace rt { namespace tbst {

template <std::size_t I>
struct index_helper;

template <>
struct index_helper<0> {
  static constexpr std::size_t other = 1;
};

template <>
struct index_helper<1> {
  static constexpr std::size_t other = 0;
};

namespace detail {
  constexpr int rbit = 1;
  constexpr int lbit = 2;
  constexpr int in_use_bit = 4;
}

template <typename T, typename Ptr>
struct node {
  typedef T value_type;
  using link_type = typename std::pointer_traits<Ptr>::template
    rebind<node<T, Ptr>>;
  template<class U, class K>
  struct rebind { using other = node<U , K>; };

  link_type link[2];
  unsigned char tag;
  value_type key;
};

template <class T, class Ptr>
std::ostream&
operator<<(std::ostream& os, const node<T, Ptr*>& o)
{
  os << o.key;
  return os;
}

template <std::size_t I>
struct set_link_null;

template <>
struct set_link_null<0> {
  template <typename Ptr>
  static void apply(Ptr p) noexcept
  { p->tag |= detail::lbit; }
};

template <>
struct set_link_null<1> {
  template <typename Ptr>
  static void apply(Ptr p) noexcept
  { p->tag |= detail::rbit; }
};

template <std::size_t I>
struct unset_link_null;

template <>
struct unset_link_null<0> {
  template <typename Ptr>
  static void apply(Ptr p) noexcept
  { p->tag &= ~detail::lbit; }
};

template <>
struct unset_link_null<1> {
  template <typename Ptr>
  static void apply(Ptr p) noexcept
  { p->tag &= ~detail::rbit; }
};

template <typename Node>
void mark_in_use(Node* o)
{ o->tag |= detail::in_use_bit; }

template <typename Node>
void mark_free(Node* o)
{ o->tag &= ~detail::in_use_bit; }

template <typename Node>
inline bool test_in_use(const Node& o)
{ return o.tag & detail::in_use_bit; }

struct is_in_use {
  template <typename Node>
  bool operator()(const Node& o)
  { return o.tag & detail::in_use_bit; }
};

template <std::size_t I>
struct has_null_link;

template <>
struct has_null_link<0> {
  template <typename Ptr>
  static constexpr bool apply(Ptr p) noexcept
  {return p->tag & detail::lbit;}
};

template <>
struct has_null_link<1> {
  template <typename Ptr>
  static constexpr bool apply(Ptr p) noexcept
  {return p->tag & detail::rbit;}
};

template <std::size_t I>
struct get_null_link;

template <>
struct get_null_link<0> {
  template <typename Ptr>
  static constexpr int apply(Ptr p) noexcept
  {
    using namespace detail;
    return p->tag & (lbit | in_use_bit);
  }
};

template <>
struct get_null_link<1> {
  template <typename Ptr>
  static constexpr int apply(Ptr p) noexcept
  {
    using namespace detail;
    return p->tag & (rbit | in_use_bit);
  }
};

template <std::size_t I, typename Ptr> 
Ptr inorder(Ptr p) noexcept
{
  // I = 0: predecessor. I = 1: sucessor.
  const std::size_t O = index_helper<I>::other;
  if (has_null_link<I>::apply(p))
    return p->link[I];

  Ptr q = p->link[I];
  while (!has_null_link<O>::apply(q))
    q = q->link[O];

  return q;
}

template <std::size_t I, typename Ptr> 
Ptr inorder_parent(Ptr p) noexcept
{
  // I = 0: predecessor. I = 1: sucessor.
  const std::size_t O = index_helper<I>::other;
  if (has_null_link<I>::apply(p))
    return p->link[I];

  Ptr pq = p;
  Ptr q = p->link[I];
  while (!has_null_link<O>::apply(q)) {
    pq = q;
    q = q->link[O];
  }

  return pq;
}

template <typename Ptr>
Ptr preorder_successor(Ptr p) noexcept
{
  if (!has_null_link<0>::apply(p))
    return p->link[0];

  if (!has_null_link<1>::apply(p))
    return p->link[1];

  // This is a leaf node.
  Ptr q = p->link[1];
  while (has_null_link<1>::apply(q))
    q = q->link[1];

  return q->link[1];
}

template <std::size_t I, typename Ptr>
void attach_node(Ptr p, Ptr q) noexcept
{
  // Attaches node q on the left of p. Does not check if pointers are valid.
  const std::size_t O = index_helper<I>::other;
  q->link[I] = p->link[I];
  q->tag = get_null_link<O>::apply(q) | get_null_link<I>::apply(p);
  p->link[I] = q;
  p->tag = get_null_link<O>::apply(p);
  q->link[O] = p;
  set_link_null<O>::apply(q);

  if (!has_null_link<I>::apply(q)) {
    Ptr qs = inorder<I>(q);
    qs->link[O] = q;
  }
}

template <std::size_t I, typename Ptr>
Ptr erase_node_lr_non_null(Ptr* linker, Ptr q) noexcept
{
  // I = 0: The inorder predecessor replaces the erased node.
  // I = 1: The inorder sucessor replaces the erased node.
  const std::size_t O = index_helper<I>::other;
  typedef Ptr node_pointer;
  node_pointer u = const_cast<node_pointer>(inorder_parent<I>(q));
  node_pointer s = q->link[I];
  if (u != q)
    s = u->link[O];
  node_pointer p = inorder<O>(q);
  s->link[O] = q->link[O];;
  unset_link_null<O>::apply(s);
  p->link[I] = s;
  if (has_null_link<I>::apply(s))
    set_link_null<O>::apply(u);
  else
    u->link[O] = s->link[I];;
  if (u != q) {
    s->link[I] = q->link[I];;
    unset_link_null<I>::apply(s);
  }
  *linker = s;
  return q;
}

template <std::size_t I, typename Ptr>
Ptr erase_node_one_null(Ptr* linker, Ptr q) noexcept
{
  const std::size_t O = index_helper<I>::other;
  typedef Ptr node_pointer;
  node_pointer u = const_cast<node_pointer>(inorder_parent<O>(q));
  node_pointer s = q->link[O];
  if (u != q)
    s = u->link[I];
  s->link[I] = q->link[I];;
  if (has_null_link<O>::apply(s))
    set_link_null<I>::apply(u);
  else
    u->link[I] = s->link[O];;
  if (u != q) {
    s->link[O] = q->link[O];;
    unset_link_null<O>::apply(s);
  }
  *linker = s;
  return q;
}

template <std::size_t I, typename Ptr>
Ptr erase_node(Ptr pq, Ptr q) noexcept
{
  // p is parent of q. We do not handle the case p = q
  // Returns the erased node to be released elsewhere.
  // WARNING: Still unfinished.
  const std::size_t O = index_helper<I>::other;
  Ptr* linker = &pq->link[O];
  if (pq->link[I] == q)
    linker = &pq->link[I];
  if (!has_null_link<O>::apply(q) && !has_null_link<I>::apply(q))
    return erase_node_lr_non_null<I>(linker, q);

  if (has_null_link<O>::apply(q) && !has_null_link<I>::apply(q))
    return erase_node_one_null<O>(linker, q);

  if (!has_null_link<O>::apply(q) && has_null_link<I>::apply(q))
    return erase_node_one_null<I>(linker, q);

  if (pq->link[O] == q) { // Both links are null
    set_link_null<O>::apply(pq);
    pq->link[O] = q->link[O];
  } else {
    set_link_null<I>::apply(pq);
    pq->link[I] = q->link[I];
  }

  return q;
}

template <class Ptr, class K, class Comp>
std::pair<Ptr, Ptr>
find_with_parent(Ptr head, const K& key, const Comp& comp)
{
  if (has_null_link<0>::apply(head)) // The tree is empty
    return std::make_pair(head, head); // end iterator.

  Ptr u = head; // pointer to the parent pointer.
  Ptr p = head->link[0];
  for (;;) {
    if (comp(key, p->key)) {
      if (!has_null_link<0>::apply(p)) {
        u = p;
        p = p->link[0];
      } else {
        return std::make_pair(head, head);
      }
    } else if (comp(p->key, key)) {
      if (!has_null_link<1>::apply(p)) {
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

