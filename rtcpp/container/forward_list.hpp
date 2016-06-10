#pragma once

#include <iterator>
#include <memory>

#include <rtcpp/memory/allocator_traits.hpp>

/*

  Work in progress.

*/

namespace rt {

template <class T, class Ptr>
struct forward_list_node {
  using pointer = typename std::pointer_traits<Ptr>::template
    rebind<forward_list_node<T, Ptr>>;
  T info;
  pointer next;
};

template <class T, class Ptr>
class forward_list_iterator :
  public std::iterator<std::forward_iterator_tag, T> {
  public:
  typedef forward_list_node<T, Ptr> node_type;
  private:
  Ptr head;
  public:
  forward_list_iterator(Ptr h) : head(h) {}
  forward_list_iterator& operator++()
  {
    head = head->next;
    return *this;
  }
  forward_list_iterator operator++(int)
  {
    forward_list_iterator tmp(*this);
    operator++();
    return tmp;
  }
  T operator*() const { return head->info; }
  bool operator==(const forward_list_iterator<T, Ptr>& rhs)
  {return head == rhs.head;}
  bool operator!=(const forward_list_iterator<T, Ptr>& rhs)
  {return !(*this == rhs);}
};

template <typename T, typename Allocator = std::allocator<T>>
class forward_list {
  private:
  using allocator_type = Allocator;
  using alloc_traits_type = rt::allocator_traits<allocator_type>;
  using void_pointer = typename alloc_traits_type::void_pointer;
  public:
  using node_type = forward_list_node<T, void_pointer>;
  private:
  using inner_alloc_type =
    typename alloc_traits_type::template rebind_alloc<node_type>;
  using inner_alloc_traits_type = typename
    rt::allocator_traits<inner_alloc_type>;
  using node_pointer = typename inner_alloc_traits_type::pointer;
  using const_node_pointer = typename
    inner_alloc_traits_type::const_pointer;
  public:
  using iterator = forward_list_iterator<T, node_pointer>;
  using const_iterator = forward_list_iterator<T, const_node_pointer>;
  using value_type = T;
  using size_type = typename Allocator::size_type;
  private:
  inner_alloc_type m_inner_alloc;
  node_type head;
  public:
  iterator begin() {return iterator(head.next);}
  iterator end() {return iterator(&head);}
  size_type max_size() const { m_inner_alloc.max_size();}
  const_iterator begin() const {return const_iterator(head.next);}
  const_iterator end() const {return const_iterator(&head);}
  forward_list(const Allocator& alloc = Allocator())
  : m_inner_alloc(alloc)
  {
    head.next = &head;
  }
  ~forward_list() { clear(); }
  bool empty() const {return head.next == &head;}
  node_pointer add_node(T data)
  {
    node_pointer q =
      inner_alloc_traits_type::allocate_node(m_inner_alloc);
    safe_construct(q, data);
    return q;
  }
  bool push_front(const T& data)
  {
    auto q = add_node(data);
    q->next = head.next;
    head.next = q;
  }
  void safe_construct(node_pointer p, const value_type& key)
  {
    try {
      inner_alloc_traits_type::construct(m_inner_alloc,
        std::addressof(p->info), key);
    } catch (...) {
      inner_alloc_traits_type::deallocate_node(m_inner_alloc, p);
      throw;
    }
  }

  void remove_if(T value)
  {
    node_pointer* p1 = &head.next;
    node_pointer p2 = head.next;
    while (p2 != &head) {
      if (p2->info == value) {
        node_pointer tmp = p2->next;
        inner_alloc_traits_type::destroy(m_inner_alloc, p2);
        inner_alloc_traits_type::deallocate_node(m_inner_alloc, p2);
        p2 = tmp;
        *p1 = p2;
        continue;
      }
      p1 = &p2->next;
      p2 = p2->next;
    }
  }
  void reverse()
  {
    node_pointer prev = &head;
    while (head.next != &head) {
      node_pointer middle = head.next;
      head.next = middle->next;
      middle->next = prev;
      prev = middle;
    }
    head.next = prev;
  }
  void sorted_insertion(const T& K)
  {
    node_pointer p = head.next;
    node_pointer q = &head;
    while (p != &head) {
      if (K < p->info) {
        insert_after(q, K);
        break;
      }
      q = p;
      p = q->next;
    }

    if (p == &head)
      insert_after(q, K);
  }
  void insert_after(node_pointer q, const T& K)
  {
      auto p = q->next;
      auto u = add_node(K);
      q->next = u;
      u->next = p;
  }
  void insertion_sort()
  {
    node_pointer a = head.next;
    node_pointer b = a->next;
    while (b != &head) {
      node_pointer p = head.next;
      node_pointer q = &head;
      while (p != b) {
        if (b->info < p->info) {
          q->next = b;
          a->next = b->next;
          b->next = p;
          b = a;
          break;
        }
        q = p;
        p = q->next;
      }
      a = b;
      b = a->next;
    }
  }
  void clear()
  {
    while (head.next != &head) {
      auto p = head.next;
      head.next = p->next;
      inner_alloc_traits_type::destroy(m_inner_alloc, p);
      inner_alloc_traits_type::deallocate_node(m_inner_alloc, p);
    }
  }
};

}

