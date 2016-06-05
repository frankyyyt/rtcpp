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
  pointer llink;
};

template <class T, class Ptr>
class forward_list_iterator : public std::iterator<std::forward_iterator_tag, T> {
  public:
  typedef forward_list_node<T, Ptr> node_type;
  private:
  Ptr head;
  public:
  forward_list_iterator(Ptr h)
  : head(h)
  {}
  forward_list_iterator& operator++()
  {
    head = head->llink;
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
  using inner_alloc_type =
    typename alloc_traits_type::template rebind_alloc<node_type>;
  using inner_alloc_traits_type = typename
    rt::allocator_traits<inner_alloc_type>;
  using node_pointer = typename inner_alloc_traits_type::pointer;
  typedef forward_list_iterator<T, node_pointer> iterator;
  typedef T value_type;
  private:
  inner_alloc_type m_inner_alloc;
  node_pointer head;
  node_pointer tail;
  public:
  iterator begin() {return iterator(head);}
  iterator end() {return iterator(0);}
  forward_list(const Allocator& alloc = Allocator())
  : m_inner_alloc(alloc)
  , head(nullptr)
  , tail(nullptr)
  { }
  node_pointer add_node(T data)
  {
    node_pointer q = inner_alloc_traits_type::allocate(m_inner_alloc, 1);
    inner_alloc_traits_type::construct(m_inner_alloc, std::addressof(q->info), data);
    q->info = data;
    return q;
  }
  bool push_back(const T& data)
  {
    if (!head) {
      head = add_node(data);
      tail = head;
      return true;
    }
    const node_pointer q = add_node(data);
    if (!q)
      return false;
    tail->llink = q;
    tail = q;
    return true;
  }
  void remove_if(T value)
  {
    node_pointer* p1 = &head;
    node_pointer p2 = head->llink;
    while (p2) {
      if (p2->info == value) {
        node_pointer tmp = p2->llink;
        inner_alloc_traits_type::destroy(m_inner_alloc, p2);
        inner_alloc_traits_type::deallocate(m_inner_alloc, p2, 1);
        p2 = tmp;
        *p1 = p2;
        continue;
      }
      tail = p2;
      p1 = &p2->llink;
      p2 = p2->llink;
    }
  }
};

}

