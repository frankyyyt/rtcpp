#include <memory>
#include <limits>
#include <utility>
#include <iterator>
#include <algorithm>
#include <functional>
#include <type_traits>
#include <initializer_list>

#include <rtcpp/memory/allocator_traits.hpp>

#include "tbst.hpp"

/*
  Implements a std::set as a threaded binary search tree. That means
  it does not guarantee logarithmic search time. It is however often
  faster than a balanced implementation as a degenerate tree is very
  rare and there is no balancing overhead.
*/

namespace rt {

template <class T, class Ptr>
class bst_iterator :
  public std::iterator<std::bidirectional_iterator_tag, const T> {
  public:
  Ptr m_p;
  bst_iterator() noexcept : m_p(0) {}
  bst_iterator(Ptr root) noexcept : m_p(root) {}

  auto& operator++() noexcept
  { m_p = tbst::inorder<1>(m_p); return *this; }

  auto operator++(int) noexcept
  { auto tmp(*this); operator++(); return tmp; }

  auto& operator--() noexcept
  { m_p = tbst::inorder<0>(m_p); return *this; }

  auto operator--(int) noexcept
  { auto tmp(*this); operator--(); return tmp; }

  T& operator*() noexcept {return m_p->key;}
  const T& operator*() const noexcept {return m_p->key;}

  friend
  auto operator==( const bst_iterator<T, Ptr>& rhs
                 , const bst_iterator<T, Ptr>& lhs) noexcept
  { return lhs.m_p == rhs.m_p; }

  friend
  auto operator!=( const bst_iterator<T, Ptr>& rhs
                 , const bst_iterator<T, Ptr>& lhs) noexcept
  { return !(lhs == rhs); }

};

template < class T
         , class Compare = std::less<T>
         , class Allocator = std::allocator<T>>
class set {
  public:
  using key_type = T;
  using value_type = T;
  using size_type = std::size_t ;
  using key_compare = Compare;
  using value_compare = Compare;
  using reference = value_type&;
  using const_reference = const value_type&;
  using pointer = typename rt::allocator_traits<Allocator>::pointer;
  using allocator_type = Allocator;
  using const_pointer = typename
    rt::allocator_traits<Allocator>::const_pointer;
  using difference_type = std::ptrdiff_t;
  private:
  using alloc_traits_type = rt::allocator_traits<Allocator>;
  using void_pointer = typename alloc_traits_type::void_pointer;
  using link_type = typename alloc_traits_type::link_type;
  public:
  using node_type = tbst::node<value_type, link_type>;
  private:
  using inner_alloc_type =
    typename alloc_traits_type::template rebind_alloc<node_type>;
  using inner_alloc_traits_type =
    rt::allocator_traits<inner_alloc_type>;
  using node_pointer = typename inner_alloc_traits_type::pointer;
  using const_node_pointer =
    typename inner_alloc_traits_type::const_pointer;
  public:
  using iterator = bst_iterator<T, node_pointer>;
  using const_iterator = iterator;
  using const_reverse_iterator =
    std::reverse_iterator<const_iterator>;
private:
  mutable inner_alloc_type m_inner_alloc;
  node_pointer m_head;
  Compare m_comp;

  void copy(set& rhs) const noexcept
  {
    auto p = m_head;
    auto q = rhs.m_head;

    for (;;) {
      if (!p->template get_null_link<0>()) {
        auto tmp = get_node();
        tbst::attach_node<0>(q, tmp);
      }

      p = tbst::preorder_successor(p);
      q = tbst::preorder_successor(q);

      if (p == m_head)
        break;

      if (!p->template get_null_link<1>()) {
        auto tmp = get_node();
        tbst::attach_node<1>(q, tmp);
      }

      q->key = p->key;
    }
  }

  auto get_node() const
  { return inner_alloc_traits_type::allocate_node(m_inner_alloc); }
  void release_node(node_pointer p) const
  { inner_alloc_traits_type::deallocate_node(m_inner_alloc, p); }

  void safe_construct(node_pointer p, const value_type& key) const
  {
    try {
      inner_alloc_traits_type::construct(m_inner_alloc, std::addressof(p->key), key);
    } catch (...) {
      release_node(p);
      throw;
    }
  }

public:
  set(const Compare& comp, const Allocator& alloc = Allocator())
  : m_inner_alloc(alloc_traits_type::select_on_container_copy_construction(alloc))
  , m_head(get_node())
  , m_comp(comp)
  {
    m_head->link[0] = m_head;
    m_head->link[1] = m_head;
    m_head->template set_link_null<0>();
  }


  explicit set(const Allocator& alloc = Allocator())
  : set(Compare(), alloc) {}

  set(const set& rhs) noexcept
  : m_inner_alloc(inner_alloc_traits_type::select_on_container_copy_construction(rhs.m_inner_alloc))
  , m_head(get_node())
  {
    m_head->link[0] = m_head;
    m_head->link[1] = m_head;
    m_head->template set_link_null<0>();
    clear();
    rhs.copy(*this);
  }

  template <typename InputIt>
  set(InputIt begin, InputIt end, const Compare& comp, const Allocator& alloc = Allocator())
  : m_inner_alloc(alloc_traits_type::select_on_container_copy_construction(alloc))
  , m_head(get_node())
  , m_comp(comp)
  {
    m_head->link[0] = m_head;
    m_head->link[1] = m_head;
    m_head->template unset_link_null<0>();
    m_head->template unset_link_null<1>();
    m_head->template set_link_null<0>();
    insert(begin, end);
  }

  template <typename InputIt>
  set(InputIt begin, InputIt end, const Allocator& alloc = Allocator())
  : set(begin, end, Compare(), alloc) {}

  set( std::initializer_list<T> init
     , const Compare& comp
     , const Allocator& alloc = Allocator())
  : set(std::begin(init), std::end(init), comp, alloc) {}

  set(std::initializer_list<T> init, const Allocator& alloc = Allocator())
  : set(init, Compare(), alloc) {}

  set(set&& rhs) : set<T, Compare, Allocator>() { swap(*this, rhs); }

  ~set() noexcept { clear(); release_node(m_head); }

  auto& operator=(set rhs) noexcept { swap(*this, rhs); return *this; }

  auto& operator=(std::initializer_list<T> init) noexcept
  { clear(); insert(std::begin(init), std::end(init)); return *this; }

  void clear() noexcept
  {
    node_pointer p = m_head;
    for (;;) {
      node_pointer q = tbst::inorder<1>(p);
      if (p != m_head) {
        inner_alloc_traits_type::destroy(m_inner_alloc, &q->key);
        release_node(p);
      }
      if (q == m_head)
        break;
      p = q;
    }
    m_head->link[0] = m_head;
    m_head->link[1] = m_head;
    m_head->template unset_link_null<0>();
    m_head->template unset_link_null<1>();
    m_head->template set_link_null<0>();
  }

  const_iterator begin() const noexcept
  {return const_iterator(tbst::inorder<1>(m_head));}
  const_iterator end() const noexcept {return const_iterator(m_head);}
  auto rbegin() const noexcept {return const_reverse_iterator(end());}
  auto rend() const noexcept {return const_reverse_iterator(begin());}
  auto key_comp() const noexcept {return m_comp;}
  auto value_comp() const noexcept {return m_comp;}
  auto size() const noexcept
  {return static_cast<size_type>(std::distance(begin(), end()));}
  bool empty() const noexcept {return begin() == end();}
  auto get_allocator() const noexcept {return m_inner_alloc;}

  template<typename K>
  size_type count(const K& key) const noexcept
  {
    if (m_head->template get_null_link<0>()) // The tree is empty
      return 0;

    node_pointer p =
      inner_alloc_traits_type::make_ptr(m_inner_alloc,
        m_head->link[0]);

    for (;;) {
      if (m_comp(key, p->key)) {
        if (!p->template get_null_link<0>())
          p = p->link[0];
        else
          return 0;
      } else if (m_comp(p->key, key)) {
        if (!p->template get_null_link<1>())
          p = p->link[1];
        else
          return 0;
      } else {
        return 1;
      }
    }
  }

  template <typename K>
  auto find(const K& key) const
  { return iterator(find_with_parent(m_head, key, m_comp).first); }
  template<typename K>
  auto max_size() const noexcept
  { return std::numeric_limits<size_type>::max(); }

  template <typename K>
  size_type erase(const K& key)
  {
    auto pair = find_with_parent(m_head, key, m_comp);
    if (pair.first == m_head)
      return 0;

    //node_pointer r = tbst::erase_node<1>(
    //  const_cast<node_pointer>(pair.second),
    //  const_cast<node_pointer>(pair.first), m_inner_alloc);
    auto r = tbst::erase_node<1>(pair.second, pair.first);
    release_node(r);
    return 1;
  }

  auto insert(const value_type& key) noexcept
  {
    if (m_head->template get_null_link<0>()) { // The tree is empty
      auto q = get_node();
      safe_construct(q, key);
      tbst::attach_node<0>(m_head, q);
      return std::make_pair(const_iterator(q), true);
    }

    auto p = m_head;
    p = m_head->link[0];
    for (;;) {
      if (m_comp(key, p->key)) {
        if (!p->template get_null_link<0>()) {
          p = p->link[0];
        } else {
          auto q = get_node();
          safe_construct(q, key);
          tbst::attach_node<0>(p, q);
          return std::make_pair(iterator(q), true);
        }
      } else if (m_comp(p->key, key)) {
        if (!p->template get_null_link<1>()) {
          p = inner_alloc_traits_type::make_ptr(m_inner_alloc, p->link[1]);
        } else {
          auto q = get_node();
          safe_construct(q, key);
          tbst::attach_node<1>(p, q);
          return std::make_pair(iterator(q), true);
        }
      } else {
        return std::make_pair(iterator(p), false);
      }
    }
  }

  template<class InputIt>
  void insert(InputIt begin, InputIt end) noexcept
  {
    for (InputIt iter = begin; iter != end; ++iter) {
      auto pair = insert(*iter);
      if (!pair.second)
         continue;
    }
  }

  friend void swap(set& lhs, set& rhs) noexcept
  {
    using std::swap;
    swap(lhs.m_inner_alloc, rhs.m_inner_alloc);
    swap(lhs.m_head       , rhs.m_head);
    swap(lhs.m_comp       , rhs.m_comp);
  }
};

template<typename Key, typename Compare, typename Alloc>
bool operator==( const set<Key, Compare, Alloc>& lhs
               , const set<Key, Compare, Alloc>& rhs) noexcept
{
  const bool b1 = lhs.size() == rhs.size();
  const bool b2 = std::equal(std::begin(lhs), std::end(lhs), std::begin(rhs));
  return b1 && b2;
}

template<typename Key, typename Compare, typename Alloc>
bool operator!=( const set<Key, Compare, Alloc>& lhs
               , const set<Key, Compare, Alloc>& rhs) noexcept
{ return !(lhs == rhs); }

}

