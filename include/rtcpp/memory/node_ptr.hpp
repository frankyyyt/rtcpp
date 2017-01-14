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
template <class I>
class node_link {
private:
  I idx;
public:
  using index_type = I;
  using difference_type = std::ptrdiff_t;

  auto get_idx() const {return idx;}
  // Dummy rebind
  template <class U> using rebind = node_link<I>;
  template <class T, std::size_t N>
  auto& operator=(const node_ptr<T, I, N>& rhs)
  { idx = rhs.get_idx(); return *this; }
  template <class T, std::size_t N>
  auto& operator=(const const_node_ptr<T, I, N>& rhs)
  { idx = rhs.get_idx(); return *this; }
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
class node_ptr_void {
private:
  void* strg {nullptr};
  I m_idx {};
public:
  using index_type = I;
  using element_type = void;
  using difference_type = std::ptrdiff_t;
  template <class U>
  using rebind = node_ptr_void<I>;
};

template <class I>
class const_node_ptr_void {
private:
  const void* strg {nullptr};
  I idx {};
public:
  using index_type = I;
  using element_type = void;
  using difference_type = std::ptrdiff_t;
  template <class U>
  using rebind = const_node_ptr_void<I>;
};

} // rt

