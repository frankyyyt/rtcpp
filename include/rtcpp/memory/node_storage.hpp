#pragma once

#include <array>
#include <vector>
#include <limits>
#include <memory>
#include <cassert>
#include <type_traits>

#include "align.hpp"
#include "node_ptr.hpp"

namespace rt {

template < class T // Node type.
         , class I // Index type.
         , std::size_t N // Number of blocks.
         >
class node_storage {
  static_assert((!std::is_signed<I>::value),
  "node_storage: Incompatible type.");
  static_assert((N - 1 <= std::numeric_limits<I>::max()),
  "node_storage: Incompatible size.");
  static_assert((is_power_of_two<N>::value),
  "node_storage: N must be a power of 2.");
  static_assert((N >= 2),
  "node_storage: Number of blocks must be at least 2.");
private:
  static constexpr auto SL = sizeof (I);
  static constexpr auto ST = sizeof (T);
  // Block size in units of link type size.
  static constexpr auto R = (SL < ST) ? ST / SL : 1;
  I free {0}; // I of the next free node.
  std::vector<std::unique_ptr<I[]>> bufs;
  I add_bloc(); // returns the I of the next free node.
  node_storage& operator=(const node_storage&) = delete;
  node_storage(const node_storage&) = delete;
  void swap(node_storage& other);
public:
  using pointer = node_ptr<T, I, N>;
  using const_pointer = const_node_ptr<T, I, N>;
  using void_pointer = void_node_ptr<I>;
  using const_void_pointer = const_void_node_ptr<I>;
  using link_type = node_link<I>;
  using value_type = T;
  using const_iterator = const_pointer;
  using iterator = pointer;

  auto begin() const { return const_pointer(this, 0); }
  auto end() const
  {
    return
      const_pointer( this
                   , static_cast<I>(get_n_blocks() * N));
  }

  auto begin() { return pointer(this, 0); }
  auto end()
  {
    return pointer(this, static_cast<I>(get_n_blocks() * N));
  }

  node_storage() {}
  std::size_t get_n_blocks() const
  {return bufs.size();}
  I* get_base_ptr(I idx)
  { return bufs[idx / N].get(); }
  const I* get_base_ptr(I idx) const
  { return bufs[idx / N].get(); }
  std::size_t get_raw_idx(I idx) const
  {return (idx % N) * R;}
  pointer pop();
  void push(pointer idx) noexcept;
  bool operator==(const node_storage& rhs) const noexcept
  {return bufs == rhs.bufs;}
};

template < class T, class I, std::size_t N>
void node_storage<T, I, N>::push(pointer idx) noexcept
{
  assert(idx.get_strg() == this);
  //if (idx.get_strg() != this) {
  //  std::cout << std::endl;
  //}

  const auto i = idx.get_idx();
  auto b = get_base_ptr(i);
  b[get_raw_idx(i)] = free;
  free = i;
}

template <class T, class I, std::size_t N>
typename node_storage<T, I, N>::pointer
node_storage<T, I, N>::pop()
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

template <class T, class I, std::size_t N>
I node_storage<T, I, N>::add_bloc()
{
    // Add a check to test if the link type is big
    // enough to grow the node pool.
    const auto size = bufs.size();
    bufs.push_back(std::make_unique<I[]>(N * R));
    const auto offset = size * N;
    for (std::size_t i = 1; i < N; ++i)
      bufs.back()[i * R] = static_cast<I>(offset + i - 1);

    bufs.back()[0] = 0;
    return static_cast<I>(bufs.size() * N - 1);
}

} // rt

