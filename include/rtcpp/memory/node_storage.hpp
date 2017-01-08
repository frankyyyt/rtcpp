#pragma once

#include <array>
#include <vector>
#include <limits>
#include <memory>
#include <cassert>
#include <type_traits>

#include "align.hpp"

namespace rt {

template <class, class, std::size_t>
class node_link;

template <class T, class Index, std::size_t N>
class node_storage;

template <class T, class Index, std::size_t N>
class node_ptr {
  public:
  using storage_type = node_storage<T, Index, N>;
  using difference_type = std::ptrdiff_t;
  private:
  storage_type* storage {nullptr};
  Index idx {};
  public:
  using index_type = Index;
  using element_type = T;
  template <class U>
  using rebind = node_ptr<U, Index, N>;

  Index get_idx() const {return idx;}
  const storage_type* get_storage() const {return storage;}
  T& operator*()
  {
    const auto b = storage->get_base_ptr(idx);
    const auto raw_idx = storage->get_raw_idx(idx);
    return *reinterpret_cast<T*>(&b[raw_idx]);
  }
  const T& operator*() const
  {
    const auto b = storage->get_base_ptr(idx);
    const auto raw_idx = storage->get_raw_idx(idx);
    return *reinterpret_cast<const T*>(&b[raw_idx]);
  }
  T* operator->()
  {
    const auto b = storage->get_base_ptr(idx);
    const auto raw_idx = storage->get_raw_idx(idx);
    return reinterpret_cast<T*>(&b[raw_idx]);
  }
  const T* operator->() const
  {
    const auto b = storage->get_base_ptr(idx);
    const auto raw_idx = storage->get_raw_idx(idx);
    return reinterpret_cast<const T*>(&b[raw_idx]);
  }
  node_ptr(std::nullptr_t p = nullptr) {}
  node_ptr& operator=(const node_link<T, Index, N>& rhs)
  {
    idx = rhs.get_idx();
    return *this;
  }
  node_ptr& operator=(const node_ptr<T, Index, N>& rhs)
  {
    idx = rhs.idx;
    storage = rhs.storage;
    return *this;
  }
  node_ptr(storage_type* pp, Index i) : storage(pp), idx(i) {}
};

//____________________________________________________
template <class T, class Index, std::size_t N>
bool operator==( const node_ptr<T, Index, N>& p1
               , const node_ptr<T, Index, N>& p2)
{return p1.get_idx() == p2.get_idx();}

template <class T, class Index, std::size_t N>
bool operator!=( const node_ptr<T, Index, N>& p1
               , const node_ptr<T, Index, N>& p2)
{return !(p1 == p2);}

template <class T, class Index, std::size_t N>
class node_link {
  private:
  Index idx;
  public:
  Index get_idx() const {return idx;}
  using index_type = Index;
  using element_type = T;
  using difference_type = std::ptrdiff_t;
  template <class U>
  using rebind = node_link<U, Index, N>;
  node_link& operator=(const node_ptr<T, Index, N>& rhs)
  {
    idx = rhs.get_idx();
    return *this;
  }
};

//____________________________________________________
template <class T, class Index, std::size_t N>
bool operator==( const node_ptr<T, Index, N>& p1
               , const node_link<T, Index, N>& p2)
{return p1.get_idx() == p2.get_idx();}

template <class T, class Index, std::size_t N>
bool operator!=( const node_ptr<T, Index, N>& p1
               , const node_link<T, Index, N>& p2)
{return !(p1 == p2);}

template <class T, class Index, std::size_t N>
bool operator==( const node_link<T, Index, N>& p1
               , const node_ptr<T, Index, N>& p2)
{return p1.get_idx() == p2.get_idx();}

template <class T, class Index, std::size_t N>
bool operator!=( const node_link<T, Index, N>& p1
               , const node_ptr<T, Index, N>& p2)
{return !(p1 == p2);}

//____________________________________________________
template <class Index, std::size_t N>
class node_ptr_void {
  public:
  using index_type = Index;
  using element_type = void;
  using difference_type = std::ptrdiff_t;
  template <class U>
  using rebind = node_link<U, Index, N>;
};

template < class T // Node type.
         , class Index // link type.
         , std::size_t N // Number of blocks.
         >
class node_storage {
  static_assert((!std::is_signed<Index>::value),
  "node_storage: Incompatible type.");
  static_assert((N - 1 <= std::numeric_limits<Index>::max()),
  "node_storage: Incompatible size.");
  static_assert((is_power_of_two<N>::value),
  "node_storage: N must be a power of 2.");
  static_assert((N >= 2),
  "node_storage: Number of blocks must be at least 2.");
private:
  static constexpr auto SL = sizeof (Index);
  static constexpr auto ST = sizeof (T);
  // Block size in units of link type size.
  static constexpr auto R = (SL < ST) ? ST / SL : 1;
  Index free {0}; // Index of the next free node.
  std::vector<std::unique_ptr<Index[]>> bufs;
  Index add_bloc(); // returns the index of the next free node.
  node_storage& operator=(const node_storage&) = delete;
  node_storage(const node_storage&) = delete;
  void swap(node_storage& other);
public:
  using pointer = node_ptr<T, Index, N>;
  using const_pointer = pointer;
  using void_pointer = node_ptr_void<Index, N>;
  using const_void_pointer = void_pointer;
  using link_type = node_link<T, Index, N>;
  using value_type = T;

  node_storage() {}
  std::size_t get_n_blocks() const {return bufs.size();}
  Index* get_base_ptr(Index idx) { return bufs[idx / N].get(); }
  const Index* get_base_ptr(Index idx) const { return bufs[idx / N].get(); }
  std::size_t get_raw_idx(Index idx) const {return (idx % N) * R;}
  pointer pop();
  void push(pointer idx) noexcept;
  bool operator==(const node_storage& rhs) const noexcept
  {return bufs == rhs.bufs;}
};

template < class T, class Index, std::size_t N>
void node_storage<T, Index, N>::push(pointer idx) noexcept
{
  assert(idx.get_storage() == this);
  //if (idx.get_storage() != this) {
  //  std::cout << std::endl;
  //}

  const auto i = idx.get_idx();
  auto b = get_base_ptr(i);
  b[get_raw_idx(i)] = free;
  free = i;
}

template <class T, class Index, std::size_t N>
typename node_storage<T, Index, N>::pointer
node_storage<T, Index, N>::pop()
{
  auto i = free;

  if (!i) {
    free = add_bloc();
    i = free;
  }

  const auto b = get_base_ptr(i);
  free = b[get_raw_idx(i)];
  return pointer(this, i);
}

template <class T, class Index, std::size_t N>
Index node_storage<T, Index, N>::add_bloc()
{
    // Add a check to test if the link type is big
    // enough to grow the node pool.
    const auto size = bufs.size();
    bufs.push_back(std::make_unique<Index[]>(N * R));
    const auto offset = size * N;
    for (std::size_t i = 1; i < N; ++i)
      bufs.back()[i * R] = static_cast<Index>(offset + i - 1);

    bufs.back()[0] = 0;
    return static_cast<Index>(bufs.size() * N - 1);
}

} // rt

