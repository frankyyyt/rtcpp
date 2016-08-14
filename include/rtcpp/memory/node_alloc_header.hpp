#pragma once

#include <array>
#include <vector>
#include <limits>

#include "align.hpp"

namespace rt {

template <class, class>
class node_link;

template <class T, class L>
class node_ptr {
  private:
  static constexpr std::size_t R = sizeof (T) / sizeof (L);
  L* ptr {nullptr};
  L idx {};

  public:
  template <class, class>
  friend class node_link;

  L get_idx() const {return idx;}
  using link_type = L;
  using element_type = T;
  template <class U>
  using rebind = node_ptr<U, L>;
  T& operator*()
  { return *reinterpret_cast<T*>(&ptr[idx * R]); }
  const T& operator*() const
  { return *reinterpret_cast<const T*>(&ptr[idx * R]); }
  T* operator->()
  { return reinterpret_cast<T*>(&ptr[idx * R]); }
  const T* operator->() const
  { return reinterpret_cast<const T*>(&ptr[idx * R]); }
  node_ptr() = default;
  node_ptr& operator=(const node_link<T, L>& rhs)
  {
    idx = rhs.idx;
    return *this;
  }
  node_ptr& operator=(const node_ptr<T, L>& rhs)
  {
    idx = rhs.idx;
    ptr = rhs.ptr;
    return *this;
  }
  node_ptr(L* pp, L i) : ptr(pp), idx(i) {}
};

//____________________________________________________
template <class T, class L>
bool operator==( const node_ptr<T, L>& p1
               , const node_ptr<T, L>& p2)
{return p1.get_idx() == p2.get_idx();}

template <class T, class L>
bool operator!=( const node_ptr<T, L>& p1
               , const node_ptr<T, L>& p2)
{return !(p1 == p2);}

template <class T, class L>
class node_link {
  private:
  template <class, class>
  friend class node_ptr;
  //private:
  L idx;
  public:
  L get_idx() const {return idx;}
  using link_type = L;
  using element_type = T;
  template <class U>
  using rebind = node_ptr<U, L>;
  node_link& operator=(const node_ptr<T, L>& rhs)
  {
    idx = rhs.idx;
    return *this;
  }
};

//____________________________________________________
template <class T, class L>
bool operator==( const node_ptr<T, L>& p1
               , const node_link<T, L>& p2)
{return p1.get_idx() == p2.get_idx();}

template <class T, class L>
bool operator!=( const node_ptr<T, L>& p1
               , const node_link<T, L>& p2)
{return !(p1 == p2);}

template <class T, class L>
bool operator==( const node_link<T, L>& p1
               , const node_ptr<T, L>& p2)
{return p1.get_idx() == p2.get_idx();}

template <class T, class L>
bool operator!=( const node_link<T, L>& p1
               , const node_ptr<T, L>& p2)
{return !(p1 == p2);}

//____________________________________________________
template <class L>
class node_ptr_void {
  public:
  using link_type = L;
  using element_type = void;
  template <class U>
  using rebind = node_link<U, L>;
};

// Expects the pointer p to have a suitable alignment for both T and
// L.

template <class T, class L, std::size_t N>
void link_stack(L* p, std::size_t offset)
{
  static constexpr auto SL = sizeof (L);
  static constexpr auto ST = sizeof (T);
  static constexpr auto R = (SL < ST) ? ST / SL : 1;

  for (std::size_t i = 0; i < N; ++i)
    p[i * R] = offset + i;
}

template <class T, class L, std::size_t N>
class node_alloc_header {
  public:
  using pointer = node_ptr<T, L>;
  private:

  static_assert((!std::is_signed<L>::value),
  "node_alloc_header: Incompatible type.");

  static_assert((N - 1 <= std::numeric_limits<L>::max()),
  "node_alloc_header: Incompatible size.");

  static_assert((is_power_of_two<N>::value),
  "node_alloc_header: N must be a power of 2.");

  static constexpr auto SL = sizeof (L);
  static constexpr auto ST = sizeof (T);
  static constexpr auto R = (SL < ST) ? ST / SL : 1;
  static constexpr auto S = N * R; // Size of allocated array.

  L* buffer[5] = {0};
  public:

  static_assert((S >= 2), "node_alloc_header: Invalid N.");

  L* get_base_ptr(L)
  {
    return buffer[0];
  }

  node_alloc_header()
  {
    buffer[0] = new L[S];
    buffer[0][0] = N - 1;
    for (std::size_t i = 1; i < N; ++i)
      buffer[0][i * R] = i - 1;
  }

  ~node_alloc_header() { delete [] buffer[0]; }

  pointer pop()
  {
    auto p = buffer[0]; // Alias

    const L i = p[0];

    if (!i)
      throw std::bad_alloc();

    p[0] = p[i * R];
    return pointer(p, i);
  }

  void push(pointer idx) noexcept
  {
    auto p = buffer[0]; // Alias.
    const auto i = idx.get_idx();
    p[i * R] = p[0];
    p[0] = i;
  }

  bool operator==(const node_alloc_header& rhs) const noexcept
  {return buffer[0] == rhs.buffer[0];}
};

}

