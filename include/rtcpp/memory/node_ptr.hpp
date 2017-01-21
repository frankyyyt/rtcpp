#pragma once

#include <type_traits>

namespace rt {

template <class>
class node_link;

template <class, class, std::size_t>
class node_storage;

template <class T, class I, std::size_t N>
class node_ptr {
public:
  using strg_type = node_storage<T, I, N>;
  using difference_type = std::ptrdiff_t;

private:
  strg_type* strg {nullptr};
  I idx {};

public:
  using index_type = I;
  using element_type = T;
  template <class U>
  using rebind = node_ptr<U, I, N>;

  node_ptr(std::nullptr_t p = nullptr) {}
  node_ptr(strg_type* p, I i) : strg(p), idx(i) {}
  node_ptr(const node_ptr& rhs) = default;
  ~node_ptr() = default;

  auto& operator=(const node_ptr<T, I, N>& rhs)
  { idx = rhs.idx; strg = rhs.strg; return *this; }
  auto& operator=(const node_link<I>& rhs)
  { idx = rhs.get_idx(); return *this; }
  auto get_idx() const {return idx;}
  const auto* get_strg() const {return strg;}

  auto operator++() { ++idx; return *this; }
  auto operator++(int)
  { auto tmp = *this; operator++; return tmp; }

  explicit operator bool () const
  {return strg && idx != 0;}
  auto& operator*()
  {
    const auto b = strg->get_base_ptr(idx);
    const auto raw_idx = strg->get_raw_idx(idx);
    return *reinterpret_cast<T*>(&b[raw_idx]);
  }
  const auto& operator*() const
  {
    const auto b = strg->get_base_ptr(idx);
    const auto raw_idx = strg->get_raw_idx(idx);
    return *reinterpret_cast<const T*>(&b[raw_idx]);
  }
  auto* operator->()
  {
    const auto b = strg->get_base_ptr(idx);
    const auto raw_idx = strg->get_raw_idx(idx);
    return reinterpret_cast<T*>(&b[raw_idx]);
  }
  const auto* operator->() const
  {
    const auto b = strg->get_base_ptr(idx);
    const auto raw_idx = strg->get_raw_idx(idx);
    return reinterpret_cast<const T*>(&b[raw_idx]);
  }
};

//____________________________________________________
template <class T, class I, std::size_t N>
auto operator==( const node_ptr<T, I, N>& p1
               , const node_ptr<T, I, N>& p2)
{
  return p1.get_strg() == p2.get_strg() && 
         p1.get_idx()  == p2.get_idx();
}

template <class T, class I, std::size_t N>
auto operator!=( const node_ptr<T, I, N>& p1
               , const node_ptr<T, I, N>& p2)
{return !(p1 == p2);}

//____________________________________________________
template <class T, class I, std::size_t N>
class const_node_ptr {
public:
  using strg_type = node_storage<T, I, N>;
  using difference_type = std::ptrdiff_t;
private:
  const strg_type* strg {nullptr};
  I idx {};
public:
  using index_type = I;
  using element_type = T;
  template <class U>
  using rebind = const_node_ptr<U, I, N>;

  auto get_idx() const {return idx;}
  const strg_type* get_strg() const {return strg;}

  auto operator++() { ++idx; return *this; }
  auto operator++(int)
  { auto tmp = *this; operator++; return tmp; }

  const auto& operator*() const
  {
    const auto b = strg->get_base_ptr(idx);
    const auto raw_idx = strg->get_raw_idx(idx);
    return *reinterpret_cast<const T*>(&b[raw_idx]);
  }
  const auto* operator->() const
  {
    const auto b = strg->get_base_ptr(idx);
    const auto raw_idx = strg->get_raw_idx(idx);
    return reinterpret_cast<const T*>(&b[raw_idx]);
  }
  const_node_ptr(std::nullptr_t p = nullptr) {}
  const_node_ptr& operator=(const node_link<I>& rhs)
  {
    idx = rhs.get_idx();
    return *this;
  }
  auto& operator=(const const_node_ptr<T, I, N>& rhs)
  {
    idx = rhs.idx;
    strg = rhs.strg;
    return *this;
  }
  const_node_ptr(const strg_type* pp, I i)
  : strg(pp), idx(i) {}
  const_node_ptr(const node_ptr<T, I, N>& pp)
  : strg(pp.get_strg()), idx(pp.get_idx()) {}
};

//____________________________________________________
template <class T, class I, std::size_t N>
auto operator==( const const_node_ptr<T, I, N>& p1
               , const const_node_ptr<T, I, N>& p2)
{return p1.get_idx() == p2.get_idx();}

template <class T, class I, std::size_t N>
auto operator!=( const const_node_ptr<T, I, N>& p1
               , const const_node_ptr<T, I, N>& p2)
{return !(p1 == p2);}

//____________________________________________________
// The following class obeys the requisite of a nullable
// pointer type.

template <class I>
class node_link {
private:
  I m_idx;

public:
  using index_type = I;
  using difference_type = std::ptrdiff_t;

  node_link(std::nullptr_t = nullptr) : m_idx(0) {}

  template <class T, std::size_t N>
  auto& operator=(const node_ptr<T, I, N>& rhs)
  { m_idx = rhs.get_idx(); return *this; }

  template <class T, std::size_t N>
  auto& operator=(const const_node_ptr<T, I, N>& rhs)
  { m_idx = rhs.get_idx(); return *this; }

  friend
  auto operator==(const node_link& p1, const node_link& p2)
  {return p1.m_idx == p2.m_idx;}

  friend
  auto operator!=(const node_link& p1, const node_link& p2)
  {return !(p1 == p2);}

  explicit operator bool () const {return m_idx != 0;}
  auto get_idx() const {return m_idx;}
};

//____________________________________________________
template <class T, class I, std::size_t N>
auto operator==( const node_ptr<T, I, N>& p1
               , const node_link<I>& p2)
{return p1.get_idx() == p2.get_idx();}

template <class T, class I, std::size_t N>
auto operator!=( const node_ptr<T, I, N>& p1
               , const node_link<I>& p2)
{return !(p1 == p2);}

template <class T, class I, std::size_t N>
auto operator==( const node_link<I>& p1
               , const node_ptr<T, I, N>& p2)
{return p1.get_idx() == p2.get_idx();}

template <class T, class I, std::size_t N>
auto operator!=( const node_link<I>& p1
               , const node_ptr<T, I, N>& p2)
{return !(p1 == p2);}

//____________________________________________________
template <class T, class I, std::size_t N>
auto operator==( const const_node_ptr<T, I, N>& p1
               , const node_link<I>& p2)
{return p1.get_idx() == p2.get_idx();}

template <class T, class I, std::size_t N>
auto operator!=( const const_node_ptr<T, I, N>& p1
               , const node_link<I>& p2)
{return !(p1 == p2);}

template <class T, class I, std::size_t N>
auto operator==( const node_link<I>& p1
               , const const_node_ptr<T, I, N>& p2)
{return p1.get_idx() == p2.get_idx();}

template <class T, class I, std::size_t N>
auto operator!=( const node_link<I>& p1
               , const const_node_ptr<T, I, N>& p2)
{return !(p1 == p2);}

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

  auto get_strg() {return m_strg;}
  auto get_node_link() {return m_idx;}

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
  : m_strg(p.get_strg()), m_idx(p.get_node_link())
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

