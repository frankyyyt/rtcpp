#pragma once

#include <stdexcept>

#define RT_TEST(name) \
  void name(const char* f = #name)

#define RT_CHECK(b) \
  if (!(b)) \
    throw std::runtime_error(f);
 
