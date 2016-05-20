#include <rtcpp/algorithm/max_element.hpp>

namespace rt
{

template <class Iter>
void straight_selection_sort(Iter begin, Iter end)
{
  for (; begin != end; ++begin)
    std::iter_swap(min_or_max_unchecked(begin, end,
      std::less<decltype(*begin)>()), begin);
}

}

