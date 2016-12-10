#include <iostream>
#include <algorithm>
#include <exception>

#include <rtcpp/memory/node_storage.hpp>
#include <rtcpp/utility/print.hpp>

template <class T, class L>
void test_node_storage()
{
  constexpr unsigned N = 4;

  rt::node_storage<T, L, N> strg;

  // Initially there must be zero blocks.
  if (strg.get_n_blocks() != 0)
    throw std::runtime_error("get_n_blocks: 0");

  auto p1 = strg.pop();
  *p1 = 10;
  // The last element in the stack must be returned.
  if (p1.get_idx() != 3)
    throw std::runtime_error("pop: 3");

  // After allocating one element we must have one block.
  if (strg.get_n_blocks() != 1)
    throw std::runtime_error("get_n_blocks: 1");

  auto p2 = strg.pop();
  *p2 = 11;
  // The last element in the stack must be returned.
  if (p2.get_idx() != 2)
    throw std::runtime_error("pop: 2");

  auto p3 = strg.pop();
  *p3 = 12;
  if (p3.get_idx() != 1)
    throw std::runtime_error("pop: 1");

  // We still must have only one block.
  if (strg.get_n_blocks() != 1)
    throw std::runtime_error("get_n_blocks: 1");

  // These should trigger another block.
  auto p4 = strg.pop();
  *p4 = 13;
  if (p4.get_idx() != 7)
    throw std::runtime_error("pop: 7");

  // We must have only two blocks now.
  if (strg.get_n_blocks() != 2)
    throw std::runtime_error("get_n_blocks: 2");

  if (*p1 != 10 || *p2 != 11 || *p3 != 12 || *p4 != 13)
    throw std::runtime_error("get_n_blocks: 2");

  // Now we release some memory.
  strg.push(p1);
  strg.push(p2);
  strg.push(p4);

  const auto p5 = strg.pop();
  if (p5.get_idx() != 7)
    throw std::runtime_error("pop: 7");

  const auto p6 = strg.pop();
  if (p6.get_idx() != 2)
    throw std::runtime_error("pop: 2");
}

int main()
{
  try {
    test_node_storage<unsigned char         , unsigned char>();
    test_node_storage<unsigned short        , unsigned short>();
    test_node_storage<unsigned int          , unsigned int>();
    test_node_storage<unsigned long int     , unsigned long int>();
    test_node_storage<unsigned long long int, unsigned long long int>();

    test_node_storage<unsigned char         , unsigned long long int>();
    test_node_storage<unsigned long long int, unsigned char>();

  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
    return 1;
  }
  return 0;
}

