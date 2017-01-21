#pragma once

#include <type_traits>

// The following class obeys the requisite of a nullable
// pointer type.

namespace rt {

//____________________________________________________
template <class I>
class node_link {
private:
  I m_idx;

public:
  using index_type = I;
  using difference_type = std::ptrdiff_t;

  node_link(I i) : m_idx(i) {}
  node_link(std::nullptr_t = nullptr) : m_idx(0) {}

  friend
  auto operator==(const node_link& p1, const node_link& p2)
  {return p1.m_idx == p2.m_idx;}

  friend
  auto operator!=(const node_link& p1, const node_link& p2)
  {return !(p1 == p2);}

  auto operator++() { ++m_idx; return *this; }
  auto operator++(int)
  { auto tmp = *this; operator++; return tmp; }

  explicit operator bool() const {return m_idx != 0;}

  auto get_idx() const {return m_idx;}
};

//_______________________________________________________
template <class, class, std::size_t>
class node_storage;

template <class T, class I, std::size_t N>
class node_ptr {
public:
  using index_type = I;
  using element_type = T;
  using link_type = node_link<I>;
  using difference_type = std::ptrdiff_t;
  using strg_type = node_storage<T, I, N>;
  template <class U> using rebind = node_ptr<U, I, N>;

private:
  strg_type* m_strg;
  link_type m_link;

public:
  node_ptr(std::nullptr_t p = nullptr)
  : m_strg(nullptr), m_link (nullptr) {}

  node_ptr(strg_type* p, I i) : m_strg(p), m_link(i) {}

  auto& operator=(const node_ptr& rhs)
  { m_link = rhs.m_link; m_strg = rhs.m_strg; return *this; }

  auto& operator=(const node_link<I>& rhs)
  { m_link = rhs; return *this; }

  auto get_link() const {return m_link;}
  const auto* get_strg() {return m_strg;}

  auto operator++() { ++m_link; return *this; }
  auto operator++(int)
  { auto tmp = *this; operator++; return tmp; }

  explicit operator bool() const {return m_strg && m_link;}
  operator link_type () const {return m_link;};

  auto& operator*()
  {
    const auto b = m_strg->get_base_ptr(m_link.get_idx());
    const auto raw_idx = m_strg->get_raw_idx(m_link.get_idx());
    return *reinterpret_cast<T*>(&b[raw_idx]);
  }
  const auto& operator*() const
  {
    const auto b = m_strg->get_base_ptr(m_link.get_idx());
    const auto raw_idx = m_strg->get_raw_idx(m_link.get_idx());
    return *reinterpret_cast<const T*>(&b[raw_idx]);
  }
  auto* operator->()
  {
    const auto b = m_strg->get_base_ptr(m_link.get_idx());
    const auto raw_idx = m_strg->get_raw_idx(m_link.get_idx());
    return reinterpret_cast<T*>(&b[raw_idx]);
  }
  const auto* operator->() const
  {
    const auto b = m_strg->get_base_ptr(m_link.get_idx());
    const auto raw_idx = m_strg->get_raw_idx(m_link.get_idx());
    return reinterpret_cast<const T*>(&b[raw_idx]);
  }

  friend
  auto operator==( const node_ptr& p1 , const node_ptr& p2)
  { return p1.m_strg == p2.m_strg && p1.m_link  == p2.m_link; }

  friend
  auto operator!=( const node_ptr& p1 , const node_ptr& p2)
  {return !(p1 == p2);}
};

//____________________________________________________
template <class T, class I, std::size_t N>
class const_node_ptr {
public:
  using index_type = I;
  using element_type = T;
  using link_type = node_link<I>;
  using difference_type = std::ptrdiff_t;
  using strg_type = node_storage<T, I, N>;
  template <class U> using rebind = const_node_ptr<U, I, N>;

private:
  const strg_type* m_strg;
  link_type m_link;

public:
  const_node_ptr(std::nullptr_t p = nullptr)
  : m_strg(nullptr), m_link(nullptr) {}

  const_node_ptr(const strg_type* pp, I i)
  : m_strg(pp), m_link(i) {}

  const_node_ptr(const node_ptr<T, I, N>& pp)
  : m_strg(pp.get_strg()), m_link(pp.get_link()) {}

  auto get_link() const {return m_link;}
  const strg_type* get_strg() const {return m_strg;}

  explicit operator bool() const {return m_strg && m_link;}
  operator link_type () const {return m_link;};
  auto operator++() { ++m_link; return *this; }
  auto operator++(int)
  { auto tmp = *this; operator++; return tmp; }

  const auto& operator*() const
  {
    const auto b = m_strg->get_base_ptr(m_link.get_idx());
    const auto raw_idx = m_strg->get_raw_idx(m_link.get_idx());
    return *reinterpret_cast<const T*>(&b[raw_idx]);
  }
  const auto* operator->() const
  {
    const auto b = m_strg->get_base_ptr(m_link.get_idx());
    const auto raw_idx = m_strg->get_raw_idx(m_link.get_idx());
    return reinterpret_cast<const T*>(&b[raw_idx]);
  }

  const_node_ptr& operator=(const node_link<I>& rhs)
  { m_link = rhs; return *this; }

  friend
  auto operator==( const const_node_ptr& p1
                 , const const_node_ptr& p2)
  {return p1.m_strg == p2.m_strg && p1.m_link == p2.m_link;}

  friend
  auto operator!=( const const_node_ptr& p1
                 , const const_node_ptr& p2)
  {return !(p1 == p2);}
};

//____________________________________________________
template <class I>
class void_node_ptr {
private:
  void* m_strg;
  node_link<I> m_idx;

public:
  using index_type = I;
  using element_type = void;
  using difference_type = std::ptrdiff_t;

  void_node_ptr(std::nullptr_t = nullptr)
  : m_strg(nullptr), m_idx(nullptr) {}

  void_node_ptr(void* p, I i)
  : m_strg(p), m_idx(i) {}

  auto get_strg() {return m_strg;}
  auto get_link() const {return m_idx;}

  explicit operator bool() const {return m_strg && m_idx;}

  friend
  auto operator==( const void_node_ptr& a
                 , const void_node_ptr& b) 
  { return a.m_strg == b.m_strg && a.m_idx == b.m_idx;}

  friend
  auto operator!=( const void_node_ptr& a
                 , const void_node_ptr& b) 
  { return !(a == b);}
};

template <class I>
class const_void_node_ptr {
private:
  const void* m_strg;
  node_link<I> m_idx;

public:
  using index_type = I;
  using element_type = void;
  using difference_type = std::ptrdiff_t;

  const_void_node_ptr(std::nullptr_t = nullptr)
  : m_strg(nullptr), m_idx(nullptr) {}

  const_void_node_ptr(void_node_ptr<I> p)
  : m_strg(p.get_strg()), m_idx(p.get_link())
  {}

  explicit operator bool() const {return m_strg && m_idx;}

  friend
  auto operator==( const const_void_node_ptr& a
                 , const const_void_node_ptr& b) 
  { return a.m_strg == b.m_strg && a.m_idx == b.m_idx;}

  friend
  auto operator!=( const const_void_node_ptr& a
                 , const const_void_node_ptr& b) 
  { return !(a == b);}
};

} // rt

