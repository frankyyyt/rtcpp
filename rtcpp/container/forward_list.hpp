#pragma once

#include <iterator>
#include <type_traits>
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
class flist_iter :
  public std::iterator<std::forward_iterator_tag, T> {
  private:
  Ptr head;
  public:
  // Make this private.
  Ptr get_internal_ptr() const {return head;}
  flist_iter(Ptr h) : head(h) {}
  flist_iter& operator++()
  {
    head = head->next;
    return *this;
  }
  flist_iter operator++(int)
  {
    flist_iter tmp(*this);
    operator++();
    return tmp;
  }
  T& operator*() const { return head->info; }
  bool operator==(const flist_iter<T, Ptr>& rhs)
  {return head == rhs.head;}
  bool operator!=(const flist_iter<T, Ptr>& rhs)
  {return !(*this == rhs);}
};

template <class T, class Ptr>
class flist_const_iter :
  public std::iterator<std::forward_iterator_tag, T> {
  private:
  Ptr head;
  public:
  // Make this private.
  Ptr get_internal_ptr() const {return head;}
  flist_const_iter(Ptr h) : head(h) {}
  const flist_const_iter& operator++()
  {
    head = head->next;
    return *this;
  }
  flist_const_iter operator++(int)
  {
    flist_const_iter tmp(*this);
    operator++();
    return tmp;
  }
  const T& operator*() const { return head->info; }
  bool operator==(const flist_const_iter<T, Ptr>& rhs)
  {return head == rhs.head;}
  bool operator!=(const flist_const_iter<T, Ptr>& rhs)
  {return !(*this == rhs);}
};

template <typename T, typename Allocator = std::allocator<T>>
class forward_list {
  public:
  using value_type = T;
  using size_type = typename Allocator::size_type;
  using allocator_type = Allocator;
  using difference_type = std::ptrdiff_t;
  using reference = T&;
  using const_reference = T const&;
  using alloct_type = rt::allocator_traits<Allocator>;
  using pointer = typename alloct_type::pointer;
  using const_pointer = typename alloct_type::const_pointer;
  using void_pointer = typename alloct_type::void_pointer;
  using node_type = forward_list_node<T, void_pointer>;
  using inner_alloc_type =
    typename alloct_type::template rebind_alloc<node_type>;
  using inner_alloct_type = typename
    rt::allocator_traits<inner_alloc_type>;
  using node_pointer = typename inner_alloct_type::pointer;
  using const_node_pointer = typename inner_alloct_type::const_pointer;
  using iterator = flist_iter<T, node_pointer>;
  using const_iterator = flist_const_iter<T, const_node_pointer>;
  private:
  inner_alloc_type m_inner_alloc;
  node_type head; // Not requested from the allocator.
  public:
  forward_list(const Allocator& alloc = Allocator()) noexcept
  : m_inner_alloc(alloc)
  {
    head.next = &head;
  }
  iterator begin() noexcept {return iterator(head.next);}
  iterator end() noexcept {return iterator(&head);}
  size_type max_size() const noexcept { m_inner_alloc.max_size();}
  const_iterator begin() const noexcept
  {return const_iterator(head.next);}
  const_iterator end() const noexcept {return const_iterator(&head);}
  const_iterator cbegin() const noexcept
  {return const_iterator(head.next);}
  const_iterator cend() const noexcept {return const_iterator(&head);}
  ~forward_list() { clear(); }
  bool empty() const {return head.next == &head;}
  node_pointer add_node(const T& data)
  {
    node_pointer q = inner_alloct_type::allocate_node(m_inner_alloc);
    safe_construct(q, data);
    return q;
  }
  node_pointer add_node(T&& data)
  {
    node_pointer q = inner_alloct_type::allocate_node(m_inner_alloc);
    safe_construct(q, std::forward<T>(data));
    return q;
  }
  bool push_front(T&& data)
  {
    auto q = add_node(std::forward<T>(data));
    q->next = head.next;
    head.next = q;
  }
  bool push_front(const T& data)
  {
    auto q = add_node(data);
    q->next = head.next;
    head.next = q;
  }
  void safe_construct(node_pointer p, const T& key)
  {
    try {
      inner_alloct_type::construct(m_inner_alloc,
        std::addressof(p->info), key);
    } catch (...) {
      inner_alloct_type::deallocate_node(m_inner_alloc, p);
      throw;
    }
  }
  void safe_construct(node_pointer p, T&& key)
  {
    try {
      inner_alloct_type::construct(m_inner_alloc,
        std::addressof(p->info), std::forward<T>(key));
    } catch (...) {
      inner_alloct_type::deallocate_node(m_inner_alloc, p);
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
        inner_alloct_type::destroy(m_inner_alloc, p2);
        inner_alloct_type::deallocate_node(m_inner_alloc, p2);
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
        insert_after(const_iterator(q), K);
        break;
      }
      q = p;
      p = q->next;
    }

    if (p == &head)
      insert_after(const_iterator(q), K);
  }
  void insert_after(const_iterator pos, const T& K)
  {
    auto q = const_cast<node_pointer>(pos.get_internal_ptr());
    auto p = q->next;
    auto u = add_node(K);
    q->next = u;
    u->next = p;
  }
  void sort() { insertion_sort(std::less<T>()); }
  void insertion_sort() { insertion_sort(std::less<T>()); }
  template <class Compare>
  void insertion_sort(Compare comp)
  {
    node_pointer a = head.next;
    node_pointer b = a->next;
    while (b != &head) {
      node_pointer p = head.next;
      node_pointer q = &head;
      while (p != b) {
        if (comp(b->info, p->info)) {
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
      inner_alloct_type::destroy(m_inner_alloc, p);
      inner_alloct_type::deallocate_node(m_inner_alloc, p);
    }
  }
};

}

