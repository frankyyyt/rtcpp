#include <rtcpp/algorithm/max_element.hpp>

namespace rt
{

template <class Iter>
void straight_selection_sort(Iter begin, Iter end)
{
  for (; begin != end; ++begin)
    std::iter_swap(rt::min_element(begin, end), begin);
}

}

