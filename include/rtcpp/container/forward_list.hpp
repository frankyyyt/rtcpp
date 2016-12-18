#pragma once

#include <algorithm>
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
  using value_type = T;
  using link_type = typename std::pointer_traits<Ptr>::template
    rebind<forward_list_node<T, Ptr>>;
  T info;
  link_type next;

  template<class U, class K>
  struct rebind { using other = forward_list_node<U , K>; };
};

template <class T, class Ptr>
class flist_iter :
  public std::iterator<std::forward_iterator_tag, T> {
  private:
  Ptr head;
  public:
  // Make this private.
  Ptr get_internal_ptr() {return head;}
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
  T& operator*() { return head->info; }
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
  Ptr get_internal_ptr() {return head;}
  flist_const_iter(Ptr h) : head(h) {}
  template <class P>
  flist_const_iter(flist_iter<T, P> h) : head(h.get_internal_ptr()) {}
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
  //using const_iterator = flist_const_iter<T, const_node_pointer>;
  using const_iterator = iterator;
  private:
  inner_alloc_type m_inner_alloc;
  node_pointer head; // Not requested from the allocator.
  node_pointer create_node(const T& data);
  public:
  forward_list(const Allocator& alloc = Allocator()) noexcept;
  template<class InputIt>
  forward_list( InputIt first
              , InputIt last
              , const Allocator& alloc = Allocator());
  iterator begin() noexcept
  {
    auto p = head;
    p = head->next;
    return iterator(p);
  }
  iterator end() noexcept {return iterator(head);}
  size_type max_size() const noexcept { m_inner_alloc.max_size();}
  const_iterator begin() const noexcept
  {
    auto p = head;
    p = head->next;
    return const_iterator(p);
  }
  const_iterator end() const noexcept {return const_iterator(head);}
  const_iterator cbegin() const noexcept
  {
    auto p = head;
    p = head->next;
    return const_iterator(p);
  }
  const_iterator cend() const noexcept {return const_iterator(head);}
  ~forward_list()
  {
    clear();
    inner_alloct_type::deallocate_node(m_inner_alloc, head);
  }
  bool empty() const {return head->next == head;}
  node_pointer create_node(T&& data);
  void push_front(T&& data);
  void push_front(const T& data);
  void safe_construct(node_pointer p, const T& key);
  void safe_construct(node_pointer p, T&& key);
  void remove_if(T value);
  void reverse();
  void sorted_insertion(const T& K);
  iterator insert_after(const_iterator pos, const T& K);
  iterator insert_after(const_iterator pos, T&& K);
  iterator insert_after(const_iterator pos, size_type n, const T& K);
  void sort() { insertion_sort(std::less<T>()); }
  void insertion_sort() { insertion_sort(std::less<T>()); }
  template <class Compare>
  void insertion_sort(Compare comp);
  void clear();
};

template <typename T, typename Allocator>
typename forward_list<T, Allocator>::node_pointer
forward_list<T, Allocator>::create_node(const T& data)
{
  auto q = inner_alloct_type::allocate_node(m_inner_alloc);
  safe_construct(q, data);
  return q;
}

template <typename T, typename Allocator>
typename forward_list<T, Allocator>::node_pointer
forward_list<T, Allocator>::create_node(T&& data)
{
  auto q = inner_alloct_type::allocate_node(m_inner_alloc);
  safe_construct(q, std::forward<T>(data));
  return q;
}

template <typename T, typename Allocator>
forward_list<T, Allocator>::
forward_list(const Allocator& alloc) noexcept
: m_inner_alloc(alloc)
, head(create_node(T()))
{
  head->next = head;
}

template <class T, class Allocator>
template<class InputIt>
forward_list<T, Allocator>::forward_list( InputIt begin
                                        , InputIt end
                                        , const Allocator& alloc)
: m_inner_alloc(alloc)
, head(create_node(T()))
{
  head->next = head;
  std::copy(begin, end, std::front_inserter(*this));
}

template <typename T, typename Allocator>
void forward_list<T, Allocator>::push_front(T&& data)
{
  auto q = create_node(std::forward<T>(data));
  q->next = head->next;
  head->next = q;
}

template <typename T, typename Allocator>
void forward_list<T, Allocator>::push_front(const T& data)
{
  auto q = create_node(data);
  q->next = head->next;
  head->next = q;
}

template <typename T, typename Allocator>
void forward_list<T, Allocator>::safe_construct(node_pointer p,
 const T& key)
{
  try {
    inner_alloct_type::construct(m_inner_alloc,
      std::addressof(p->info), key);
  } catch (...) {
    inner_alloct_type::deallocate_node(m_inner_alloc, p);
    throw;
  }
}

template <typename T, typename Allocator>
void forward_list<T, Allocator>::safe_construct(
typename forward_list<T, Allocator>::node_pointer p, T&& key)
{
  try {
    inner_alloct_type::construct(m_inner_alloc,
      std::addressof(p->info), std::forward<T>(key));
  } catch (...) {
    inner_alloct_type::deallocate_node(m_inner_alloc, p);
    throw;
  }
}

template <typename T, typename Allocator>
void forward_list<T, Allocator>::remove_if(T value)
{
  auto* p1 = &head->next;
  auto p2 = head;
  p2 = head->next;
  while (p2 != head) {
    if (p2->info == value) {
      auto tmp = p2->next;
      inner_alloct_type::destroy(m_inner_alloc, &p2->info);
      inner_alloct_type::deallocate_node(m_inner_alloc, p2);
      p2 = tmp;
      *p1 = p2;
      continue;
    }
    p1 = &p2->next;
    p2 = p2->next;
  }
}

template <typename T, typename Allocator>
void forward_list<T, Allocator>::reverse()
{
  auto prev = head;
  auto middle = head;
  while (head->next != head) {
    middle = head->next;
    head->next = middle->next;
    middle->next = prev;
    prev = middle;
  }
  head->next = prev;
}

template <typename T, typename Allocator>
void forward_list<T, Allocator>::sorted_insertion(const T& K)
{
  auto p = head;
  p = head->next;
  auto q = head;
  while (p != head) {
    if (K < p->info) {
      insert_after(const_iterator(q), K);
      break;
    }
    q = p;
    p = q->next;
  }

  if (p == head)
    insert_after(const_iterator(q), K);
}

template <typename T, typename Allocator>
typename forward_list<T, Allocator>::iterator
forward_list<T, Allocator>::insert_after(
  typename forward_list<T, Allocator>::const_iterator pos, const T& K)
{
  //auto q = const_cast<node_pointer>(pos.get_internal_ptr());
  auto q = pos.get_internal_ptr();
  auto p = q->next;
  auto u = create_node(K);
  q->next = u;
  u->next = p;
  return iterator(u);
}

template <typename T, typename Allocator>
typename forward_list<T, Allocator>::iterator
forward_list<T, Allocator>::insert_after(
 typename forward_list<T, Allocator>::const_iterator pos, T&& K)
{
  //auto q = const_cast<node_pointer>(pos.get_internal_ptr());
  auto q = pos.get_internal_ptr();
  auto p = q->next;
  auto u = create_node(std::forward<T>(K));
  q->next = u;
  u->next = p;
  return iterator(u);
}

template <class T, class Allocator>
typename forward_list<T, Allocator>::iterator
forward_list<T, Allocator>::insert_after(
typename forward_list<T, Allocator>::const_iterator pos,
 size_type n, const T& K)
{
  //auto q = const_cast<node_pointer>(pos.get_internal_ptr());
  auto q = pos.get_internal_ptr();
  iterator iter(q);
  while (n--)
    iter = insert_after(iter, K);
  return iter;
}

template <typename T, typename Allocator>
template <class Compare>
void forward_list<T, Allocator>::insertion_sort(Compare comp)
{
  auto a = head;
  a = head->next;
  auto b = a;
  b = a->next;
  while (b != head) {
    auto p = head;
    p = head->next;
    auto q = head;
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

template <typename T, typename Allocator>
void forward_list<T, Allocator>::clear()
{
  while (head->next != head) {
    auto p = head;
    p = head->next;
    head->next = p->next;
    inner_alloct_type::destroy(m_inner_alloc, &p->info);
    inner_alloct_type::deallocate_node(m_inner_alloc, p);
  }
}

}

