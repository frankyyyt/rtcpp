#pragma once

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
  strg_type* storage {nullptr};
  I idx {};
public:
  using index_type = I;
  using element_type = T;
  template <class U>
  using rebind = node_ptr<U, I, N>;

  auto get_idx() const {return idx;}
  const auto* get_storage() const {return storage;}
  auto operator++() { ++idx; return *this; }
  auto operator++(int)
  { auto tmp = *this; operator++; return tmp; }

  auto& operator*()
  {
    const auto b = storage->get_base_ptr(idx);
    const auto raw_idx = storage->get_raw_idx(idx);
    return *reinterpret_cast<T*>(&b[raw_idx]);
  }
  const auto& operator*() const
  {
    const auto b = storage->get_base_ptr(idx);
    const auto raw_idx = storage->get_raw_idx(idx);
    return *reinterpret_cast<const T*>(&b[raw_idx]);
  }
  auto* operator->()
  {
    const auto b = storage->get_base_ptr(idx);
    const auto raw_idx = storage->get_raw_idx(idx);
    return reinterpret_cast<T*>(&b[raw_idx]);
  }
  const auto* operator->() const
  {
    const auto b = storage->get_base_ptr(idx);
    const auto raw_idx = storage->get_raw_idx(idx);
    return reinterpret_cast<const T*>(&b[raw_idx]);
  }
  node_ptr(std::nullptr_t p = nullptr) {}
  auto& operator=(const node_link<I>& rhs)
  {
    idx = rhs.get_idx();
    return *this;
  }
  auto& operator=(const node_ptr<T, I, N>& rhs)
  {
    idx = rhs.idx;
    storage = rhs.storage;
    return *this;
  }
  node_ptr(strg_type* pp, I i) : storage(pp), idx(i) {}
};

//____________________________________________________
template <class T, class I, std::size_t N>
auto operator==( const node_ptr<T, I, N>& p1
               , const node_ptr<T, I, N>& p2)
{return p1.get_idx() == p2.get_idx();}

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
  const strg_type* storage {nullptr};
  I idx {};
public:
  using index_type = I;
  using element_type = T;
  template <class U>
  using rebind = const_node_ptr<U, I, N>;

  auto get_idx() const {return idx;}
  const strg_type* get_storage() const {return storage;}

  auto operator++() { ++idx; return *this; }
  auto operator++(int)
  { auto tmp = *this; operator++; return tmp; }

  const auto& operator*() const
  {
    const auto b = storage->get_base_ptr(idx);
    const auto raw_idx = storage->get_raw_idx(idx);
    return *reinterpret_cast<const T*>(&b[raw_idx]);
  }
  const auto* operator->() const
  {
    const auto b = storage->get_base_ptr(idx);
    const auto raw_idx = storage->get_raw_idx(idx);
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
    storage = rhs.storage;
    return *this;
  }
  const_node_ptr(const strg_type* pp, I i)
  : storage(pp), idx(i) {}
  const_node_ptr(const node_ptr<T, I, N>& pp)
  : storage(pp.get_storage()), idx(pp.get_idx()) {}
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
  void* storage {nullptr};
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
  const void* storage {nullptr};
  I idx {};
public:
  using index_type = I;
  using element_type = void;
  using difference_type = std::ptrdiff_t;
  template <class U>
  using rebind = const_node_ptr_void<I>;
};

} // rt

