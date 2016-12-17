#pragma once

#include <vector>

#include <rtcpp/utility/timer.hpp>

namespace rt {

template <class C>
void
print_list_bench(const std::vector<typename C::value_type>& data)
{
  C l;
  rt::timer t;
  for (std::size_t i = 0; i < data.size(); ++i)
    l.sorted_insertion(data[i]);
}

}

