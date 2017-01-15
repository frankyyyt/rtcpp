#include <rtcpp/memory/align.hpp>

#include "rt_test.hpp"

RT_TEST(test_align)
{
  using namespace rt;
  RT_CHECK(is_power_of_two<2>::value)
  RT_CHECK(is_power_of_two<4>::value)
  RT_CHECK(is_power_of_two<8>::value)
  RT_CHECK(is_power_of_two<16>::value)
  RT_CHECK(is_power_of_two<32>::value)
  RT_CHECK(is_power_of_two<64>::value)
  RT_CHECK(is_aligned<8>(16))
  RT_CHECK(align_next<8>(13) == 16)
  RT_CHECK(align_next<4>(1) == 4)
  RT_CHECK(align_next<4>(27) == 28)
  RT_CHECK(align_next<8>(27) == 32)
}

int main()
{
  test_align();
}

